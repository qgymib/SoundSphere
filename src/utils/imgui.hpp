#ifndef SOUND_SPHERE_UTILS_IMGUI_HPP
#define SOUND_SPHERE_UTILS_IMGUI_HPP

#include <imgui.h>
#include <string>

namespace ImGui {

/**
 * @brief Append to last item.
 * @param[in] str   Tip string.
 * @param[in] width Word wrap count.
 */
void Tip(const char* str, float width = 35.0f);

/**
 * @brief Add a tip mark.
 * @note You may want to use ImGui::SameLine() to let the mark stay right after the item.
 * @param[in] str   Tip string.
 * @param[in] width Word wrap count.
 */
void TipMark(const char* str, float width = 35.0f);

}

#endif
