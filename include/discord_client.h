#pragma once
#include <iostream>
#include "discord.h"
#include "media_session.h"

class DiscordClient {
public:
    DiscordClient();
    ~DiscordClient();

    void updateRichPresence(const TrackInfo& track);

    discord::Core* getCore();
private:
    long long app_id = 1206479426884345927LL;
    
    discord::ClientId client_id;
    discord::Core* core{};
    discord::Result result;

    void init();
};