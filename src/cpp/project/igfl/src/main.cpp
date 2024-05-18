#include <cerrno>
#include <cstdlib>

#include <unistd.h>

#include "lib/imgui.h"
#include "lib/imtui-impl-ncurses.h"
#include "lib/imtui.h"

#include <xph/die.hpp>
#include <xph/exec_info.hpp>
#include <xph/param.hpp>

#include "options.hpp"
#include "types.hpp"
#include "ui.hpp"

DEFINE_EXEC_INFO();

int main(int argc, char* argv[])
{
    xph::gather_exec_info(argc, argv);

    igfl::Options options(argc, argv);

    if (errno = 0; options.get_init_dir() && (chdir(options.get_init_dir()->c_str()) || errno))
        xph::die("could not change directory to ", *options.get_init_dir());

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    const auto disable_mouse_support = options.get_disable_mouse_support();
    const auto active_fps = options.get_active_fps();
    const auto idle_fps = options.get_idle_fps();
    const auto screen = ImTui_ImplNcurses_Init(!disable_mouse_support, active_fps, idle_fps);
    ImTui_ImplText_Init();

    auto& io = ImGui::GetIO();

    io.IniFilename = xph::coalesce_deref_map<const char*>([&](auto x) { return x.c_str(); },
                                                          options.get_ini_file());
    io.LogFilename = xph::coalesce_deref_map<const char*>([&](auto x) { return x.c_str(); },
                                                          options.get_log_file());

    igfl::UI ui;

    for (std::size_t frame_count = 0;; ++frame_count) {
        ImTui_ImplNcurses_NewFrame();
        ImTui_ImplText_NewFrame();
        ImGui::NewFrame();

        switch (ui.new_frame(io, io.DeltaTime, frame_count)) {
            case application_target::exit:
                return EXIT_SUCCESS;
            case application_target::die:
                return EXIT_FAILURE;
            case application_target::none:
            default:
                break;
        }

        ImGui::Render();
        ImTui_ImplText_RenderDrawData(ImGui::GetDrawData(), screen);
        ImTui_ImplNcurses_DrawScreen(true);
    }

    ImTui_ImplText_Shutdown();
    ImTui_ImplNcurses_Shutdown();

    return EXIT_SUCCESS;
}
