#define DISCORDPP_IMPLEMENTATION

#include "discord_client.h"
#include <spdlog/spdlog.h>

DiscordClient::DiscordClient() { DiscordClient::init(); }

DiscordClient::~DiscordClient() {}

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

    discordpp::Activity newActivity{};
    discordpp::ActivityAssets assets;
    newActivity.SetType(discordpp::ActivityTypes::Listening);
    
    if (!track.found) {
        assets.SetLargeImage("channels4_profile");
        newActivity.SetState("Ничего не играет :(");
        newActivity.SetDetails("Яндекс.Музыка");
        newActivity.SetAssets(assets);
    } else {
        assets.SetLargeImage(track.album_cover_url);
        newActivity.SetDetails(track.title.c_str());
        newActivity.SetState(track.artist.c_str());
        
        auto now = std::chrono::system_clock::now();
        discordpp::ActivityTimestamps timestamps;
        
        if (track.is_playing) {
            auto start_time = now - std::chrono::seconds(track.current_sec);
            auto end_time = now + std::chrono::seconds(track.duration_sec - track.current_sec);
            
            timestamps.SetStart(std::chrono::duration_cast<std::chrono::seconds>(
                start_time.time_since_epoch()).count());
            timestamps.SetEnd(std::chrono::duration_cast<std::chrono::seconds>(
                end_time.time_since_epoch()).count());
            
            newActivity.SetTimestamps(timestamps);
            
            assets.SetSmallImage("play");
            assets.SetSmallText("Playing");
        } else {
            assets.SetSmallImage("pause");
            assets.SetSmallText("Paused");
        }

        newActivity.SetAssets(assets);

        discordpp::ActivityButton button;
        button.SetLabel("GitHub проект");
        button.SetUrl("https://github.com/bezdarnosti-yt/yandex_music_discord_presence");
        newActivity.AddButton(button);
    }

    newActivity.SetAssets(assets);
    
    this->client.UpdateRichPresence(
        newActivity, [](const discordpp::ClientResult &result) {
            if (result.Successful()) {
                spdlog::info("Rich presence updated");
            } else {
                spdlog::error("Failed to update rich presence: {}", result.Error());
            }
        }
    );
}
