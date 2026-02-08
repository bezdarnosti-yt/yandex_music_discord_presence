#pragma once

#include <Windows.h>
#include <string>
#include <iostream>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Media.Control.h>

struct TrackInfo {
    std::string title;
    std::string artist;
    std::string album;
    int current_sec;
    int duration_sec;
    bool is_playing;
    bool found;
};

std::string to_string(winrt::hstring const& hstr) {
    std::wstring wstr(hstr.c_str());
    int size = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
    std::string result(size - 1, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &result[0], size, nullptr, nullptr);
    return result;
}

TrackInfo getMediaSessionTrack() {
    TrackInfo info{};
    info.found = false;

    try {
        auto manager =
            winrt::Windows::Media::Control::
            GlobalSystemMediaTransportControlsSessionManager::RequestAsync().get();

        auto sessions = manager.GetSessions();

        for (uint32_t i = 0; i < sessions.Size(); ++i) {
            auto session = sessions.GetAt(i);
            if (!session) continue;

            if (winrt::to_string(session.SourceAppUserModelId())
                != "ru.yandex.desktop.music")
                continue;

            auto mediaProps = session.TryGetMediaPropertiesAsync().get();
            if (mediaProps) {
                info.title  = winrt::to_string(mediaProps.Title());
                info.artist = winrt::to_string(mediaProps.Artist());
                info.album  = winrt::to_string(mediaProps.AlbumTitle());
            }

            auto playbackInfo = session.GetPlaybackInfo();
            info.is_playing =
                playbackInfo.PlaybackStatus() ==
                winrt::Windows::Media::Control::
                GlobalSystemMediaTransportControlsSessionPlaybackStatus::Playing;

            auto timeline = session.GetTimelineProperties();
            info.current_sec  = int(timeline.Position().count() / 10000000);
            info.duration_sec = int(timeline.EndTime().count() / 10000000);

            info.found = true;
            return info;
        }
    }
    catch (const winrt::hresult_error& e) {
        std::cerr << "[YandexMusic ERROR] "
                  << winrt::to_string(e.message()) << "\n";
    }

    return info;
}