#include <base64.h>
#include <regex>
#include <zlib.h>
#include "utils/binary.hpp"
#include "utils/string.hpp"
#include "utils/time.hpp"
#include "krc.hpp"

static soundsphere::Bin _krc_xor(const std::string &data)
{
    soundsphere::Bin ret;
    if (data[0] != 0x6b || data[1] != 0x72 || data[2] != 0x63 || data[3] != 0x31)
    {
        return ret;
    }

    static const uint8_t enckey[] = {
        0x40, 0x47, 0x61, 0x77, 0x5e, 0x32, 0x74, 0x47, 0x51, 0x36, 0x31, 0x2d, 0xce, 0xd2, 0x6e, 0x69,
    };
    for (size_t i = 4; i < data.size(); i++)
    {
        uint8_t x = data[i];
        uint8_t y = enckey[(i - 4) % sizeof(enckey)];
        uint8_t v = x ^ y;
        ret.push_back(v);
    }

    return ret;
}

static std::string _zlib_uncompress(const soundsphere::Bin &data)
{
    std::string unzip_data;

    z_stream stream;
    memset(&stream, 0, sizeof(stream));
    stream.next_in = (Bytef *)data.data();
    stream.avail_in = (uInt)data.size();

    if (inflateInit(&stream) != Z_OK)
    {
        return unzip_data;
    }

    int result;
    char buffer[4096];

    do
    {
        stream.next_out = (Bytef *)buffer;
        stream.avail_out = sizeof(buffer);

        result = inflate(&stream, Z_NO_FLUSH);
        if (result == Z_STREAM_END || result == Z_DATA_ERROR || result == Z_MEM_ERROR)
        {
            break;
        }

        unzip_data.append(buffer, sizeof(buffer) - stream.avail_out);
    } while (result != Z_STREAM_END);

    inflateEnd(&stream);
    return unzip_data;
}

static std::string _format_time(int time)
{
    double duration = (time) / 1000;
    return soundsphere::time_seconds_to_string(duration);
}

static std::string _krc_to_lyric(const std::string &data)
{
    std::regex metaRegex("^\\[(\\S+):(\\S+)\\]$");
    std::regex timestampsRegex("^\\[(\\d+),(\\d+)\\]");
    std::regex timestamps2Regex("<(\\d+),(\\d+),(\\d+)>([^<]*)");

    soundsphere::StringVec lines = soundsphere::string_split(data, "\n");
    lines = soundsphere::string_trim_vec(lines);

    std::string lyric;
    std::smatch matches;
    for (auto it = lines.begin(); it != lines.end(); it++)
    {
        std::string line = *it;

        if (std::regex_search(line, matches, metaRegex))
        {
            std::string str = matches[0].str();
            lyric += str + "\n";
            continue;
        }

        if (std::regex_search(line, matches, timestampsRegex))
        {
            std::string lyric_line;

            std::string str = matches[1].str();
            int startTime = std::stoi(str);
            str = matches[2].str();
            int duration = std::stoi(str);
            lyric_line += "[" + _format_time(startTime) + "]";

            auto words_begin = std::sregex_iterator(line.begin(), line.end(), timestamps2Regex);
            for (; words_begin != std::sregex_iterator(); words_begin++)
            {
                std::smatch match = *words_begin;
                int offset = std::stoi(match[1].str());
                std::string subword = match[4].str();
                lyric_line += "<" + _format_time(startTime + offset) + ">" + subword;
            }

            lyric_line += "<" + _format_time(startTime + duration) + ">";
            lyric += lyric_line + "\n";
        }
    }

    return lyric;
}

std::string soundsphere::krc_to_lyric(const std::string &data)
{
    std::string base64_decode_data = base64_decode(data);
    soundsphere::Bin zip_data = _krc_xor(base64_decode_data);
    std::string unzip_data = _zlib_uncompress(zip_data);
    return _krc_to_lyric(unzip_data);
}
