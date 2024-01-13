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
#include <sys/inotify.h>
#include <unistd.h>

// C++ libraries
#include <die.hpp>
#include <iteratorutil.hpp>
#include <lexical_cast.hpp>
#include <sysutil.hpp>

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

public:
    std::string_view alpha_path;
    double alpha = k_default_alpha;
    bool exit_if_none_selected = false;
    std::string_view lock_path;
    bool ignore_primary = false;
    std::unordered_set<std::string> selected_monitors;
    std::unordered_set<std::string> ignored_monitors;

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
               "  -A PATH    listen for alpha changes in PATH. listening is done via inotify.\n"
               "  -a ALPHA   set alpha of blinds to ALPHA. default is "
            << k_default_alpha
            << ".\n"
               "  -e         immediately exit if no monitors are blanked\n"
               "  -h         display this help and exit\n"
               "  -l PATH    path to lock file. default is \"${TMPDIR:-/tmp}/"
            << exec_name
            << ".lock\"\n"
               "  -m NAME    don't blank monitor with name NAME, can be given multiple times\n"
               "  -p         don't blank primary monitor\n";
        std::exit(EXIT_SUCCESS);
    }

    void error(void)
    {
        std::cerr << "Try '" << exec_name << " -h' for more information.\n";
        std::exit(EXIT_FAILURE);
    }

    void parse_args(int& argc, char**& argv)
    {
        for (int i; (i = getopt(argc, argv, "A:a:ehl:m:p")) != -1;) {
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

[[noreturn]] void cleanup(int exit_code)
{
    for (auto& window : windows)
        XDestroyWindow(display, window);
    XCloseDisplay(display);
    std::filesystem::remove(lock_file);
    std::exit(exit_code);
}

[[noreturn]] void terminate(void)
{
    cleanup(EXIT_SUCCESS);
}

[[noreturn]] void die(void)
{
    cleanup(EXIT_FAILURE);
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
        return {};
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
        while (watch_alpha(options.alpha_path.data())) {
            std::ifstream ifl(options.alpha_path.data());
            if (!ifl.is_open())
                continue;

            double alpha;
            ifl >> alpha;
            std::for_each(windows.begin(), windows.end(), [&](auto window) {
                set_window_alpha(window, alpha);
                XFlush(display);
            });
        }
        die();
    }

    std::promise<void>().get_future().wait();
    terminate();
}
