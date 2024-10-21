#include <ev.h>
#include <imgui.h>
#include <list>
#include <vector>
#include "i18n/__init__.h"
#include "runtime/__init__.hpp"
#include "__init__.hpp"

using namespace soundsphere;

typedef std::list<Msg::Ptr> MsgQueue;

typedef struct widget_item
{
    widget_item(widget_id_t id, const widget_t *widget);
    ~widget_item();

    widget_id_t     id;
    const widget_t *widget;

    ev_mutex_t req_queue_mutex;
    MsgQueue   req_queue;
} widget_item_t;

typedef std::vector<widget_item_t *> WidgetItemVec;

typedef struct widget_ctx
{
    widget_ctx();
    ~widget_ctx();

    WidgetItemVec widgets;

    ev_mutex_t rsp_queue_mutex;
    MsgQueue   rsp_queue;

    ev_mutex_t evt_queue_mutex;
    MsgQueue   evt_queue;
} widget_ctx_t;

widget_layout_t      soundsphere::_layout;
static widget_ctx_t *s_widget_ctx = nullptr;

widget_item::widget_item(widget_id_t id, const widget_t *widget)
{
    this->id = id;
    this->widget = widget;
    ev_mutex_init(&req_queue_mutex, 0);
}

widget_item::~widget_item()
{
    ev_mutex_exit(&req_queue_mutex);
}

widget_ctx::widget_ctx()
{
#define EXPAND_WIDGET_MAP_AS_VEC(a, b)                                                                                 \
    do                                                                                                                 \
    {                                                                                                                  \
        widget_item_t *item = new widget_item_t(a, &b);                                                                \
        widgets.push_back(item);                                                                                       \
    } while (0);
    SOUNDSPHERE_WIDGET_TABLE(EXPAND_WIDGET_MAP_AS_VEC);
#undef EXPAND_WIDGET_MAP_AS_VEC

    ev_mutex_init(&rsp_queue_mutex, 0);
    ev_mutex_init(&evt_queue_mutex, 0);
}

widget_ctx::~widget_ctx()
{
    while (!widgets.empty())
    {
        widget_item_t *widget = widgets.back();
        widgets.pop_back();
        delete widget;
    }

    ev_mutex_exit(&evt_queue_mutex);
    ev_mutex_exit(&rsp_queue_mutex);
}

Msg::Req::~Req()
{
}

Msg::Rsp::~Rsp()
{
}

Msg::Evt::~Evt()
{
}

Msg::Msg(widget_id_t id, uint64_t msgid, Msg::Req::Ptr req, Msg::Rsp::Fn fn)
{
    this->msg_type = TYPE_REQ;
    this->widget_id = id;
    this->msg_id = msgid;
    this->req = req;
    this->rsp_call = fn;
}

Msg::Msg(const Msg *req, Rsp::Ptr rsp)
{
    this->msg_type = TYPE_RSP;
    this->widget_id = WIDGET_ID__MAX;
    this->msg_id = req->msg_id;
    this->rsp_call = req->rsp_call;
    this->rsp = rsp;
}

Msg::Msg(uint64_t msgid, Evt::Ptr evt)
{
    this->msg_type = TYPE_EVT;
    this->widget_id = WIDGET_ID__MAX;
    this->msg_id = msgid;
    this->evt = evt;
}

Msg::~Msg()
{
}

Msg::Type Msg::type() const
{
    return msg_type;
}

Msg::Dispatch::Dispatch()
{
    this->accept_type = Msg::TYPE_REQ;
}

Msg::Dispatch::~Dispatch()
{
}

void Msg::Dispatch::dispatch(Msg::Ptr msg)
{
    if (msg->msg_type != accept_type)
    {
        return;
    }

    uint64_t            msg_id = msg->msg_id;
    HandleMap::iterator it = handle_map.find(msg_id);
    if (it == handle_map.end())
    {
        return;
    }

    (it->second)(msg);
}

void Msg::Dispatch::set_mode(Msg::Type type)
{
    this->accept_type = type;
}

static float _widget_get_main_menu_bar_height(void)
{
    float main_menu_bar_height = 0;
    if (ImGui::BeginMainMenuBar())
    {
        /* The height of MainMenuBar is can be get here. */
        main_menu_bar_height = ImGui::GetWindowHeight();
        ImGui::EndMainMenuBar();
    }
    return main_menu_bar_height;
}

static float _widget_get_cover_width(ImGuiIO &io)
{
    float cover_width_x = io.DisplaySize.x * 0.5f;
    float cover_width_y = io.DisplaySize.y * 0.5f;
    float cover_width = cover_width_x < cover_width_y ? cover_width_x : cover_width_y;
    return cover_width > 256.0f ? 256.0f : cover_width;
}

static float _widget_get_filter_width(ImGuiIO &io)
{
    float filter_width = io.DisplaySize.x * 0.3f;
    return filter_width > 256.0f ? 256.0f : filter_width;
}

void soundsphere::widget_init(void)
{
    s_widget_ctx = new widget_ctx_t;

    WidgetItemVec::iterator it = s_widget_ctx->widgets.begin();
    for (; it != s_widget_ctx->widgets.end(); it++)
    {
        widget_item_t *widget = *it;
        widget->widget->init();
    }
}

void soundsphere::widget_exit(void)
{
    for (size_t i = s_widget_ctx->widgets.size(); i > 0; i--)
    {
        widget_item_t *widget = s_widget_ctx->widgets[i - 1];
        widget->widget->exit();
    }

    delete s_widget_ctx;
    s_widget_ctx = nullptr;
}

static void _widget_draw_update_pos_size(void)
{
    ImGuiIO &io = ImGui::GetIO();

    const float main_menu_bar_height = _widget_get_main_menu_bar_height();
    const float status_bar_height = ImGui::GetFrameHeight() * 1.5f;
    const float play_bar_height = ImGui::GetFrameHeight() * 1.7f;
    const float filter_width = _widget_get_filter_width(io);
    const float cover_width = _widget_get_cover_width(io);
    const float title_height = 64.0f;

    soundsphere::_layout.statusbar.size.x = io.DisplaySize.x;
    soundsphere::_layout.statusbar.size.y = status_bar_height;
    soundsphere::_layout.statusbar.pos.x = 0;
    soundsphere::_layout.statusbar.pos.y = io.DisplaySize.y - status_bar_height;

    soundsphere::_layout.playbar.size.x = io.DisplaySize.x;
    soundsphere::_layout.playbar.size.y = play_bar_height;
    soundsphere::_layout.playbar.pos.x = 0;
    soundsphere::_layout.playbar.pos.y = io.DisplaySize.y - (status_bar_height + play_bar_height);

    soundsphere::_layout.filter.size.x = filter_width;
    soundsphere::_layout.filter.size.y =
        io.DisplaySize.y - (main_menu_bar_height + play_bar_height + status_bar_height);
    soundsphere::_layout.filter.pos.x = 0;
    soundsphere::_layout.filter.pos.y = main_menu_bar_height;

    soundsphere::_layout.cover.size.x = cover_width;
    soundsphere::_layout.cover.size.y = cover_width;
    soundsphere::_layout.cover.pos.x = io.DisplaySize.x - cover_width;
    soundsphere::_layout.cover.pos.y = main_menu_bar_height;

    soundsphere::_layout.title.size.x = cover_width;
    soundsphere::_layout.title.size.y = title_height;
    soundsphere::_layout.title.pos.x = io.DisplaySize.x - cover_width;
    soundsphere::_layout.title.pos.y = main_menu_bar_height + cover_width;

    soundsphere::_layout.lyric.size.x = cover_width;
    soundsphere::_layout.lyric.size.y =
        io.DisplaySize.y - (main_menu_bar_height + cover_width + title_height + status_bar_height + play_bar_height);
    soundsphere::_layout.lyric.pos.x = io.DisplaySize.x - cover_width;
    soundsphere::_layout.lyric.pos.y = main_menu_bar_height + cover_width + title_height;

    soundsphere::_layout.playlist.size.x = io.DisplaySize.x - filter_width - cover_width;
    soundsphere::_layout.playlist.size.y =
        io.DisplaySize.y - main_menu_bar_height - play_bar_height - status_bar_height;
    soundsphere::_layout.playlist.pos.x = filter_width;
    soundsphere::_layout.playlist.pos.y = main_menu_bar_height;
}

static void _soundsphere_process_all_requests(widget_item_t *widget)
{
    bool                  have_msg;
    soundsphere::Msg::Ptr msg;

    do
    {
        have_msg = false;
        ev_mutex_enter(&widget->req_queue_mutex);
        if (!widget->req_queue.empty())
        {
            have_msg = true;
            msg = widget->req_queue.front();
            widget->req_queue.pop_front();
        }
        ev_mutex_leave(&widget->req_queue_mutex);

        if (have_msg && widget->widget->message != nullptr)
        {
            widget->widget->message(msg);
        }
    } while (have_msg == true);
}

static void _soundsphere_process_all_response(void)
{
    bool     have_msg;
    Msg::Ptr msg;

    do
    {
        have_msg = false;
        ev_mutex_enter(&s_widget_ctx->rsp_queue_mutex);
        if (!s_widget_ctx->rsp_queue.empty())
        {
            have_msg = true;
            msg = s_widget_ctx->rsp_queue.front();
            s_widget_ctx->rsp_queue.pop_front();
        }
        ev_mutex_leave(&s_widget_ctx->rsp_queue_mutex);

        if (have_msg && msg->rsp_call)
        {
            msg->rsp_call(msg->rsp);
        }
    } while (have_msg == true);
}

static void _soundsphere_process_all_events(void)
{
    bool     have_msg;
    Msg::Ptr msg;

    do
    {
        have_msg = false;
        ev_mutex_enter(&s_widget_ctx->evt_queue_mutex);
        if (!s_widget_ctx->evt_queue.empty())
        {
            have_msg = true;
            msg = s_widget_ctx->evt_queue.front();
            s_widget_ctx->evt_queue.pop_front();
        }
        ev_mutex_leave(&s_widget_ctx->evt_queue_mutex);

        if (have_msg)
        {
            WidgetItemVec::iterator it = s_widget_ctx->widgets.begin();
            for (; it != s_widget_ctx->widgets.end(); it++)
            {
                widget_item_t *widget = *it;
                if (widget->widget->message != nullptr)
                {
                    widget->widget->message(msg);
                }
            }
        }
    } while (have_msg == true);
}

void soundsphere::widget_draw(void)
{
    _widget_draw_update_pos_size();
    _soundsphere_process_all_response();
    _soundsphere_process_all_events();

    WidgetItemVec::iterator it = s_widget_ctx->widgets.begin();
    for (; it != s_widget_ctx->widgets.end(); it++)
    {
        widget_item_t *widget = *it;

        _soundsphere_process_all_requests(widget);
        widget->widget->draw();
    }
}

static int _soundsphere_widget_send_req(Msg::Ptr msg)
{
    widget_id_t widget_id = msg->widget_id;

    for (size_t i = 0; i < s_widget_ctx->widgets.size(); i++)
    {
        widget_item_t *widget = s_widget_ctx->widgets[i];
        if (widget->id == widget_id)
        {
            if (widget->widget->message == nullptr)
            {
                return -ENOSYS;
            }
            ev_mutex_enter(&widget->req_queue_mutex);
            widget->req_queue.push_back(msg);
            ev_mutex_leave(&widget->req_queue_mutex);
            return 0;
        }
    }

    return -ENOENT;
}

static int _soundsphere_widget_send_rsp(Msg::Ptr msg)
{
    ev_mutex_enter(&s_widget_ctx->rsp_queue_mutex);
    {
        s_widget_ctx->rsp_queue.push_back(msg);
    }
    ev_mutex_leave(&s_widget_ctx->rsp_queue_mutex);

    return 0;
}

static int _soundsphere_widget_send_evt(Msg::Ptr msg)
{
    ev_mutex_enter(&s_widget_ctx->evt_queue_mutex);
    {
        s_widget_ctx->evt_queue.push_back(msg);
    }
    ev_mutex_leave(&s_widget_ctx->evt_queue_mutex);

    return 0;
}

int soundsphere::widget_send_msg(Msg::Ptr msg)
{
    switch (msg->msg_type)
    {
    case Msg::TYPE_REQ:
        return _soundsphere_widget_send_req(msg);

    case Msg::TYPE_RSP:
        return _soundsphere_widget_send_rsp(msg);

    case Msg::TYPE_EVT:
        return _soundsphere_widget_send_evt(msg);

    default:
        break;
    }

    return -EINVAL;
}
