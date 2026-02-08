#include <iostream>
#include <thread>
#include <chrono>
#include <ctime>
#include <string>
#include <Windows.h>
#include "discord.h"
#include "media_session.h"
#include "tray_icon.h"

void updateRichPresence(discord::Core* core, const TrackInfo& track) {
    discord::Activity activity{};

    activity.GetAssets().SetLargeImage("channels4_profile");
    
    if (!track.found) {
        activity.SetState("Not playing...");
        activity.SetDetails("Yandex Music");
        activity.SetType(discord::ActivityType::Listening);
    } else {
        activity.SetDetails((track.title + " — " + track.artist).c_str());
        activity.SetType(discord::ActivityType::Listening);
        
        activity.GetAssets().SetLargeText(track.album.c_str());
        
        if (track.is_playing) {
            activity.GetAssets().SetSmallImage("play");
            activity.GetAssets().SetSmallText("Playing");
            activity.SetState("Playing");
        } else {
            activity.GetAssets().SetSmallImage("pause");
            activity.GetAssets().SetSmallText("Paused");
            activity.SetState("Paused");
        }
    }
    
    core->ActivityManager().UpdateActivity(activity, [](discord::Result result) {});
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    winrt::init_apartment();
    
    discord::ClientId client_id = 1206479426884345927LL;
    
    TrayIcon tray;
    
    WNDCLASSW wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"YandexMusicRPC";
    RegisterClassW(&wc);
    
    HWND hwnd = CreateWindowW(L"YandexMusicRPC", L"Yandex Music RPC", 0, 0, 0, 0, 0, NULL, NULL, hInstance, &tray);
    
    discord::Core* core{};
    auto result = discord::Core::Create(client_id, DiscordCreateFlags_Default, &core);
    
    if (result != discord::Result::Ok) {
        MessageBoxW(NULL, L"Failed to initialize Discord SDK", L"Error", MB_ICONERROR);
        return -1;
    }
    
    for (int i = 0; i < 30; i++) {
        core->RunCallbacks();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    TrackInfo lastTrack{};
    int updateCounter = 0;

    TrackInfo currentTrack = getMediaSessionTrack();
    updateRichPresence(core, currentTrack);
    lastTrack = currentTrack;
    
    MSG msg;
    while (tray.running) {
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        
        core->RunCallbacks();
        
        if (updateCounter % 20 == 0) {
            TrackInfo currentTrack = getMediaSessionTrack();
            
            if (currentTrack.found != lastTrack.found ||
                currentTrack.title != lastTrack.title || 
                currentTrack.artist != lastTrack.artist ||
                currentTrack.is_playing != lastTrack.is_playing) {
                
                updateRichPresence(core, currentTrack);
                lastTrack = currentTrack;
            }
        }
        
        updateCounter++;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    tray.Remove();
    winrt::uninit_apartment();
    delete core;
    return 0;
}