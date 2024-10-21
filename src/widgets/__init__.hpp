#ifndef SOUND_SPHERE_WIDGETS_INIT_HPP
#define SOUND_SPHERE_WIDGETS_INIT_HPP

#include <memory>
#include <functional>
#include <map>
#include "utils/imgui.hpp"

/* clang-format off */
#define SOUNDSPHERE_WIDGET_TABLE(xx)                            \
    xx(WIDGET_ID_MENUBAR_OPEN,          menubar_open)           \
    xx(WIDGET_ID_MENUBAR_PREFERENCES,   menubar_preferences)    \
    xx(WIDGET_ID_MENUBAR_TRANSLATIONS,  menubar_translations)   \
    xx(WIDGET_ID_MENUBAR_DEBUG,         menubar_debug)          \
    xx(WIDGET_ID_MENUBAR_ABOUT,         menubar_about)          \
    xx(WIDGET_ID_UI_COVER,              ui_cover)               \
    xx(WIDGET_ID_UI_FILTER,             ui_filter)              \
    xx(WIDGET_ID_UI_LYRIC,              ui_lyric)               \
    xx(WIDGET_ID_UI_PLAYBAR,            ui_playbar)             \
    xx(WIDGET_ID_UI_PLAYLIST,           ui_playlist)            \
    xx(WIDGET_ID_UI_STATUSBAR,          ui_statusbar)           \
    xx(WIDGET_ID_UI_TITLE,              ui_title)               \
    xx(WIDGET_ID_TOOL_TAG_EDITOR,       tool_tag_editor)        \
    xx(WIDGET_ID_DUMMY_PLAYER,          dummy_player)
/* clang-format on */

/**
 * @brief Make message ID.
 * @param[in] widget_id See #widget_id_t.
 * @param[in] msg_id    Message ID in this widget.
 * @return The unique message ID.
 */
#define MAKE_MSGID(widget_id, msg_id) ((uint64_t)((((uint64_t)(widget_id)) << 32) | ((uint32_t)(msg_id))))

namespace soundsphere
{

typedef enum widget_id
{
    /**
     * @brief Menubar item ID.
     * @{
     */
    WIDGET_ID_MENUBAR_OPEN,
    WIDGET_ID_MENUBAR_PREFERENCES,
    WIDGET_ID_MENUBAR_TRANSLATIONS,
    WIDGET_ID_MENUBAR_DEBUG,
    WIDGET_ID_MENUBAR_ABOUT,
    /**
     * @}
     */

    /**
     * @brief UI components.
     * @{
     */
    WIDGET_ID_UI_COVER,
    WIDGET_ID_UI_FILTER,
    WIDGET_ID_UI_LYRIC,
    WIDGET_ID_UI_PLAYBAR,
    WIDGET_ID_UI_PLAYLIST,
    WIDGET_ID_UI_STATUSBAR,
    WIDGET_ID_UI_TITLE,
    /**
     * @}
     */

    WIDGET_ID_TOOL_TAG_EDITOR,

    /**
     * @brief Dummy components that never shows on UI.
     * @{
     */
    WIDGET_ID_DUMMY_PLAYER,
    /**
     * @}
     */

    /**
     * @brief Max value.
     */
    WIDGET_ID__MAX,
} widget_id_t;

typedef struct widget_pos_size
{
    ImVec2 pos;  /**< Position. */
    ImVec2 size; /**< Size. */
} widget_pos_size_t;

typedef struct widget_layout
{
    widget_pos_size_t filter;
    widget_pos_size_t cover;
    widget_pos_size_t lyric;
    widget_pos_size_t playlist;
    widget_pos_size_t playbar;
    widget_pos_size_t statusbar;
    widget_pos_size_t title;
} widget_layout_t;

class Msg
{
public:
    typedef std::shared_ptr<Msg> Ptr;
    enum Type
    {
        TYPE_REQ,
        TYPE_RSP,
        TYPE_EVT,
    };

public:
    struct Req
    {
        typedef std::shared_ptr<Req>     Ptr;
        typedef std::function<void(Ptr)> Fn;
        virtual ~Req();
    };

    struct Rsp
    {
        typedef std::shared_ptr<Rsp>     Ptr;
        typedef std::function<void(Ptr)> Fn;
        virtual ~Rsp();
    };

    struct Evt
    {
        typedef std::shared_ptr<Evt>     Ptr;
        typedef std::function<void(Ptr)> Fn;
        virtual ~Evt();
    };

    template <typename T>
    class ReqProxy
    {
    public:
        typedef std::function<void(std::shared_ptr<typename T::Req>)> Fn;

        ReqProxy(Fn cb)
        {
            this->cb = cb;
        }

        void proxy_cb(std::shared_ptr<Msg::Req> req)
        {
            auto obj = std::dynamic_pointer_cast<typename T::Req>(req);
            if (cb)
            {
                cb(obj);
            }
        }

    private:
        Fn cb;
    };

    template <typename T>
    class RspProxy
    {
    public:
        typedef std::function<void(std::shared_ptr<typename T::Rsp>)> Fn;

        RspProxy(Fn cb)
        {
            this->cb = cb;
        }

        void proxy_cb(std::shared_ptr<Msg::Rsp> rsp)
        {
            auto obj = std::dynamic_pointer_cast<typename T::Rsp>(rsp);
            if (cb)
            {
                cb(obj);
            }
        }

    private:
        Fn cb;
    };

    class Dispatch
    {
    public:
        Dispatch();
        virtual ~Dispatch();

    public:
        /**
         * @brief Register event handle.
         */
        template <typename T>
        void register_handle(std::function<void(Msg::Ptr)> fn)
        {
            handle_map.insert(HandleMap::value_type(T::ID, fn));
        }

        /**
         * @brief Dispatch message.
         */
        void dispatch(Ptr msg);

        /**
         * @brief Set message mode.
         */
        void set_mode(Msg::Type type);

    private:
        typedef std::function<void(Msg::Ptr)> MsgFn;
        typedef std::map<int, MsgFn>          HandleMap;
        HandleMap                             handle_map;
        Msg::Type                             accept_type;
    };

public:
    Msg(widget_id_t id, uint64_t msgid, Req::Ptr req, Rsp::Fn fn);
    Msg(const Msg *req, Rsp::Ptr rsp);
    Msg(uint64_t msgid, Evt::Ptr evt);
    virtual ~Msg();

public:
    /**
     * @brief Get payload type.
     */
    Type type() const;

    /**
     * @brief Get request payload.
     */
    template <typename T>
    std::shared_ptr<typename T::Req> get_req() const
    {
        return std::dynamic_pointer_cast<typename T::Req>(req);
    }

    /**
     * @brief Get response payload.
     */
    template <typename T>
    std::shared_ptr<typename T::Rsp> get_rsp() const
    {
        return std::dynamic_pointer_cast<typename T::Rsp>(rsp);
    }

    /**
     * @brief Get event payload.
     */
    template <typename T>
    std::shared_ptr<typename T::Evt> get_evt() const
    {
        return std::dynamic_pointer_cast<typename T::Evt>(evt);
    }

public:
    template <typename T, typename... _Args>
    static std::shared_ptr<typename T::Req> make_req(_Args &&...__args)
    {
        return std::make_shared<typename T::Req>(std::forward<_Args>(__args)...);
    }

    template <typename T, typename... _Args>
    static std::shared_ptr<typename T::Rsp> make_rsp(_Args &&...__args)
    {
        return std::make_shared<typename T::Rsp>(std::forward<_Args>(__args)...);
    }

    template <typename T, typename... _Args>
    static std::shared_ptr<typename T::Evt> make_evt(_Args &&...__args)
    {
        return std::make_shared<typename T::Evt>(std::forward<_Args>(__args)...);
    }

public:
    Type        msg_type;  /**< Message type. */
    widget_id_t widget_id; /**< Widget that receive this message. */
    uint64_t    msg_id;    /**< Message ID. */
    Req::Ptr    req;       /**< Request payload. */
    Rsp::Ptr    rsp;       /**< Response payload. */
    Evt::Ptr    evt;       /**< Event payload. */
    Rsp::Fn     rsp_call;  /**< Proxy response call. */
};

typedef struct widget
{
    /**
     * @brief Initialize addon.
     */
    void (*init)(void);

    /**
     * @brief Cleanup addon.
     */
    void (*exit)(void);

    /**
     * @brief Draw widget UI.
     */
    void (*draw)(void);

    /**
     * @brief (Optional) Message handle.
     */
    void (*message)(Msg::Ptr msg);
} widget_t;

#define SOUNDSPHERE_EXPAND_WIDGET_MAP_AS_EXTERN(a, b) extern const widget_t b;
SOUNDSPHERE_WIDGET_TABLE(SOUNDSPHERE_EXPAND_WIDGET_MAP_AS_EXTERN)
#undef SOUNDSPHERE_EXPAND_WIDGET_MAP_AS_EXTERN

/**
 * @brief UI layout for ui_* widgets.
 */
extern widget_layout_t _layout;

/**
 * @brief Initialize all addons.
 */
void widget_init(void);

/**
 * @brief Cleanup all addons.
 */
void widget_exit(void);

/**
 * @brief Draw all addons.
 */
void widget_draw(void);

/**
 * @brief Internal function for send message.
 */
int widget_send_msg(Msg::Ptr msg);

/**
 * @brief Send request to widget and wait for response.
 * @note MT-Safe.
 */
template <typename T>
int widget_send_req(widget_id_t id, std::shared_ptr<typename T::Req> req,
                    typename Msg::RspProxy<T>::Fn cb = typename Msg::RspProxy<T>::Fn())
{
    Msg::RspProxy<T> proxy(cb);
    Msg::Rsp::Fn     proxy_fn = std::bind(&Msg::RspProxy<T>::proxy_cb, proxy, std::placeholders::_1);
    Msg::Ptr         msg = std::make_shared<Msg>(id, T::ID, req, proxy_fn);
    return widget_send_msg(msg);
}

/**
 * @brief Send request and does not care about response.
 * @note MT-Safe.
 */
template <typename T, typename... _Args>
int widget_fast_req(widget_id_t id, _Args &&...__args)
{
    auto req = Msg::make_req<T>(std::forward<_Args>(__args)...);
    return widget_send_req<T>(id, req);
}

/**
 * @brief Send response.
 * @note MT-Safe.
 */
template <typename T>
void widget_send_rsp(Msg::Ptr msg, std::shared_ptr<typename T::Rsp> rsp)
{
    Msg::Ptr obj = std::make_shared<Msg>(msg.get(), rsp);
    widget_send_msg(obj);
}

/**
 * @brief Send response.
 * @note MT-Safe.
 */
template <typename T, typename... _Args>
void widget_fast_rsp(Msg::Ptr msg, _Args &&...__args)
{
    auto rsp = Msg::make_rsp<T>(std::forward<_Args>(__args)...);
    widget_send_rsp<T>(msg, rsp);
}

/**
 * @brief Send event.
 * @note MT-Safe.
 */
template <typename T>
void widget_send_evt(std::shared_ptr<typename T::Evt> evt)
{
    Msg::Ptr obj = std::make_shared<Msg>(T::ID, evt);
    widget_send_msg(obj);
}

/**
 * @brief Send event.
 * @note MT-Safe.
 */
template <typename T, typename... _Args>
void widget_fast_evt(_Args &&...__args)
{
    auto evt = Msg::make_evt<T>(std::forward<_Args>(__args)...);
    widget_send_evt<T>(evt);
}

} // namespace soundsphere

#endif
