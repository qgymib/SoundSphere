#include "string.hpp"
#include "imgui.hpp"

void ImGui::Tip(const char *str, float width)
{
    if (ImGui::BeginItemTooltip())
    {
        ImGui::PushTextWrapPos(::ImGui::GetFontSize() * width);
        ImGui::TextUnformatted(str);
        ImGui::PopTextWrapPos();

        ImGui::EndTooltip();
    }
}

void ImGui::TipMark(const char *str, float width)
{
    ImGui::TextDisabled("(?)");
    ImGui::Tip(str, width);
}

static std::string _imgui_text_center_cursor(const char *fmt, va_list ap)
{
    va_list ap_copy;
    va_copy(ap_copy, ap);
    std::string data = soundsphere::string_format_v(fmt, ap);
    va_end(ap_copy);

    float window_width = ImGui::GetWindowSize().x;
    float text_width = ImGui::CalcTextSize(data.c_str()).x;

    ImGui::SetCursorPosX((window_width - text_width) * 0.5f);
    return data;
}

void ImGui::TextColoredCenter(const ImVec4 &color, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    std::string data = _imgui_text_center_cursor(fmt, ap);
    va_end(ap);

    ImGui::TextColored(color, "%s", data.c_str());
}

void ImGui::TextCenter(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    std::string data = _imgui_text_center_cursor(fmt, ap);
    va_end(ap);

    ImGui::Text("%s", data.c_str());
}
