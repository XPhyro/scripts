// @CXXFLAGS -I/usr/include/obs/
// @LDFLAGS -I/usr/include/obs/ -lobs

#include <cerrno>
#include <cstdlib>
#include <sstream>

#include <unistd.h>

#include <obs/obs-frontend-api.h>
#include <obs/obs-module.h>

OBS_DECLARE_MODULE()

void play_sound(const char* filename)
{
    static std::stringstream ss;
    ss.str("");
    ss.clear();

    ss << obs_get_module_data_path(obs_current_module()) << "/" << filename;

    errno = 0;
    if (auto pid = fork(); pid < 0 || errno) {
        perror("fork");
    } else if (pid == 0) {
        execlp("play", "play", "-q", "--", ss.str().c_str(), NULL);
        std::exit(EXIT_SUCCESS);
    }
}

void event_callback(enum obs_frontend_event event, [[maybe_unused]] void* private_data)
{
    const char* filename = NULL;

    switch (event) {
        case OBS_FRONTEND_EVENT_STREAMING_STARTED:
            filename = "stream-started.mp3";
            break;
        case OBS_FRONTEND_EVENT_RECORDING_STARTED:
            filename = "recording-started.mp3";
            break;
        case OBS_FRONTEND_EVENT_REPLAY_BUFFER_STARTED:
            filename = "replay-buffer-started.mp3";
            break;
        case OBS_FRONTEND_EVENT_RECORDING_PAUSED:
            filename = "recording-paused.mp3";
            break;
        case OBS_FRONTEND_EVENT_STREAMING_STOPPED:
            filename = "stream-stopped.mp3";
            break;
        case OBS_FRONTEND_EVENT_RECORDING_STOPPED:
            filename = "recording-stopped.mp3";
            break;
        case OBS_FRONTEND_EVENT_REPLAY_BUFFER_STOPPED:
            filename = "replay-buffer-stopped.mp3";
            break;
        case OBS_FRONTEND_EVENT_RECORDING_UNPAUSED:
            filename = "recording-resumed.mp3";
            break;
        case OBS_FRONTEND_EVENT_REPLAY_BUFFER_SAVED:
            filename = "replay-saved.mp3";
            break;
        default:
            break;
    }

    play_sound(filename);
}

const char* obs_module_author(void)
{
    return "Berke Kocaoğlu";
}

const char* obs_module_name(void)
{
    return "event-sounds";
}

const char* obs_module_description(void)
{
    return "Play sounds on user events.";
}

bool obs_module_load(void)
{
    obs_frontend_add_event_callback(event_callback, 0);
    return true;
}

void obs_module_unload(void) {}
