#ifndef SOUND_SPHERE_RUNTIME_INIT_HPP
#define SOUND_SPHERE_RUNTIME_INIT_HPP

#include <functional>
#include <memory>
#include <SDL_mixer.h>
#include "playitem.hpp"

namespace soundsphere {

/**
 * @brief Global runtime data.
 */
typedef struct runtime
{
    runtime();

    /**
     * @brief Current playlist.
     */
    PlayItem::PtrVecPtr media_list;

    struct
    {
        /**
         * @brief The current playing music.
         */
        PlayItem::Ptr   current_music;
    } dummy_player;

    struct
    {
        /**
         * @brief List shown in ui.
         */
        PlayItem::PtrVecPtr show_vec;

        /**
         * @brief Which item selected.
         */
        uint64_t selected_id;
    } playlist;

    struct
    {
        uint64_t auto_center_time;
    } lyric;

    struct
    {
        /**
         * @brief Is the audio is playing.
         */
        int is_playing;

        /**
         * @brief Volume. [0, 100].
         */
        int volume;

        /**
         * @brief Music duration, in seconds.
         */
        double music_duration;

        /**
         * @brief Music position, in seconds.
         */
        double music_position;
    } playbar;

    struct
    {
        /**
         * @brief Music type.
         */
        Mix_MusicType music_type;

        /**
         * @brief Bitrate in kb/s.
         */
        int bitrate;

        /**
         * @brief Sample rate in Hz.
         */
        int samplerate;

        /**
         * @brief The number of audio channels.
         */
        int channels;
    } statusbar;
} runtime_t;

class JobDataBase
{
public:
    typedef std::shared_ptr<JobDataBase> JobPtr;

public:
    JobDataBase(){};
    virtual ~JobDataBase(){};

public:
    virtual void call(void) = 0;
};

template<typename T>
class JobData : public JobDataBase
{
public:
    typedef std::shared_ptr<T> ObjPtr;
    typedef std::function<void(ObjPtr)> Func;

public:
    JobData(Func func, ObjPtr obj)
    {
        m_func = func;
        m_obj = obj;
    }

    virtual ~JobData(){};

public:
    virtual void call(void)
    {
        m_func(m_obj);
    }

private:
    Func m_func;
    ObjPtr m_obj;
};

/**
 * @brief Global runtime.
 */
extern runtime_t _G;

/**
 * @brief Initialize runtime.
 */
void runtime_init(void);

/**
 * @brief Cleanup runtime.
 */
void runtime_exit(void);

/**
 * @brief Runtime loop.
 */
void runtime_loop(void);

/**
 * @brief Queue job into loop.
 * @warning You should use #runtime_call_in_ui().
 * @param[in] job   Job object.
 */
void runtime_submit_job(JobDataBase::JobPtr job);

/**
 * @brief Submit a job to be called in UI loop.
 * @warning Do not do any blocking operations in UI thread.
 * @note It is recommend to only update resources in UI thread.
 * @param[in] fn    Job callback.
 * @param[in] obj   Job parameter.
 */
template<typename T>
void runtime_call_in_ui(typename JobData<T>::Func fn, typename JobData<T>::ObjPtr obj)
{
    JobDataBase::JobPtr ptr = std::make_shared< JobData<T> >(fn, obj);
    runtime_submit_job(ptr);
}

}

#endif
