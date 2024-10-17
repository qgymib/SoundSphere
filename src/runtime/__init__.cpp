#include <imgui.h>
#include <queue>
#include <mutex>
#include "config/__init__.hpp"
#include "__init__.hpp"

typedef std::queue<soundsphere::JobDataBase::JobPtr> JobQueue;

soundsphere::runtime_t soundsphere::_G;

typedef struct runtime_ctx
{
    runtime_ctx();

    JobQueue job_queue;
    std::mutex job_mutex;
} runtime_ctx_t;

static runtime_ctx_t *s_runtime_ctx = nullptr;

runtime_ctx::runtime_ctx()
{
}

soundsphere::runtime::runtime()
{
    playlist.selected_id = (uint64_t)-1;

    playbar.is_playing = false;
    playbar.music_duration = 0.0;
    playbar.music_position = 0.0;
}

static void _runtime_save_playlist(void)
{
    soundsphere::MusicTagPtrVecPtr vec = soundsphere::_G.media_list;
    soundsphere::_config.songs.clear();

    soundsphere::MusicTagPtrVec::iterator it = vec->begin();
    for (; it != vec->end(); it++)
    {
        soundsphere::MusicTagPtr tag = *it;
        soundsphere::_config.songs.push_back(tag->path);
    }
}

void soundsphere::runtime_init(void)
{
    s_runtime_ctx = new runtime_ctx_t;
    soundsphere::_G.media_list = music_read_tag_v(soundsphere::_config.songs);
}

void soundsphere::runtime_exit(void)
{
    _runtime_save_playlist();

    delete s_runtime_ctx;
    s_runtime_ctx = nullptr;
}

void soundsphere::runtime_submit_job(JobDataBase::JobPtr job)
{
    std::unique_lock<std::mutex> lock(s_runtime_ctx->job_mutex);
    s_runtime_ctx->job_queue.emplace(job);
}

void soundsphere::runtime_loop(void)
{
    for (;;)
    {
        JobDataBase::JobPtr job;
        {
            std::unique_lock<std::mutex> lock(s_runtime_ctx->job_mutex);
            if (s_runtime_ctx->job_queue.empty())
            {
                return;
            }
            job = s_runtime_ctx->job_queue.front();
            s_runtime_ctx->job_queue.pop();
        }

        job->call();
    }
}
