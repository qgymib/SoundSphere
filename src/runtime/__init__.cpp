#include <imgui.h>
#include <queue>
#include <mutex>
#include "__init__.hpp"

typedef std::queue<soundsphere::JobDataBase::JobPtr> JobQueue;

soundsphere::runtime_t soundsphere::_G;

static JobQueue* s_job_queue = nullptr;
static std::mutex* s_job_mutex = nullptr;

soundsphere::runtime::runtime()
{
    playlist.selected_id = (uint64_t)-1;

    lyric.auto_center_time_ms = 3 * 1000;
    lyric.fore_lyric_color[0] = 1.0f;
    lyric.fore_lyric_color[1] = 0.0f;
    lyric.fore_lyric_color[2] = 0.0f;
    lyric.fore_lyric_color[3] = 1.0f;
    lyric.back_lyric_color[0] = 1.0f;
    lyric.back_lyric_color[1] = 1.0f;
    lyric.back_lyric_color[2] = 1.0f;
    lyric.back_lyric_color[3] = 1.0f;

    playbar.is_playing = false;
    playbar.volume = 50;
    playbar.music_duration = 0.0;
    playbar.music_position = 0.0;

    statusbar.music_type = MUS_NONE;
    statusbar.bitrate = 0;
    statusbar.samplerate = 0;
    statusbar.channels = 0;
}

void soundsphere::runtime_init(void)
{
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
