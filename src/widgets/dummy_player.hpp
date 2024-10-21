#ifndef SOUND_SPHERE_WIDGETS_DUMMY_PLAYER_HPP
#define SOUND_SPHERE_WIDGETS_DUMMY_PLAYER_HPP

#include "__init__.hpp"

namespace soundsphere
{

struct DummyPlayerReload
{
    static const uint64_t ID = MAKE_MSGID(WIDGET_ID_DUMMY_PLAYER, __LINE__);

    struct Req : public Msg::Req
    {
    };

    struct Rsp : public Msg::Rsp
    {
    };
};

struct DummyPlayerPause
{
    static const uint64_t ID = MAKE_MSGID(WIDGET_ID_DUMMY_PLAYER, __LINE__);

    struct Req : public Msg::Req
    {
    };

    struct Rsp : public Msg::Rsp
    {
    };
};

struct DummyPlayerNext
{
    static const uint64_t ID = MAKE_MSGID(WIDGET_ID_DUMMY_PLAYER, __LINE__);

    struct Req : public Msg::Req
    {
    };

    struct Rsp : public Msg::Rsp
    {
    };
};

struct DummyPlayerSetVolume
{
    static const uint64_t ID = MAKE_MSGID(WIDGET_ID_DUMMY_PLAYER, __LINE__);

    struct Req : public Msg::Req
    {
        Req(int volume);
        int volume;
    };

    struct Rsp : public Msg::Rsp
    {
    };
};

struct DummyPlayerSetPosition
{
    static const uint64_t ID = MAKE_MSGID(WIDGET_ID_DUMMY_PLAYER, __LINE__);
    struct Req : public Msg::Req
    {
        /**
         * @brief Construct request.
         * @param[in] position  Percentage of position.
         */
        Req(float position);

        /**
         * @brief Percentage of position.
         */
        float position;
    };

    struct Rsp : public Msg::Rsp
    {
    };
};

struct DummyPlayerSetShuffleMode
{
    static const uint64_t ID = MAKE_MSGID(WIDGET_ID_DUMMY_PLAYER, __LINE__);

    enum shuffle_mode
    {
        SHUFFLE_ORDER,
        SHUFFLE_RANDOM,
        SHUFFLE_REPEAT,
    };

    struct Req : public Msg::Req
    {
        Req(shuffle_mode mode);
        shuffle_mode mode;
    };

    struct Rsp : public Msg::Rsp
    {
    };

    struct Evt : public Msg::Evt
    {
        Evt(shuffle_mode mode);
        shuffle_mode mode;
    };
};

/**
 * @brief Play or resume audio.
 * If audio is paused, resume the audio.
 * If audio is playing, re-play the audio.
 * If audio is not playing, play the selected item.
 */
struct DummyPlayerResumeOrPlay
{
    static const uint64_t ID = MAKE_MSGID(WIDGET_ID_DUMMY_PLAYER, __LINE__);

    struct Req : public Msg::Req
    {
    };

    struct Rsp : public Msg::Rsp
    {
    };
};

} // namespace soundsphere

#endif
