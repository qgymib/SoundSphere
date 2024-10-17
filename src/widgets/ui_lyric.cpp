#include <imgui.h>
#include <map>
#include "config/__init__.hpp"
#include "runtime/__init__.hpp"
#include "utils/string.hpp"
#include "utils/time.hpp"
#include "__init__.hpp"

/**
 * @brief Lyric.
 * The key is the lyric sentence time position in seconds.
 * The value is the lyric sentence;
 */
typedef std::map<double, std::string> Lyric;

typedef struct lyric_ctx
{
    lyric_ctx();

    /**
     * @brief Music ID.
     */
    uint64_t path_hash;

    /**
     * @brief Lyric.
     */
    Lyric lyric;

    /**
     * @brief The lyric scroll position
     */
    float lyric_scroll_y;

    /**
     * @brief Last scroll position.
     */
    float last_scroll_y;

    /**
     * @brief The last user manually scroll time.
     */
    uint64_t last_user_scroll_time;
} lyric_ctx_t;

static lyric_ctx_t *s_lyric = nullptr;

lyric_ctx::lyric_ctx()
{
    path_hash = (uint64_t)-1;
    lyric_scroll_y = 0.0f;
    last_scroll_y = 0.0f;
    last_user_scroll_time = 0;
}

static void _ui_lyric_init(void)
{
    s_lyric = new lyric_ctx_t;
}

static void _ui_lyric_exit(void)
{
    delete s_lyric;
    s_lyric = nullptr;
}

/**
 * + [12:45.78]
 * + [12:45.789]
 */
static double _split_lyric(std::string &dst, const std::string &str)
{
    if (!(str[0] == '[' && isdigit(str[1]) && isdigit(str[2]) && str[3] == ':' && isdigit(str[4]) && isdigit(str[5]) &&
          str[6] == '.' && isdigit(str[7]) && isdigit(str[8])))
    {
        dst = str;
        return 0;
    }

    double minute = std::stod(str.substr(1, 2));
    double seconds = std::stod(str.substr(4, 2));
    double millisecond = 0;

    if (str[9] == ']')
    {
        millisecond = std::stod(str.substr(7, 2));
        dst = str.substr(10);
        return minute * 60 + seconds + millisecond / 1000;
    }

    if (!isdigit(str[9]) || str[10] != ']')
    {
        dst = str;
        return false;
    }

    millisecond = std::stod(str.substr(7, 3));
    dst = str.substr(11);
    return minute * 60 + seconds + millisecond / 1000;
}

static void _strip_lyric(std::string &lyric)
{
    size_t start = 0;

    while (start < lyric.size())
    {
        size_t pos = lyric.find("<", start);
        if (pos == std::string::npos)
        {
            break;
        }

        if (isdigit(lyric[pos + 1]) && isdigit(lyric[pos + 2]) && lyric[pos + 3] == ':' && isdigit(lyric[pos + 4]) &&
            isdigit(lyric[pos + 5]) && lyric[pos + 6] == '.' && isdigit(lyric[pos + 7]) && isdigit(lyric[pos + 8]))
        {
            if (lyric[pos + 9] == '>')
            {
                lyric.erase(pos, 10);
                start = pos;
                continue;
            }

            if (isdigit(lyric[pos + 9]) && lyric[pos + 10] == '>')
            {
                lyric.erase(pos, 11);
                start = pos;
                continue;
            }

            start = pos + 9;
            continue;
        }

        start++;
    }
}

static Lyric _compile_lyric(const std::string &src)
{
    Lyric lyric;
    soundsphere::StringVec raw_lines = soundsphere::string_split(src, "\n");
    soundsphere::StringVec lines = soundsphere::string_trim_vec(raw_lines);

    soundsphere::StringVec::iterator it = lines.begin();
    for (; it != lines.end(); it++)
    {
        std::string sentence;
        double position = _split_lyric(sentence, *it);
        _strip_lyric(sentence);

        lyric.insert(Lyric::value_type(position, sentence));
    }

    return lyric;
}

static void _scroll_here(void)
{
    ImGui::SetScrollHereY();
    s_lyric->lyric_scroll_y = ImGui::GetScrollY();
    s_lyric->last_scroll_y = s_lyric->lyric_scroll_y;
    s_lyric->last_user_scroll_time = 0;
}

/**
 * @brief Scroll the lyric.
 *
 * This is how scroll works:
 * 1. Player draw lyrics for eachline, and call this function when draw the
 *   current playing lyric.
 * 2. For normal condition, the lyric should shown in center of the view.
 * 3. But if user scroll the lyric view, the lyric should come back in center
 *   after certain delay, to let user can actually scroll the lyric.
 * 4. The auto center function should not work when music is not playing.
 */
static void _auto_scroll(void)
{
    float scroll_y = ImGui::GetScrollY();
    if (scroll_y == s_lyric->lyric_scroll_y)
    {
        _scroll_here();
        return;
    }

    if (!soundsphere::_G.playbar.is_playing)
    {
        return;
    }

    uint64_t current_time = soundsphere::clock_time_ms();
    if (s_lyric->last_user_scroll_time == 0 || scroll_y != s_lyric->last_scroll_y)
    {
        s_lyric->last_scroll_y = scroll_y;
        s_lyric->last_user_scroll_time = current_time;
        return;
    }

    uint64_t diff_time = current_time - s_lyric->last_user_scroll_time;
    if (diff_time < soundsphere::_config.lyric.auto_center_time_ms)
    {
        return;
    }

    _scroll_here();
}

static void _show_lyric(const Lyric &lyric, double playing_position)
{
    ImVec4 fore_color(soundsphere::_config.lyric.fore_font_color[0], soundsphere::_config.lyric.fore_font_color[1],
                      soundsphere::_config.lyric.fore_font_color[2], soundsphere::_config.lyric.fore_font_color[3]);
    ImVec4 back_color(soundsphere::_config.lyric.back_font_color[0], soundsphere::_config.lyric.back_font_color[1],
                      soundsphere::_config.lyric.back_font_color[2], soundsphere::_config.lyric.back_font_color[3]);

    Lyric::const_iterator it = lyric.begin();
    for (; it != lyric.end(); it++)
    {
        double music_position = it->first;
        const std::string &sentence = it->second;

        /* Lyric not played. */
        if (music_position > playing_position)
        {
            ImGui::TextColoredCenter(back_color, "%s", sentence.c_str());
            continue;
        }

        /* If it is the last lyric, just highlight it. */
        if (it == std::prev(lyric.end()))
        {
            ImGui::TextColoredCenter(fore_color, "%s", sentence.c_str());
            _auto_scroll();
            continue;
        }

        /* Get next lyric position */
        double next_position = std::next(it)->first;

        /* If current playing position is in range, highlight this lyric. */
        if (music_position < playing_position && playing_position < next_position)
        {
            ImGui::TextColoredCenter(fore_color, "%s", sentence.c_str());
            _auto_scroll();
            continue;
        }

        ImGui::TextColoredCenter(back_color, "%s", sentence.c_str());
    }
}

static void _ui_lyric_draw(void)
{
    /* Set position and size. */
    ImGui::SetNextWindowSize(soundsphere::_layout.lyric.size);
    ImGui::SetNextWindowPos(soundsphere::_layout.lyric.pos);

    const int lyric_flags =
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBringToFrontOnFocus;
    if (ImGui::Begin("Lyric", nullptr, lyric_flags))
    {
        soundsphere::MusicTagPtr obj = soundsphere::_G.dummy_player.current_music;
        if (obj.get() == nullptr)
        {
            goto finish;
        }

        if (s_lyric->path_hash != obj->path_hash)
        {
            s_lyric->path_hash = obj->path_hash;
            s_lyric->lyric = _compile_lyric(obj->info.lyric);
        }

        _show_lyric(s_lyric->lyric, soundsphere::_G.playbar.music_position);
    }

finish:
    ImGui::End();
}

const soundsphere::widget_t soundsphere::ui_lyric = {
    _ui_lyric_init,
    _ui_lyric_exit,
    _ui_lyric_draw,
};
