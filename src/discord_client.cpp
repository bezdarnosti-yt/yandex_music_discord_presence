#define DISCORDPP_IMPLEMENTATION

#include "discord_client.h"

#include <spdlog/spdlog.h>

DiscordClient::DiscordClient() {
    DiscordClient::init();
}

DiscordClient::~DiscordClient() {

}

void DiscordClient::init() {
    this->client.SetApplicationId(this->app_id);
    this->client.AddLogCallback(
        [](const auto &message, auto severity) {
            switch (severity) {
                case discordpp::LoggingSeverity::Info:
                    spdlog::info(message);
                    break;
                case discordpp::LoggingSeverity::Warning:
                    spdlog::warn(message);
                    break;
                case discordpp::LoggingSeverity::Error:
                    spdlog::error(message);
                    break;
                case discordpp::LoggingSeverity::Verbose:
                    spdlog::debug(message);
                    break;
                default:
                    spdlog::error(message);
            }
        },
        discordpp::LoggingSeverity::Warning);
}

void DiscordClient::runCallbacks() {
    discordpp::RunCallbacks();
}

void DiscordClient::updateRichPresence(const TrackInfo& track) {
    discordpp::RunCallbacks();

    discordpp::ActivityAssets assets;

    assets.SetLargeImage("channels4_profile");
    
    if (!track.found) {
        activity.SetState("Not playing...");
        activity.SetDetails("Yandex Music");
        activity.SetType(discordpp::ActivityTypes::Listening);
    } else {
        activity.SetDetails((track.title + " — " + track.artist).c_str());
        activity.SetType(discordpp::ActivityTypes::Listening);

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
            assets.SetSmallImage("play");
            assets.SetSmallText("Playing");
            activity.SetState(s_finalResult.c_str());
        } else {
            assets.SetSmallImage("pause");
            assets.SetSmallText("Paused");
            activity.SetState("Paused");
        }
    }

    activity.SetAssets(assets);
    
    this->client.UpdateRichPresence(
        this->activity, [](const discordpp::ClientResult &result) {
            if (result.Successful()) {
                spdlog::info("Rich presence updated");
            } else {
                spdlog::error("Failed to update rich presence: {}", result.Error());
            }
        }
    );
}
