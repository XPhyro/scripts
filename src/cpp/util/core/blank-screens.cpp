// @LDFLAGS -lX11 -lXrandr

// C++
#include <cstdlib>
#include <exec_info.hpp>
#include <future>
#include <iostream>
#include <iterator>
#include <ranges>
#include <string_view>
#include <unordered_set>
#include <vector>

// C
#include <unistd.h>

// C++ libraries
#include <die.hpp>
#include <iteratorutil.hpp>
#include <sysutil.hpp>

// third-party
#include <X11/Xlib.h>
#include <X11/extensions/Xrandr.h>

static Display* display;
static std::vector<Window> windows;

DEFINE_EXEC_INFO();

struct Options {
public:
    bool exit_if_none_selected = false;
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
        std::cout << "Usage: " << exec_name << " [OPTION...] [NAME...]\n"
                  << "Blank monitors.\n"
                     "\n"
                     "If no NAME is provided, all monitors are blanked.\n"
                     "\n"
                     "  -e        immediately exit if no monitors are blanked\n"
                     "  -h        display this help and exit\n"
                     "  -m NAME   don't blank monitor with name NAME, can be given multiple times\n"
                     "  -p        don't blank primary monitor\n";
        std::exit(EXIT_SUCCESS);
    }

    void error(void)
    {
        std::cerr << "Try '" << exec_name << " -h' for more information.\n";
        std::exit(EXIT_FAILURE);
    }

    void parse_args(int& argc, char**& argv)
    {
        for (int i; (i = getopt(argc, argv, "ehm:p")) != -1;) {
            switch (i) {
                case 'e':
                    exit_if_none_selected = true;
                    break;
                case 'h':
                    help();
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

void cleanup(void)
{
    for (auto& window : windows)
        XDestroyWindow(display, window);
    XCloseDisplay(display);
}

void handle_signals([[maybe_unused]] int sig)
{
    cleanup();
    std::exit(EXIT_SUCCESS);
}

int main(int argc, char* argv[])
{
    xph::gather_exec_info(argc, argv);

    Options options(xph::exec_name, argc, argv);

    xph::die_if(!(display = XOpenDisplay(NULL)), "unable to open display");
    int default_screen = DefaultScreen(display);

    Window root_window = RootWindow(display, default_screen);

    XRRScreenResources* screen_resources = XRRGetScreenResources(display, root_window);
    xph::die_if(!screen_resources, "unable to get screen resources");

    RROutput primary_output = XRRGetOutputPrimary(display, root_window);

    for (decltype(screen_resources->noutput) i = 0; i < screen_resources->noutput; ++i) {
        if (options.ignore_primary && screen_resources->outputs[i] == primary_output)
            continue;

        XRROutputInfo* output_info =
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

    if (options.exit_if_none_selected && windows.empty()) {
        cleanup();
        return EXIT_SUCCESS;
    }

    for (auto& window : windows) {
        XSetWindowAttributes attr;
        attr.override_redirect = True;
        XChangeWindowAttributes(display, window, CWOverrideRedirect, &attr);

        XMapWindow(display, window);
        XFlush(display);

        XSetWindowBackground(display, window, BlackPixel(display, default_screen));
        XClearWindow(display, window);
        XFlush(display);
    }

    xph::sys::signals<4>({ SIGINT, SIGTERM, SIGQUIT, SIGHUP }, handle_signals);
    std::promise<void>().get_future().wait();

    return EXIT_SUCCESS;
}
