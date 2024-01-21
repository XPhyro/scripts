// @LDFLAGS -lX11 -lXrandr

// C++
#include <algorithm>
#include <csignal>
#include <cstdlib>
#include <exec_info.hpp>
#include <filesystem>
#include <fstream>
#include <future>
#include <iostream>
#include <optional>
#include <ranges>
#include <string_view>
#include <type_traits>
#include <unordered_set>
#include <vector>

// C
#include <cstdio>
#include <fcntl.h>
#include <sys/inotify.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

// C++ libraries
#include <xph/auto_charptr.hpp>
#include <xph/die.hpp>
#include <xph/iteratorutil.hpp>
#include <xph/lexical_cast.hpp>
#include <xph/mathutil.hpp>
#include <xph/sysutil.hpp>

// C libraries
#include <xph/ioutil.h>

// third-party
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/extensions/Xrandr.h>

std::string lock_file;
static Display* display;
static std::vector<Window> windows;

DEFINE_EXEC_INFO();

struct Options {
private:
    static const constexpr double k_default_alpha = 1.0;
    static const constexpr double k_default_frame_rate = 100.0;
    static const constexpr double k_default_lerp_factor = 0.10;

public:
    std::string_view alpha_path;
    double alpha = k_default_alpha;
    bool exit_if_none_selected = false;
    std::chrono::duration<double> frame_time{ 1.0 / k_default_frame_rate };
    std::string_view lock_path;
    std::unordered_set<std::string> ignored_monitors;
    bool ignore_primary = false;
    double lerp_factor = k_default_lerp_factor;
    std::unordered_set<std::string> selected_monitors;

private:
    std::string_view exec_name;

public:
    Options(void) = delete;

    Options(std::string_view exec_name, int& argc, char**& argv) : exec_name(exec_name)
    {
        parse_args(argc, argv);
    }

    void help(void)
    {
        std::cout
            << "Usage: " << exec_name << " [OPTION...] [NAME...]\n"
            << "Blank monitors.\n"
               "\n"
               "If no NAME is provided, all monitors are blanked.\n"
               "\n"
               "  -A PATH      listen for alpha changes in PATH. listening is done via inotify.\n"
               "  -a ALPHA     set alpha of blinds to ALPHA. default is "
            << k_default_alpha
            << ".\n"
               "  -e           immediately exit if no monitors are blanked\n"
               "  -f FPS       set frame rate of alpha interpolation to FPS. default is "
            << k_default_frame_rate
            << ".\n"
               "  -h           display this help and exit\n"
               "  -l PATH      path to lock file. default is \"${TMPDIR:-/tmp}/"
            << exec_name
            << ".lock\"\n"
               "  -m NAME      don't blank monitor with name NAME, can be given multiple times\n"
               "  -p           don't blank primary monitor\n"
               "  -t FACTOR    set alpha interpolation factor to FACTOR. default is "
            << k_default_lerp_factor << ".\n";
        std::exit(EXIT_SUCCESS);
    }

    void error(void)
    {
        std::cerr << "Try '" << exec_name << " -h' for more information.\n";
        std::exit(EXIT_FAILURE);
    }

    void parse_args(int& argc, char**& argv)
    {
        for (int i; (i = getopt(argc, argv, "A:a:ef:hl:m:pt:")) != -1;) {
            switch (i) {
                case 'A':
                    alpha_path = optarg;
                    break;
                case 'a':
                    alpha = xph::lexical_cast<decltype(optarg), decltype(alpha)>(optarg);
                    break;
                case 'e':
                    exit_if_none_selected = true;
                    break;
                case 'f':
                    frame_time = decltype(frame_time)(
                        1.0 /
                        xph::lexical_cast<decltype(optarg), decltype(frame_time)::rep>(optarg));
                    break;
                case 'h':
                    help();
                    break;
                case 'l':
                    lock_path = optarg;
                    break;
                case 'm':
                    ignored_monitors.emplace(optarg);
                    break;
                case 'p':
                    ignore_primary = true;
                    break;
                case 't':
                    lerp_factor =
                        xph::lexical_cast<decltype(optarg), decltype(lerp_factor)>(optarg);
                    break;
                default:
                    error();
                    break;
            }
        }

        argc -= optind;
        argv += optind;

        std::ranges::copy(std::views::counted(argv, argc), xph::inserter(selected_monitors));
    }
};

void cleanup(void)
{
    for (auto& window : windows)
        XDestroyWindow(display, window);
    XCloseDisplay(display);
    std::filesystem::remove(lock_file);
}

[[noreturn]] void terminate(void)
{
    cleanup();
    std::exit(EXIT_SUCCESS);
}

template <typename... Ts>
[[noreturn]] void die(const Ts&... args)
{
    cleanup();
    xph::die(args...);
}

void handle_signals([[maybe_unused]] int sig)
{
    terminate();
}

std::optional<int> force_single_instance(const Options& options)
{
    if (!options.lock_path.empty()) {
        lock_file = options.lock_path;
    } else {
        const auto tmpdir = std::getenv("TMPDIR");
        lock_file = tmpdir ? tmpdir : "/tmp";

        lock_file.reserve(lock_file.size() + 1 + xph::exec_name.size() + 5);
        lock_file += "/";
        lock_file += xph::exec_name;
        lock_file += ".lock";
    }

    if (std::ifstream ifl(lock_file); ifl.is_open()) {
        pid_t pid;
        ifl >> pid;
        kill(pid, SIGTERM);
        ifl.close();
        return { EXIT_SUCCESS };
    } else if (std::ofstream ofl(lock_file); ofl.is_open()) {
        pid_t pid = getpid();
        ofl << pid;
        return std::nullopt;
    } else {
        return { EXIT_FAILURE };
    }
}

void set_window_alpha(Window window, double alpha)
{
    unsigned long opacity = 0xFFFFFFFFul * alpha;
    auto opacity_atom = XInternAtom(display, "_NET_WM_WINDOW_OPACITY", False);
    XChangeProperty(display,
                    window,
                    opacity_atom,
                    XA_CARDINAL,
                    32,
                    PropModeReplace,
                    reinterpret_cast<unsigned char*>(&opacity),
                    1L);
}

void create_windows(const Options& options)
{
    xph::die_if(!(display = XOpenDisplay(NULL)), "unable to open display");

    auto default_screen = DefaultScreen(display);
    auto root_window = RootWindow(display, default_screen);

    XRRScreenResources* screen_resources = XRRGetScreenResources(display, root_window);
    xph::die_if(!screen_resources, "unable to get screen resources");

    RROutput primary_output = XRRGetOutputPrimary(display, root_window);

    for (decltype(screen_resources->noutput) i = 0; i < screen_resources->noutput; ++i) {
        if (options.ignore_primary && screen_resources->outputs[i] == primary_output)
            continue;

        auto output_info =
            XRRGetOutputInfo(display, screen_resources, screen_resources->outputs[i]);
        if (!output_info) {
            std::cerr << xph::exec_name << ": unable to get information for monitor " << i << '\n';
            continue;
        }

        if (output_info->connection || options.ignored_monitors.contains(output_info->name) ||
            (!options.selected_monitors.empty() &&
             !options.selected_monitors.contains(output_info->name)))
            goto next;

        XRRCrtcInfo* crtc_info;
        crtc_info = XRRGetCrtcInfo(display, screen_resources, output_info->crtc);
        if (!crtc_info) {
            std::cerr << xph::exec_name << ": unable to get information for monitor " << i << '\n';
            goto next;
        }

        windows.push_back(XCreateSimpleWindow(display,
                                              root_window,
                                              crtc_info->x,
                                              crtc_info->y,
                                              crtc_info->width,
                                              crtc_info->height,
                                              0,
                                              0,
                                              0));

        XRRFreeCrtcInfo(crtc_info);
next:
        XRRFreeOutputInfo(output_info);
    }
    XRRFreeScreenResources(screen_resources);

    for (auto& window : windows) {
        XSetWindowAttributes window_attributes;
        window_attributes.override_redirect = True;
        XChangeWindowAttributes(display, window, CWOverrideRedirect, &window_attributes);

        XMapWindow(display, window);
        XFlush(display);

        set_window_alpha(window, options.alpha);
        XSetWindowBackground(display, window, BlackPixel(display, default_screen));
        XClearWindow(display, window);
        XFlush(display);
    }
}

[[nodiscard]] bool watch_alpha(const char* path)
{
    const constexpr auto eventsize = sizeof(struct inotify_event);
    const constexpr auto buflen = (eventsize + 16) * 1024;

    const auto fd = inotify_init();
    if (fd < 0) {
        perror("inotify_init");
        return false;
    }

    const auto wd = inotify_add_watch(fd, path, IN_CREATE | IN_MODIFY | IN_MOVE_SELF);

    char buf[buflen];
    const auto length = read(fd, buf, buflen);
    if (length < 0) {
        perror("read");
        return false;
    }

    std::remove_const<decltype(length)>::type i = 0;
    while (i < length) {
        auto event = reinterpret_cast<const struct inotify_event*>(&buf[i]);
        if (event->len)
            goto cleanup;
        i += eventsize + event->len;
    }

cleanup:
    inotify_rm_watch(fd, wd);
    close(fd);

    return true;
}

int main(int argc, char* argv[])
{
    xph::gather_exec_info(argc, argv);

    Options options(xph::exec_name, argc, argv);

    if (std::optional<int> optional_ret; (optional_ret = force_single_instance(options)))
        return *optional_ret;

    create_windows(options);

    if (options.exit_if_none_selected && windows.empty())
        terminate();

    xph::sys::signals<4>({ SIGINT, SIGTERM, SIGQUIT, SIGHUP }, handle_signals);

    if (!options.alpha_path.empty()) {
        if (!std::filesystem::exists(options.alpha_path) &&
            ::mkfifo(options.alpha_path.data(), 0666)) {
            std::perror("mkfifo");
            die("could not create fifo at ", options.alpha_path);
        }

        auto fd = ::open(options.alpha_path.data(), O_RDONLY | O_NONBLOCK);
        if (fd == -1) {
            std::perror("open");
            die("could not open ", options.alpha_path);
        }

        if (struct stat st; !::fstat(fd, &st) && !S_ISFIFO(st.st_mode))
            die(options.alpha_path, " exists and is not a fifo");

        fd_set read_fds;
        FD_ZERO(&read_fds);
        FD_SET(fd, &read_fds);

        struct timeval timeout = {
            .tv_sec = 0,
            .tv_usec = 0,
        };

        while (watch_alpha(options.alpha_path.data())) {
            static auto alpha = options.alpha;
            static std::stringstream ss;
            static double new_alpha;
            static auto read_alpha = [&](void) {
                for (char buf; ::read(fd, &buf, sizeof(char)) == sizeof(char) && buf != '\n';
                     ss << buf) {}
                ss >> new_alpha;
                ss.str("");
                ss.clear();
                new_alpha = std::clamp(new_alpha, 0.0, 1.0);
            };

            read_alpha();

            const constexpr double epsilon = 0.0001;
            while (!xph::approx_eq(alpha, new_alpha, epsilon)) {
                alpha += (new_alpha - alpha) * options.lerp_factor;
                alpha = std::clamp(alpha, 0.0, 1.0);
                std::for_each(windows.begin(), windows.end(), [&](auto window) {
                    set_window_alpha(window, alpha);
                    XFlush(display);
                });

                std::this_thread::sleep_for(options.frame_time);

                if (auto select = ::select(fd + 1, &read_fds, nullptr, nullptr, &timeout);
                    select == -1) {
                    std::perror("select");
                    die("could not check for data in ", options.alpha_path);
                } else if (select) {
                    read_alpha();
                }
            }

            std::for_each(windows.begin(), windows.end(), [&](auto window) {
                set_window_alpha(window, new_alpha);
                XFlush(display);
            });
            alpha = new_alpha;
        }
        die("could not watch ", options.alpha_path);
    }

    std::promise<void>().get_future().wait();
    terminate();
}
