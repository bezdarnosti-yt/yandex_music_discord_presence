#include <iostream>
#include <thread>
#include <chrono>
#include <string>
#include <Windows.h>

#include "discord.h"

#include "media_session.h"
#include "tray_icon.h"
#include "discord_client.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    winrt::init_apartment();
    
    TrayIcon tray;
    
    WNDCLASSW wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"YandexMusicRPC";
    RegisterClassW(&wc);
    
    HWND hwnd = CreateWindowW(L"YandexMusicRPC", L"Yandex Music RPC", 0, 0, 0, 0, 0, NULL, NULL, hInstance, &tray);
    
    DiscordClient client;
    
    TrackInfo lastTrack{};
    int updateCounter = 0;

    TrackInfo currentTrack = getMediaSessionTrack();
    client.updateRichPresence(currentTrack);
    lastTrack = currentTrack;
    
    MSG msg;
    while (tray.running) {
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        
        client.getCore()->RunCallbacks();
        
        if (updateCounter % 20 == 0) {
            currentTrack = getMediaSessionTrack();
            
            if (currentTrack.found != lastTrack.found ||
                currentTrack.title != lastTrack.title || 
                currentTrack.artist != lastTrack.artist ||
                currentTrack.is_playing != lastTrack.is_playing) {
                
                client.updateRichPresence(currentTrack);
                lastTrack = currentTrack;
            }
        }
        
        updateCounter++;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    tray.Remove();
    winrt::uninit_apartment();
    return 0;
}