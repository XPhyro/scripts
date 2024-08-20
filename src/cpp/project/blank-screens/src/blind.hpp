#ifndef HEADER_SCRIPTS_CXX_BS_BLIND_
#define HEADER_SCRIPTS_CXX_BS_BLIND_

#include <X11/extensions/Xrandr.h>

#include "cli.hpp"

namespace bs {
    class blind {
    public:
        std::string_view m_monitor;

    private:
        std::shared_ptr<cli> m_cli;
        Window m_window;
        Display* m_display;

    public:
        blind(void) = delete;
        ~blind(void);
        blind(std::shared_ptr<cli> cli,
              std::string_view monitor,
              Display* display,
              int default_screen,
              Window root_window,
              XRRCrtcInfo* crtc_info);

        void set_alpha(double alpha);
    };
} // namespace bs

#endif /* ifndef HEADER_SCRIPTS_CXX_BS_BLIND_ */
