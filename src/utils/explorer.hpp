#ifndef SOUND_SPHERE_UTILS_EXPLORER_HPP
#define SOUND_SPHERE_UTILS_EXPLORER_HPP

#include "string.hpp"

namespace soundsphere
{

struct FileItem
{
    bool isfile;
    std::string path;
    std::string name;
};

typedef std::vector<FileItem> FileItemVec;

/**
 * @brief Open files.
 * @note This is a blocking interface.
 * @param[out] paths    The selected files.
 * @param[in] filter    List of filter. The filter must have syntax like:
 *   `NAME\nPATTERN1\nPATTERN12\n`
 * @param[in] filter_sz The number of filter.
 * @return              true if select success, false if failure.
 */
bool explorer_open_files(StringVec &paths, const char *filter[], size_t filter_sz);

/**
 * @brief Open folder.
 * @param[out] path Folder path.
 * @return          true if select success, false if failure.
 */
bool explorer_open_folder(std::string &path);

/**
 * @brief Scan give path and return matching files.
 * @param[in] path      The path to scan.
 * @param[in] filter    List of filter.
 * @param[in] filter_sz The number of filters.
 * @return              The matching file list.
 */
StringVec explorer_scan_folder(const std::string &path, const char *filter[], size_t filter_sz);

/**
 * @brief Get items in folder (non-recursive).
 * @param[in] path  Folder path.
 * @return Items in this folder.
 */
FileItemVec explorer_folder_items(const std::string &path);

} // namespace soundsphere

#endif
