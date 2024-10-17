#include <ev.h>
#include <stdio.h>
#include "utils/autoptr.hpp"
#include "explorer.hpp"

#if defined(_WIN32)
#define NATIVE_PATH_SPLIT "\\"
#else
#define NATIVE_PATH_SPLIT "/"
#endif

struct FilterPattern
{
    std::string name;
    soundsphere::StringVec filters;
};

typedef std::vector<FilterPattern> FilterPatternVec;

/**
 * @brief Convert filter to structured pattern.
 * @param[in] filter    Pattern string.
 * @return Structured pattern.
 */
static FilterPattern _filter_to_pattern(const char *filter)
{
    FilterPattern pattern;
    pattern.filters = soundsphere::string_split(filter, "\n");
    pattern.name = pattern.filters[0];
    pattern.filters.erase(pattern.filters.begin());

    return pattern;
}

/**
 * @brief Convert list of filters to list of structured pattern.
 * @param[in] filters   List of filters.
 * @param[in] filter_sz The number of filters.
 * @return  List of structured pattern.
 */
static FilterPatternVec _filters_to_patterns(const char *filters[], size_t filter_sz)
{
    FilterPatternVec pattern_vec;
    for (size_t i = 0; i < filter_sz; i++)
    {
        FilterPattern pattern = _filter_to_pattern(filters[i]);
        pattern_vec.push_back(pattern);
    }

    return pattern_vec;
}

#if defined(_WIN32)

#include <windows.h>
#include <shobjidl.h>
#include <assert.h>

class comdlg_filterspec
{
public:
    comdlg_filterspec()
    {
        m_save_types = NULL;
        m_save_type_sz = 0;
    }

    virtual ~comdlg_filterspec()
    {
        for (size_t i = 0; i < m_save_type_sz; i++)
        {
            free((void *)m_save_types[i].pszName);
            free((void *)m_save_types[i].pszSpec);
        }
        free(m_save_types);
    }

public:
    void append(const std::string &name, const soundsphere::StringVec &filters)
    {
        std::string filter;
        for (size_t i = 0; i < filters.size(); i++)
        {
            const std::string &item = filters[i];
            if (i != 0)
            {
                filter.append(";");
            }
            filter.append(item);
        }

        size_t new_sz = sizeof(COMDLG_FILTERSPEC) * (m_save_type_sz + 1);
        m_save_types = (COMDLG_FILTERSPEC *)realloc(m_save_types, new_sz);

        m_save_types[m_save_type_sz].pszName = _wcsdup(soundsphere::utf8_to_wide(name.c_str()).get());
        m_save_types[m_save_type_sz].pszSpec = _wcsdup(soundsphere::utf8_to_wide(filter.c_str()).get());
        m_save_type_sz++;
    }

public:
    COMDLG_FILTERSPEC *m_save_types;
    size_t m_save_type_sz;
};

static std::shared_ptr<comdlg_filterspec> _pattern_vec_to_filterspec(const FilterPatternVec &pattern_vec)
{
    std::shared_ptr<comdlg_filterspec> spec = std::make_shared<comdlg_filterspec>();

    for (size_t i = 0; i < pattern_vec.size(); i++)
    {
        const FilterPattern &pattern = pattern_vec[i];
        spec->append(pattern.name, pattern.filters);
    }

    return spec;
}

bool soundsphere::explorer_open_files(soundsphere::StringVec &paths, const char *filter[], size_t filter_sz)
{
    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    if (!SUCCEEDED(hr))
    {
        return false;
    }
    soundsphere::defer _1([]() { CoUninitialize(); });

    soundsphere::auto_ptr<IFileOpenDialog> pfd([](IFileDialog *p) { p->Release(); });
    hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pfd));
    if (!SUCCEEDED(hr))
    {
        return false;
    }

    DWORD dwFlags;
    hr = pfd->GetOptions(&dwFlags);
    if (!SUCCEEDED(hr))
    {
        return false;
    }

    hr = pfd->SetOptions(dwFlags | FOS_FORCEFILESYSTEM | FOS_ALLOWMULTISELECT);
    if (!SUCCEEDED(hr))
    {
        return false;
    }

    // Set the file types to display only. Notice that, this is a 1-based array.
    FilterPatternVec pattern_vec = _filters_to_patterns(filter, filter_sz);
    std::shared_ptr<comdlg_filterspec> spec = _pattern_vec_to_filterspec(pattern_vec);
    hr = pfd->SetFileTypes((UINT)spec->m_save_type_sz, spec->m_save_types);
    if (!SUCCEEDED(hr))
    {
        return false;
    }

    hr = pfd->SetFileTypeIndex(1);
    if (!SUCCEEDED(hr))
    {
        return false;
    }

    // Show the dialog
    hr = pfd->Show(NULL);
    if (!SUCCEEDED(hr))
    {
        return false;
    }

    soundsphere::auto_ptr<IShellItemArray> psiaResults([](IShellItemArray *p) { p->Release(); });
    hr = pfd->GetResults(&psiaResults);
    if (!SUCCEEDED(hr))
    {
        return false;
    }

    DWORD dwNumItems = 0;
    hr = psiaResults->GetCount(&dwNumItems);
    if (!SUCCEEDED(hr))
    {
        return false;
    }

    paths.clear();
    for (DWORD i = 0; i < dwNumItems; i++)
    {
        soundsphere::auto_ptr<IShellItem> psiResult([](IShellItem *p) { p->Release(); });
        hr = psiaResults->GetItemAt(i, &psiResult);
        if (!SUCCEEDED(hr))
        {
            return false;
        }

        PWSTR pszFilePath = NULL;
        hr = psiResult->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
        if (!SUCCEEDED(hr))
        {
            return false;
        }

        std::string path = soundsphere::wide_to_utf8(pszFilePath);
        CoTaskMemFree(pszFilePath);

        paths.push_back(path);
    }

    return true;
}

bool soundsphere::explorer_open_folder(std::string &path)
{
    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    if (!SUCCEEDED(hr))
    {
        return false;
    }
    soundsphere::defer _1([]() { CoUninitialize(); });

    soundsphere::auto_ptr<IFileDialog> pFileDialog([](IFileDialog *p) { p->Release(); });
    hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pFileDialog));
    if (!SUCCEEDED(hr))
    {
        return false;
    }

    DWORD dwOptions;
    pFileDialog->GetOptions(&dwOptions);
    pFileDialog->SetOptions(dwOptions | FOS_PICKFOLDERS);

    // Show the Open dialog box
    hr = pFileDialog->Show(NULL);
    if (!SUCCEEDED(hr))
    {
        return false;
    }

    soundsphere::auto_ptr<IShellItem> pItem([](IShellItem *p) { p->Release(); });
    hr = pFileDialog->GetResult(&pItem);
    if (!SUCCEEDED(hr))
    {
        return false;
    }

    PWSTR pszFilePath;
    hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

    // Display the folder path
    if (SUCCEEDED(hr))
    {
        path = soundsphere::wide_to_utf8(pszFilePath);
        CoTaskMemFree(pszFilePath);
    }

    return true;
}

#else

static bool _popen_r(const std::string &cmd, std::string &out)
{
    char temp[1024];
    std::string buff;
    FILE *f = popen(cmd.c_str(), "r");

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

static bool _explorer_open_files_by_zenity(soundsphere::StringVec &paths, const FilterPatternVec &filters)
{
    std::string cmd("zenity --file-selection --multiple");

    for (size_t i = 0; i < filters.size(); i++)
    {
        const FilterPattern &pattern = filters[i];

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

static bool _explorer_open_folder_by_zenity(std::string &path)
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

bool soundsphere::explorer_open_files(soundsphere::StringVec &paths, const char *filter[], size_t filter_sz)
{
    FilterPatternVec pattern_vec = _filters_to_patterns(filter, filter_sz);
    return _explorer_open_files_by_zenity(paths, pattern_vec);
}

bool soundsphere::explorer_open_folder(std::string &path)
{
    return _explorer_open_folder_by_zenity(path);
}

#endif

static bool _is_file_name_match(const std::string &name, const FilterPatternVec &patterns)
{
    for (size_t i = 0; i < patterns.size(); i++)
    {
        const FilterPattern &pattern = patterns[i];
        for (size_t j = 0; j < pattern.filters.size(); j++)
        {
            const std::string &filter = pattern.filters[j];
            if (soundsphere::string_wildcard(name, filter))
            {
                return true;
            }
        }
    }
    return false;
}

soundsphere::FileItemVec soundsphere::explorer_folder_items(const std::string &path)
{
    FileItemVec items;

    ev_fs_req_t req;
    ev_fs_readdir(NULL, &req, path.c_str(), NULL);

    ev_dirent_t *d = ev_fs_get_first_dirent(&req);
    for (; d != NULL; d = ev_fs_get_next_dirent(d))
    {
        FileItem item;
        item.isfile = d->type == EV_DIRENT_FILE;
        item.name = d->name;
        item.path = path + NATIVE_PATH_SPLIT + item.name;
        items.push_back(item);
    }
    ev_fs_req_cleanup(&req);

    return items;
}

soundsphere::StringVec soundsphere::explorer_scan_folder(const std::string &path, const char *filter[],
                                                         size_t filter_sz)
{
    StringVec ret_vec;
    FilterPatternVec pattern_vec = _filters_to_patterns(filter, filter_sz);
    FileItemVec item_vec = explorer_folder_items(path);

    for (size_t i = 0; i < item_vec.size(); i++)
    {
        FileItem &item = item_vec[i];
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
