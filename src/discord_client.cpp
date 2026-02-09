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
    
    this->core->ActivityManager().UpdateActivity(activity, [](discord::Result result) {});
}
