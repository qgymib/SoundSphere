#include <ev.h>
#include "__init__.hpp"

struct SS_LibC_Thread
{
    ev_os_thread_t handle;
};

static int _export_thread_create(SS_LibC_Thread** thread, void(*proc)(void*), void* arg)
{
    if (thread == nullptr)
    {
        return -EINVAL;
    }

    *thread = new SS_LibC_Thread;
    int ret = ev_thread_init(&(*thread)->handle, nullptr, proc, arg);
    if (ret != 0)
    {
        delete (*thread);
        *thread = nullptr;
    }
    return ret;
}

static int _export_thread_join(SS_LibC_Thread* thread, uint32_t timeout)
{
    int ret = ev_thread_exit(&thread->handle, timeout);
    if (ret == 0)
    {
        delete thread;
    }
    return ret;
}

const SS_LibC soundsphere::export_com_libc = {
    _export_thread_create,
    _export_thread_join,
};
