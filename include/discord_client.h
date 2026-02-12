#pragma once
#include <iostream>
#include <chrono>
#include <ctime>
#include "discordpp.h"
#include "media_session.h"

class DiscordClient {
public:
    DiscordClient();
    ~DiscordClient();

    void init();
    void runCallbacks();
    void updateRichPresence(const TrackInfo& track);
private:
    const uint64_t app_id = 1206479426884345927LL;

    discordpp::Client client;
    discordpp::Activity activity{};
};