#include "ui.hpp"

#include <cstddef>

#include <imgui/imgui.h>

#include "types.hpp"

application_target igfl::UI::new_frame(ImGuiIO& io, float delta_time, std::size_t frame_count)
{
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
                static_cast<double>(delta_time),
                static_cast<double>(1.0f / delta_time));
    ImGui::TextUnformatted(io.IniFilename);
    ImGui::TextUnformatted(io.LogFilename);

    static float slider_float = 1.23f;
    ImGui::Text("Float:");
    ImGui::SameLine();
    ImGui::SliderFloat("##slider_float", &slider_float, 0.0f, 10.0f);

    static bool space_pressed = false;
    space_pressed |= ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Space));
    ImGui::Text("Is Space Pressed:");
    ImGui::SameLine();
    ImGui::Checkbox("##space_pressed", &space_pressed);

    if (ImGui::IsKeyPressed('q') ||
        ImGui::Button("Exit program", { ImGui::GetContentRegionAvail().x, 2 }))
        return application_target::exit;

    return application_target::none;
}
