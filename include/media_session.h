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

std::string to_string(winrt::hstring const& hstr);

TrackInfo getMediaSessionTrack();
