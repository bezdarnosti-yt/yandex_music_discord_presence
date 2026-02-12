#include <iostream>
#include <thread>
#include <chrono>
#include <string>
#include <Windows.h>

#include "media_session.h"
#include "tray_icon.h"
#include "discord_client.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // Debug purpose
    if (AllocConsole()) {
        FILE* pCout;
        freopen_s(&pCout, "CONOUT$", "w", stdout); // Redirect stdout
        SetConsoleTitle("Debug Console");
        std::cout.clear(); // Clear the error state for the standard stream objects
    }

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
    int currentTime = 0;
    int prevTime = 0;

    TrackInfo currentTrack = getMediaSessionTrack();
    client.updateRichPresence(currentTrack);
    lastTrack = currentTrack;
    
    MSG msg;
    while (tray.running) {
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        
        if (updateCounter % 10 == 0) {
            currentTrack = getMediaSessionTrack();
            
            if (currentTrack.found != lastTrack.found ||
                currentTrack.title != lastTrack.title || 
                currentTrack.artist != lastTrack.artist) {
                
                lastTrack = currentTrack;

                currentTime = 0;
            }

            if (currentTrack.is_playing != true) {
                currentTrack.current_sec = currentTime;
                client.updateRichPresence(currentTrack);
                updateCounter += 1;
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                continue;
            }

            if (currentTrack.current_sec != prevTime) {
                prevTime = currentTrack.current_sec;
                currentTime = prevTime;
            }

            currentTime += 1;
            currentTrack.current_sec = currentTime;

            client.updateRichPresence(currentTrack);
        }
        
        updateCounter++;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    tray.Remove();
    winrt::uninit_apartment();
    return 0;
}