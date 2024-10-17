#ifndef SOUND_SPHERE_BACKENDS_INIT_HPP
#define SOUND_SPHERE_BACKENDS_INIT_HPP

#include <functional>
#include <memory>

namespace soundsphere
{

/**
 * @brief UI draw function.
 */
typedef std::function<void(void)> DrawFn;

/**
 * @brief Texture for ImGui.
 */
typedef std::shared_ptr<void> Texture;

/**
 * @brief Initialize backend.
 */
void backend_init(void);

/**
 * @brief Cleanup backend.
 */
void backend_exit(void);

/**
 * @brief Draw UI.
 * @note The place call this function is the UI thread.
 * @param[in] fn    Draw callback.
 */
void backend_draw(DrawFn fn);

/**
 * @brief Load image from memory and return the texture that can be render
 *   directly.
 * @note This function must be called from UI thread.
 * @param[in] data  The image data.
 * @param[in] size  The image size.
 * @return  The texture.
 */
Texture backend_load_image(const void *data, size_t size);

/**
 * @brief Load image from file and return the texture that can be render
 *   directly.
 * @note This function must be called from UI thread.
 * @param[in] path  File path.
 * @return  The texture.
 */
Texture backend_load_image_from_file(const char *path);

} // namespace soundsphere

#endif
