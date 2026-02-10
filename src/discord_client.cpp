#include "discord_client.h"

DiscordClient::DiscordClient() {
    DiscordClient::init();
}

DiscordClient::~DiscordClient() {
    delete this->core;
}

void DiscordClient::init() {
    this->client_id = app_id;
    this->result = discord::Core::Create(this->client_id, DiscordCreateFlags_Default, &this->core);

    if (this->result != discord::Result::Ok) {
        MessageBoxW(NULL, L"Failed to initialize Discord SDK", L"Error", MB_ICONERROR);
    }

    for (int i = 0; i < 30; i++) {
        this->core->RunCallbacks();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

discord::Core* DiscordClient::getCore() {
    return this->core;
}

void DiscordClient::updateRichPresence(const TrackInfo& track) {
    discord::Activity activity{};

    activity.GetAssets().SetLargeImage("channels4_profile");
    
    if (!track.found) {
        activity.SetState("Not playing...");
        activity.SetDetails("Yandex Music");
        activity.SetType(discord::ActivityType::Playing);
    } else {
        activity.SetDetails((track.title + " — " + track.artist).c_str());
        activity.SetType(discord::ActivityType::Playing);

        activity.GetAssets().SetLargeText(track.album.c_str());

        int currentTime = track.current_sec;
        int maxTime = track.duration_sec;

        std::string s_currentTime;
        if ((currentTime / 60) < 10) {
            s_currentTime.append("0");
        }
        s_currentTime.append(std::to_string(currentTime / 60));
        s_currentTime.append(":");
        if ((currentTime % 60) < 10) {
            s_currentTime.append("0");
        }
        s_currentTime.append(std::to_string(currentTime % 60));

        std::string s_maxTime;
        if ((maxTime / 60) < 10) {
            s_maxTime.append("0");
        }
        s_maxTime.append(std::to_string(maxTime / 60));
        s_maxTime.append(":");
        if ((maxTime % 60) < 10) {
            s_maxTime.append("0");
        }
        s_maxTime.append(std::to_string(maxTime % 60));

        std::string s_finalResult = s_currentTime + "/" + s_maxTime;
        
        if (track.is_playing) {
            activity.GetAssets().SetSmallImage("play");
            activity.GetAssets().SetSmallText("Playing");
            activity.SetState(s_finalResult.c_str());
        } else {
            activity.GetAssets().SetSmallImage("pause");
            activity.GetAssets().SetSmallText("Paused");
            activity.SetState("Paused");
        }

        std::cout << "updated" << std::endl;
    }
    
    this->core->ActivityManager().UpdateActivity(activity, [](discord::Result result) {});
}
