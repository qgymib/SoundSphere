#include <algorithm>
#include <cctype>
#include <locale>
#include <codecvt>

#include "string.hpp"

/**
 * @brief Checks if the given character is whitespace.
 * We cannot use std::isspace because on Win32 \c must between [0, 255], however
 * we use UTF-8 encoding so it may out of range.
 */
static int _isspace(int c)
{
    return c == ' ' || c == '\f' || c == '\n' || c == '\r' || c == '\t' || c == '\v';
}

#if defined(_WIN32)

#include <Windows.h>
#include <cassert>

std::string soundsphere::wide_to_utf8(WCHAR *src)
{
    std::string ret;

    int target_len = WideCharToMultiByte(CP_UTF8, 0, src, -1, NULL, 0, NULL, NULL);
    if (target_len == 0)
    {
        return ret;
    }

    char *buf = (char *)malloc(target_len);
    if (buf == NULL)
    {
        return ret;
    }

    int r = WideCharToMultiByte(CP_UTF8, 0, src, -1, buf, target_len, NULL, NULL);
    assert(r == target_len);
    (void)r;

    ret = buf;
    free(buf);

    return ret;
}

soundsphere::wstring soundsphere::utf8_to_wide(const char *src)
{
    int pathw_len = MultiByteToWideChar(CP_UTF8, 0, src, -1, NULL, 0);
    if (pathw_len == 0)
    {
        return NULL;
    }

    size_t buf_sz = pathw_len * sizeof(WCHAR);
    WCHAR *buf = (WCHAR *)malloc(buf_sz);
    if (buf == NULL)
    {
        return NULL;
    }

    int r = MultiByteToWideChar(CP_UTF8, 0, src, -1, buf, pathw_len);
    assert(r == pathw_len);
    (void)r;

    return soundsphere::wstring(buf, [](WCHAR *s) { free(s); });
}

#endif

soundsphere::StringVec soundsphere::string_split(const std::string &orig, const std::string &delimiter)
{
    soundsphere::StringVec vetStr;

    /* If #orig is empty or equal to #delimiter, return empty vector */
    if (orig == "" || orig == delimiter)
    {
        return vetStr;
    }
    /* If #delimiter is empty, return original vector */
    if (delimiter == "")
    {
        vetStr.push_back(orig);
        return vetStr;
    }

    std::string::size_type startPos = 0;
    size_t index = orig.find(delimiter);
    while (index != std::string::npos)
    {
        std::string str = orig.substr(startPos, index - startPos);
        if (str != "")
        {
            vetStr.push_back(str);
        }
        startPos = index + delimiter.length();
        index = orig.find(delimiter, startPos);
    }

    /* Use last substring */
    std::string str = orig.substr(startPos);
    if (str != "")
    {
        vetStr.push_back(str);
    }

    return vetStr;
}

bool soundsphere::string_wildcard(const std::string &str, const std::string &pattern)
{
    const size_t sLen = str.length();
    const size_t pLen = pattern.length();
    size_t sIdx = 0, pIdx = 0;
    int starIdx = -1, matchIdx = 0;

    while (sIdx < sLen)
    {
        if (pIdx < pLen && (pattern[pIdx] == '?' || pattern[pIdx] == str[sIdx]))
        {
            // Match single character or '?'
            sIdx++;
            pIdx++;
        }
        else if (pIdx < pLen && pattern[pIdx] == '*')
        {
            // Match '*' and remember its position
            starIdx = (int)pIdx;
            matchIdx = (int)sIdx;
            pIdx++;
        }
        else if (starIdx != -1)
        {
            // Backtrack: if there's a previous '*', try to match more characters
            pIdx = starIdx + 1;
            matchIdx++;
            sIdx = matchIdx;
        }
        else
        {
            // No match
            return false;
        }
    }

    // Check for remaining '*' in pattern
    while (pIdx < pLen && pattern[pIdx] == '*')
    {
        pIdx++;
    }

    return pIdx == pLen;
}

std::string soundsphere::string_trim(const std::string &str)
{
    std::string::const_iterator start = str.begin();
    while (start != str.end() && _isspace(*start))
    {
        start++;
    }
    if (start == str.end())
    {
        return std::string();
    }

    std::string::const_iterator end = str.end();
    do
    {
        end--;
    } while (end != start && _isspace(*end));

    if (end == str.end())
    {
        return std::string(start, end);
    }
    return std::string(start, end + 1);
}

soundsphere::StringVec soundsphere::string_trim_vec(const soundsphere::StringVec &vec)
{
    soundsphere::StringVec ret;

    soundsphere::StringVec::const_iterator it = vec.begin();
    for (; it != vec.end(); it++)
    {
        const std::string &src = *it;
        std::string dst = soundsphere::string_trim(src);
        ret.push_back(dst);
    }

    return ret;
}

std::string soundsphere::string_format(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    std::string ret = soundsphere::string_format_v(fmt, args);
    va_end(args);

    return ret;
}

std::string soundsphere::string_format_v(const char *fmt, va_list ap)
{
    va_list args_copy;
    va_copy(args_copy, ap);
    int size = std::vsnprintf(nullptr, 0, fmt, args_copy) + 1; // +1 for null terminator
    va_end(args_copy);

    // Create a vector with the required size
    std::vector<char> buf(size);
    va_copy(args_copy, ap);
    std::vsnprintf(buf.data(), size, fmt, args_copy);
    va_end(args_copy);

    // Return as a std::string
    return std::string(buf.data(), buf.size() - 1); // Exclude the null terminator
}

uint64_t soundsphere::string_hash_djb2(const std::string &str)
{
    const uint8_t *data = (uint8_t *)str.data();
    const size_t size = str.size();

    uint64_t hash = 5381;
    for (size_t i = 0; i < size; i++)
    {
        hash = ((hash << 5) + hash) + data[i];
    }

    return hash;
}

std::string soundsphere::string_replace(const std::string &str, const std::string &match, const std::string &replace)
{
    std::string copy = str;
    size_t pos = 0;

    while ((pos = copy.find(match, pos)) != std::string::npos)
    {
        copy.replace(pos, match.length(), replace);
        pos += replace.length();
    }

    return copy;
}

bool soundsphere::string_last_match(const std::string &str, const std::string &pat)
{
    if (pat.size() > str.size())
    {
        return false;
    }

    size_t off = str.size() - pat.size();
    return str.compare(off, std::string::npos, pat) == 0;
}
