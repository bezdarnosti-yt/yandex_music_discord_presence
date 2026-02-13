#include "media_session.h"

std::string to_string(winrt::hstring const& hstr) {
    std::wstring wstr(hstr.c_str());
    int size = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
    std::string result(size - 1, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &result[0], size, nullptr, nullptr);
    return result;
}

static winrt::Windows::Media::Control::GlobalSystemMediaTransportControlsSessionManager g_manager = nullptr;
static int g_managerRefreshCounter = 0;

TrackInfo getMediaSessionTrack() {
    TrackInfo info{};
    info.found = false;

    try {
        if (!g_manager || g_managerRefreshCounter++ > 50) {
            g_manager = winrt::Windows::Media::Control::
                GlobalSystemMediaTransportControlsSessionManager::RequestAsync().get();
            g_managerRefreshCounter = 0;
        }

        if (!g_manager) return info;

        // Получение всех сессий из Windows Media API
        auto sessions = g_manager.GetSessions();
        if (!sessions || sessions.Size() == 0) return info;

        for (uint32_t i = 0; i < sessions.Size(); ++i) {
            auto session = sessions.GetAt(i);
            if (!session) continue;

            // Выбираем сессию только с Яндекс.Музыкой
            // Теоретически можно под любую программу использовать данный репо
            // Если подогнать album_art_finder под API
            auto appId = winrt::to_string(session.SourceAppUserModelId());
            if (appId != "ru.yandex.desktop.music")
                continue;

            auto mediaProps = session.TryGetMediaPropertiesAsync().get();
            if (!mediaProps) continue;
            
            auto title = winrt::to_string(mediaProps.Title());
            if (title.empty()) continue;

            info.title  = title;
            info.artist = winrt::to_string(mediaProps.Artist());
            info.album  = winrt::to_string(mediaProps.AlbumTitle());

            auto playbackInfo = session.GetPlaybackInfo();
            if (playbackInfo) {
                auto status = playbackInfo.PlaybackStatus();
                info.is_playing = (status == 
                    winrt::Windows::Media::Control::
                    GlobalSystemMediaTransportControlsSessionPlaybackStatus::Playing);
            }

            auto timeline = session.GetTimelineProperties();
            if (timeline) {
                info.current_sec  = int(timeline.Position().count() / 10000000);
                info.duration_sec = int(timeline.EndTime().count() / 10000000);
            }

            info.found = true;
            return info;
        }
    }
    catch (...) {
        g_manager = nullptr;
    }

    return info;
}