#include <ev.h>
#include <cassert>
#include "__init__.hpp"

//////////////////////////////////////////////////////////////////////////
// Thread
//////////////////////////////////////////////////////////////////////////

struct SS_LibC_Thread
{
    ev_os_thread_t handle;
};

static void _export_thread_create(SS_LibC_Thread** thread, void(*proc)(void*), void* arg)
{
    assert(thread != nullptr);

    *thread = (SS_LibC_Thread*)ev_malloc(sizeof(SS_LibC_Thread));
    int ret = ev_thread_init(&(*thread)->handle, nullptr, proc, arg);
    if (ret != 0)
    {
        ev_free(*thread);
        *thread = nullptr;
    }
}

static int _export_thread_join(SS_LibC_Thread* thread, uint32_t timeout)
{
    assert(thread != nullptr);

    int ret = ev_thread_exit(&thread->handle, timeout);
    if (ret == 0)
    {
        ev_free(thread);
    }
    return ret;
}

//////////////////////////////////////////////////////////////////////////
// Mutex
//////////////////////////////////////////////////////////////////////////

struct SS_LibC_Mutex
{
    ev_mutex_t mutex;
};

static void _export_mutex_create(SS_LibC_Mutex** mutex, int recursive)
{
    assert(mutex != nullptr);
    *mutex = (SS_LibC_Mutex*)ev_malloc(sizeof(SS_LibC_Mutex));
    ev_mutex_init(&(*mutex)->mutex, recursive);
}

static void _export_mutex_destroy(SS_LibC_Mutex* mutex)
{
    assert(mutex != nullptr);
    ev_mutex_exit(&mutex->mutex);
    ev_free(mutex);
}

static void _export_mutex_enter(SS_LibC_Mutex* mutex)
{
    assert(mutex != nullptr);
    ev_mutex_enter(&mutex->mutex);
}

static void _export_mutex_leave(SS_LibC_Mutex* mutex)
{
    assert(mutex != nullptr);
    ev_mutex_leave(&mutex->mutex);
}

const SS_LibC soundsphere::export_com_libc = {
    _export_thread_create,
    _export_thread_join,
    _export_mutex_create,
    _export_mutex_destroy,
    _export_mutex_enter,
    _export_mutex_leave,
};
