#include "imgui.hpp"

void ImGui::Tip(const char* str, float width)
{
    if (ImGui::BeginItemTooltip())
    {
        ImGui::PushTextWrapPos(::ImGui::GetFontSize() * width);
        ImGui::TextUnformatted(str);
        ImGui::PopTextWrapPos();

        ImGui::EndTooltip();
    }
}

void ImGui::TipMark(const char* str, float width)
{
    ImGui::TextDisabled("(?)");
    ImGui::Tip(str, width);
}
