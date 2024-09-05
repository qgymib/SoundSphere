#include <stdio.h>
#include <filesystem>
#include "explorer.hpp"

struct FilterPattern
{
    std::string             name;
    soundsphere::StringVec  filters;
};

typedef std::vector<FilterPattern> FilterPatternVec;

static bool _popen_r(const std::string& cmd, std::string& out)
{
    char temp[1024];
    std::string buff;
    FILE* f = popen(cmd.c_str(), "r");

    while (!feof(f))
    {
        size_t read_sz = fread(temp, 1, sizeof(temp), f);
        buff.append(temp, read_sz);
    }

    if (pclose(f))
    {
        return false;
    }

    out = buff;
    return true;
}

static bool _explorer_open_files_by_zenity(soundsphere::StringVec& paths,
    const FilterPatternVec& filters)
{
    std::string cmd("zenity --file-selection --multiple");

    for (size_t i = 0; i < filters.size(); i++)
    {
        const FilterPattern& pattern = filters[i];

        std::string filter = " --file-filter=\"" + pattern.name + " |";
        for (size_t j = 0; j < pattern.filters.size(); j++)
        {
            filter.append(" ");
            filter.append(pattern.filters[j]);
        }
        filter.append("\"");

        cmd.append(filter);
    }

    std::string out;
    if (!_popen_r(cmd, out))
    {
        return false;
    }

    paths = soundsphere::string_split(out, "\n");
    return true;
}

static bool _explorer_open_folder_by_zenity(std::string& path)
{
    std::string cmd("zenity --file-selection --directory");

    std::string out;
    if (!_popen_r(cmd, out))
    {
        return false;
    }

    path = soundsphere::string_trim(out);
    return true;
}

static FilterPattern _filter_to_pattern(const char* filter)
{
    FilterPattern pattern;
    pattern.filters = soundsphere::string_split(filter, "\n");
    pattern.name = pattern.filters[0];
    pattern.filters.erase(pattern.filters.begin());

    return pattern;
}

static FilterPatternVec _filters_to_patterns(const char* filters[], size_t filter_sz)
{
    FilterPatternVec pattern_vec;
    for (size_t i = 0; i < filter_sz; i++)
    {
        FilterPattern pattern = _filter_to_pattern(filters[i]);
        pattern_vec.push_back(pattern);
    }

    return pattern_vec;
}

bool soundsphere::explorer_open_files(soundsphere::StringVec& paths,
    const char* filter[], size_t filter_sz)
{
    FilterPatternVec pattern_vec = _filters_to_patterns(filter, filter_sz);
    return _explorer_open_files_by_zenity(paths, pattern_vec);
}

bool soundsphere::explorer_open_folder(std::string& path)
{
    return _explorer_open_folder_by_zenity(path);
}

soundsphere::FileItemVec soundsphere::explorer_folder_items(const std::string& path)
{
    FileItemVec items;

    for (const auto& entry : std::filesystem::directory_iterator(path))
    {
        FileItem item;
        item.path = entry.path().string();
        item.name = entry.path().filename().string();
        item.isfile = entry.is_regular_file();
        items.push_back(item);
    }

    return items;
}

static bool _is_file_name_match(const std::string& name, const FilterPatternVec& patterns)
{
    for (size_t i = 0; i < patterns.size(); i++)
    {
        const FilterPattern& pattern = patterns[i];
        for (size_t j = 0; j < pattern.filters.size(); j++)
        {
            const std::string& filter = pattern.filters[i];
            if (soundsphere::string_wildcard(name, filter))
            {
                return true;
            }
        }
    }
    return false;
}

soundsphere::StringVec soundsphere::explorer_scan_folder(const std::string& path,
    const char* filter[], size_t filter_sz)
{
    StringVec ret_vec;
    FilterPatternVec pattern_vec = _filters_to_patterns(filter, filter_sz);
    FileItemVec item_vec = explorer_folder_items(path);

    for (size_t i = 0; i < item_vec.size(); i++)
    {
        FileItem& item = item_vec[i];
        if (item.isfile)
        {
            if (_is_file_name_match(item.name, pattern_vec))
            {
                ret_vec.push_back(item.path);
            }
        }
        else
        {
            StringVec tmp_ret = explorer_scan_folder(item.path, filter, filter_sz);
            ret_vec.insert(ret_vec.end(), tmp_ret.begin(), tmp_ret.end());
        }
    }

    return ret_vec;
}
