#pragma once
#include "Global.h"


namespace app::discord
{
    using json = nlohmann::json;

    class User
    {
    public:
        std::string id;
        std::string username;
        std::string discriminator;
        std::string avatar;
        bool bot;
        bool system;
        bool mfa_enabled;

        std::string authorization;

        User(const std::string& token);
        User();

        bool send_message(const std::string& channel_id, const std::string& message);
        std::map<std::string, std::string, std::greater<std::string>> get_messages(const std::string& channel_id);
        bool delete_message(const std::string& channel_id, const std::string& message_id);
    };

}