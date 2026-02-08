#include <iostream>
#include <thread>
#include <chrono>
#include <ctime>
#include <string>
#include "discord.h"
#include "media_session.h"

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
    
    core->ActivityManager().UpdateActivity(activity, [](discord::Result result) {
        if (result == discord::Result::Ok) {
            std::cout << "Rich Presence updated" << std::endl;
        }
    });
}

int main() {
    winrt::init_apartment();
    
    discord::ClientId client_id = 1206479426884345927LL;
    
    std::cout << "Init Discord SDK..." << std::endl;
    
    discord::Core* core{};
    auto result = discord::Core::Create(client_id, DiscordCreateFlags_Default, &core);
    
    if (result != discord::Result::Ok) {
        std::cerr << "\nInit error Discord SDK!" << std::endl;
        return -1;
    }
    
    std::cout << "Discord SDK initialized!" << std::endl;
    
    for (int i = 0; i < 30; i++) {
        core->RunCallbacks();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    std::cout << "Yandex Music Discord RPC launched!" << std::endl;
    
    TrackInfo lastTrack{};
    int updateCounter = 0;

    TrackInfo currentTrack = getMediaSessionTrack();
    updateRichPresence(core, currentTrack);
    lastTrack = currentTrack;
    
    while (true) {
        core->RunCallbacks();
        
        if (updateCounter % 20 == 0) {
            TrackInfo currentTrack = getMediaSessionTrack();
            
            if (currentTrack.title != lastTrack.title || 
                currentTrack.artist != lastTrack.artist ||
                currentTrack.is_playing != lastTrack.is_playing) {
                
                if (currentTrack.found) {
                    std::cout << "\n" << currentTrack.title << std::endl;
                    std::cout << currentTrack.artist << std::endl;
                    
                    if (currentTrack.is_playing) {
                        std::cout << "Playing" << std::endl;
                    } else {
                        std::cout << "Paused" << std::endl;
                    }
                } else {
                    std::cout << "\nSong doesn't found" << std::endl;
                }
                
                updateRichPresence(core, currentTrack);
                lastTrack = currentTrack;
            }
        }
        
        updateCounter++;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    winrt::uninit_apartment();
    delete core;
    return 0;
}