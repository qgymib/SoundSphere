#include <algorithm>
#include <cctype>
#include <locale>
#include <codecvt>
#include "string.hpp"

soundsphere::StringVec soundsphere::string_split(const std::string& orig, const std::string& delimiter)
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

bool soundsphere::string_wildcard(const std::string& str, const std::string& pattern)
{
    const size_t sLen = str.length();
    const size_t pLen = pattern.length();
    size_t sIdx = 0, pIdx = 0;
    int starIdx = -1, matchIdx = 0;

    while (sIdx < sLen) {
        if (pIdx < pLen && (pattern[pIdx] == '?' || pattern[pIdx] == str[sIdx])) {
            // Match single character or '?'
            sIdx++;
            pIdx++;
        } else if (pIdx < pLen && pattern[pIdx] == '*') {
            // Match '*' and remember its position
            starIdx = (int)pIdx;
            matchIdx = (int)sIdx;
            pIdx++;
        } else if (starIdx != -1) {
            // Backtrack: if there's a previous '*', try to match more characters
            pIdx = starIdx + 1;
            matchIdx++;
            sIdx = matchIdx;
        } else {
            // No match
            return false;
        }
    }

    // Check for remaining '*' in pattern
    while (pIdx < pLen && pattern[pIdx] == '*') {
        pIdx++;
    }

    return pIdx == pLen;
}

std::string soundsphere::string_trim(const std::string& str)
{
    std::string::const_iterator start = str.begin();
    while (start != str.end() && std::isspace(*start))
    {
        start++;
    }

    std::string::const_iterator end = str.end();
    do {
        end--;
    } while (end != start && std::isspace(*end));

    return std::string(start, end + 1);
}
