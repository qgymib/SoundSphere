#include <imgui.h>
#include <queue>
#include <mutex>
#include "__init__.hpp"

typedef std::queue<soundsphere::JobDataBase::JobPtr> JobQueue;

soundsphere::runtime_t soundsphere::_G;

static JobQueue* s_job_queue = nullptr;
static std::mutex* s_job_mutex = nullptr;

void soundsphere::runtime_init(void)
{
    soundsphere::_G.playlist_select_idx = -1;
    soundsphere::_G.playlist_play_idx = -1;
    soundsphere::_G.volume = 0;
    soundsphere::_G.music_type = MUS_NONE;
    soundsphere::_G.music_duration = 0.0;
    soundsphere::_G.music_position = 0.0;

    s_job_queue = new JobQueue;
    s_job_mutex = new std::mutex;
}

void soundsphere::runtime_exit(void)
{
    if (s_job_queue != nullptr)
    {
        delete s_job_queue;
        s_job_queue = nullptr;
    }

    if (s_job_mutex != nullptr)
    {
        delete s_job_mutex;
        s_job_mutex = nullptr;
    }
}

void soundsphere::runtime_submit_job(JobDataBase::JobPtr job)
{
    std::unique_lock<std::mutex> lock(*s_job_mutex);
    s_job_queue->emplace(job);
}

void soundsphere::runtime_loop(void)
{
    for (;;)
    {
        JobDataBase::JobPtr job;
        {
            std::unique_lock<std::mutex> lock(*s_job_mutex);
            if (s_job_queue->empty())
            {
                return;
            }
            job = s_job_queue->front();
            s_job_queue->pop();
        }

        job->call();
    }
}
