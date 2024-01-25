#include <cstdlib>

#include <imgui/imgui.h>
#include <imtui/imtui-impl-ncurses.h>
#include <imtui/imtui.h>

int main()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    const auto mouse_support = true;
    const auto active_fps = 144.0f;
    const auto idle_fps = 24.0f;
    const auto screen = ImTui_ImplNcurses_Init(mouse_support, active_fps, idle_fps);
    ImTui_ImplText_Init();

    auto& io = ImGui::GetIO();
    io.IniFilename = nullptr;
    io.LogFilename = nullptr;

    for (std::size_t frame_count = 0;; ++frame_count) {
        ImTui_ImplNcurses_NewFrame();
        ImTui_ImplText_NewFrame();
        ImGui::NewFrame();

        ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x, io.DisplaySize.y));
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::Begin("Root",
                     nullptr,
                     ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar |
                         ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus |
                         ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_MenuBar);

        ImGui::Text("Frame Count: %zu", frame_count);
        ImGui::Text("Mouse Position: (%f, %f)",
                    static_cast<double>(io.MousePos.x),
                    static_cast<double>(io.MousePos.y));
        ImGui::Text("%.3fms (%.1f FPS)",
                    static_cast<double>(1000.0f / io.Framerate),
                    static_cast<double>(io.Framerate));

        static float slider_float = 1.23f;
        ImGui::Text("Float:");
        ImGui::SameLine();
        ImGui::SliderFloat("##slider_float", &slider_float, 0.0f, 10.0f);

        static bool space_pressed = false;
        space_pressed |= ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Space));
        ImGui::Text("Is Space Pressed:");
        ImGui::SameLine();
        ImGui::Checkbox("##space_pressed", &space_pressed);

        if (ImGui::Button("Exit program", { ImGui::GetContentRegionAvail().x, 2 }))
            break;

        ImGui::Render();
        ImTui_ImplText_RenderDrawData(ImGui::GetDrawData(), screen);
        ImTui_ImplNcurses_DrawScreen(true);
    }

    ImTui_ImplText_Shutdown();
    ImTui_ImplNcurses_Shutdown();

    return EXIT_SUCCESS;
}
