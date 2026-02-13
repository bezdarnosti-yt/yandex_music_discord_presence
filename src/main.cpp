#include <iostream>
#include <thread>
#include <chrono>
#include <string>
#include <Windows.h>

#include "media_session.h"
#include "tray_icon.h"
#include "discord_client.h"
#include "album_art_finder.h"
#include <spdlog/spdlog.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // Терминал для отладки, можно удалить в релизной компиляции
    if (AllocConsole()) {
        FILE* pCout;
        freopen_s(&pCout, "CONOUT$", "w", stdout);
        SetConsoleTitle("Debug Console");
        std::cout.clear();
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
    TrackInfo currentTrack{};
    std::string currentCover = "";

    int updateCounter = 0;
    
    MSG msg;
    while (tray.running) {
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        
        // Проверяем состояние каждые полсекунды
        if (updateCounter % 5 == 0) {
            spdlog::info("Cycle updated!");

            currentTrack = getMediaSessionTrack();

            bool needsUpdate = false;
            
            // Проверка на смену трека
            if (currentTrack.found != lastTrack.found ||
                currentTrack.title != lastTrack.title || 
                currentTrack.artist != lastTrack.artist) {
                
                currentCover = getAlbumCoverUrl(currentTrack.artist, currentTrack.title);
                needsUpdate = true;

                spdlog::info("Track changed!");
            }

            // Проверка на изменение статуса воспроизведения
            if (currentTrack.is_playing != lastTrack.is_playing) {
                needsUpdate = true;
                spdlog::info("Track paused / play!");
            }

            // Проверка на перемотку
            if (currentTrack.found && lastTrack.found) {
                int timeDiff = abs(currentTrack.current_sec - lastTrack.current_sec);
                if (timeDiff > 2 && timeDiff < currentTrack.duration_sec - 2) {
                    needsUpdate = true;
                    spdlog::info("Track ff!");
                }
            }

            // Обновляем Discord при изменениях
            if (needsUpdate) {
                spdlog::info("RPC needs to updated!");
                currentTrack.album_cover_url = currentCover;
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