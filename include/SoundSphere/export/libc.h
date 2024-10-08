#ifndef SOUND_SPHERE_EXPORT_LIBC_H
#define SOUND_SPHERE_EXPORT_LIBC_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct SS_LibC_Thread SS_LibC_Thread;
typedef struct SS_LibC_Mutex SS_LibC_Mutex;


typedef struct SS_LibC
{
    /**
     * @brief Thread
     * @{
     */

    /**
     * @brief Create a thread.
     */
    void (*thread_create)(SS_LibC_Thread** thread, void(*proc)(void*), void* arg);

    /**
     * @brief Wait for thread exit.
     * @param[in] thread    The thread to wait.
     * @param[in] timeout   Timeout in millisecond. Set to -1 to wait infinite.
     * @return 0 if thread exit, and the \p thread is release. Otherwise the
     *   \p thread is not exit, you need to call this function again.
     */
    int (*thread_join)(SS_LibC_Thread* thread, uint32_t timeout);

    /**
     * @}
     */

    /**
     * @brief Mutex
     * @{
     */
    void (*mutex_create)(SS_LibC_Mutex** mutex, int recursive);
    void (*mutex_destroy)(SS_LibC_Mutex* mutex);
    void (*mutex_enter)(SS_LibC_Mutex* mutex);
    void (*mutex_leave)(SS_LibC_Mutex* mutex);
    /**
     * @}
     */
} SS_LibC;

#ifdef __cplusplus
}
#endif
#endif
