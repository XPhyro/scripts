// @CXXFLAGS -I/usr/include/obs/
// @LDFLAGS -I/usr/include/obs/ -lobs

#include <cerrno>
#include <csignal>
#include <cstdlib>
#include <optional>
#include <sstream>
#include <unordered_map>

#include <unistd.h>

#include <obs/obs-frontend-api.h>
#include <obs/obs-module.h>

OBS_DECLARE_MODULE()

std::unordered_map<enum obs_frontend_event, const char*> event_sounds = {
    { OBS_FRONTEND_EVENT_STREAMING_STARTED, "stream-started.mp3" },
    { OBS_FRONTEND_EVENT_STREAMING_STOPPED, "stream-stopped.mp3" },
    { OBS_FRONTEND_EVENT_RECORDING_STARTED, "recording-started.mp3" },
    { OBS_FRONTEND_EVENT_RECORDING_STOPPED, "recording-stopped.mp3" },
    { OBS_FRONTEND_EVENT_REPLAY_BUFFER_STARTED, "replay-buffer-started.mp3" },
    { OBS_FRONTEND_EVENT_REPLAY_BUFFER_STOPPED, "replay-buffer-stopped.mp3" },
    { OBS_FRONTEND_EVENT_RECORDING_PAUSED, "recording-paused.mp3" },
    { OBS_FRONTEND_EVENT_RECORDING_UNPAUSED, "recording-resumed.mp3" },
    { OBS_FRONTEND_EVENT_REPLAY_BUFFER_SAVED, "replay-saved.mp3" },
};

std::optional<decltype(fork())> play_sound(const char* filename)
{
    static std::stringstream ss;
    ss.str("");
    ss.clear();

    ss << obs_get_module_data_path(obs_current_module()) << "/" << filename;

    errno = 0;
    if (auto pid = fork(); pid < 0 || errno) {
        perror("fork");
        return std::nullopt;
    } else if (pid == 0) {
        execlp("play", "play", "-q", "--", ss.str().c_str(), NULL);
        std::exit(EXIT_SUCCESS);
    } else {
        return { pid };
    }
}

void event_callback(enum obs_frontend_event event, [[maybe_unused]] void* private_data)
{
    if (!event_sounds.contains(event))
        return;

    static decltype(play_sound("")) last_pid;
    if (last_pid) {
        kill(*last_pid, SIGTERM);
        last_pid.reset();
    }
    last_pid = play_sound(event_sounds.at(event));
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
