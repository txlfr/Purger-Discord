#include "Global.h"
#include "Discord.h"

namespace app::discord
{
    User::User() : id(""), username(""), discriminator(""), avatar(""),
        bot(false), system(false), mfa_enabled(false), authorization("")
    {
    }

	User::User(const std::string& token)
	{
        // CPR MY BELOVED
        cpr::Response response = cpr::Get(
            cpr::Url{ "https://discord.com/api/v10/users/@me" },
            cpr::Header{ {"Authorization", token} }
        );

        if (response.status_code != 200) {
            console::error("Failed to retrieve user info. HTTP Status: {}", response.status_code);
            return;
        }

        auto json_data = nlohmann::json::parse(response.text);

        id = json_data.value("id", "");
        username = json_data.value("username", "");
        discriminator = json_data.value("discriminator", "");
        avatar = json_data.value("avatar", "");
        bot = json_data.value("bot", false);
        system = json_data.value("system", false);
        mfa_enabled = json_data.value("mfa_enabled", false);

        console::sub_header(console::detail::gradient_text(std::format("Logged into {}({})", username, id))); // autistic code forgive me'

        authorization = token;
	}

    bool User::send_message(const std::string& channel_id, const std::string& message)
    {
        std::string api_endpoint = std::format("https://discord.com/api/v10/channels/{}/messages", channel_id);

        nlohmann::json payload = {
            {"content", message}
        };

        cpr::Response response = cpr::Post(
            cpr::Url{ api_endpoint },
            cpr::Header{
                {"Authorization", authorization},
                {"Content-Type", "application/json"}
            },
            cpr::Body{ payload.dump() }
        );

        if (response.status_code == 429) // Rate limit
        {
            auto retry_after = nlohmann::json::parse(response.text)["retry_after"].get<int>();
            console::warn("Rate limit hit. Retrying after {} ms", retry_after);
               Fiber::get()->yield(retry_after);
            return send_message(channel_id, message);
        }

        if (response.status_code == 200 || response.status_code == 201)
        {
            console::info("Message sent successfully to channel: {}", channel_id);
            return true;
        }

        console::error("Failed to send message. HTTP Status: {} | Response: {}", response.status_code, response.text);
        return false;
    }

    std::map<std::string, std::string, std::greater<std::string>> User::get_messages(const std::string& channel_id)
    {
        std::map<std::string, std::string, std::greater<std::string>> messages;
        std::string before;

        while (true)
        {
            Fiber::get()->yield(500ms);

            std::string api_endpoint = "https://discord.com/api/v10/channels/" + channel_id + "/messages?limit=100" +
                (before.empty() ? "" : "&before=" + before);
            cpr::Response response = cpr::Get(
                cpr::Url{ api_endpoint },
                cpr::Header{ {"Authorization", authorization} }
            );

            if (response.status_code == 429) // Rate limit
            {
                auto retry_after = nlohmann::json::parse(response.text)["retry_after"].get<int>();
                console::warn("Rate limit hit. Retrying after {} ms", retry_after);
                Fiber::get()->yield(retry_after);
                continue;
            }

            if (response.status_code == 200)
            {
                auto json_data = nlohmann::json::parse(response.text);

                if (json_data.empty())
                    break;

                for (const auto& message : json_data)
                {
                    if (message.contains("author") && message["author"].contains("id") &&
                        message.contains("id") && message.contains("content"))
                    {
                        if (message["author"]["id"] == id)
                        {
                            messages[message["id"]] = message["content"];
                        }
                    }
                }

                before = json_data.back()["id"].get<std::string>();

                if (json_data.size() < 100)
                {
                    break;
                }
            }
            else
            {
                console::error("Failed to fetch messages. HTTP Status: {} | Response: {}", response.status_code, response.text);
                break;
            }
        }

        return messages;
    }

    bool User::delete_message(const std::string& channel_id, const std::string& message_id)
    {
        std::string api_endpoint = std::format("https://discord.com/api/v10/channels/{}/messages/{}", channel_id, message_id);

        cpr::Response response = cpr::Delete(
            cpr::Url{ api_endpoint },
            cpr::Header{ {"Authorization", authorization} }
        );

        if (response.status_code == 429) 
        {
            auto retry_after = nlohmann::json::parse(response.text)["retry_after"].get<int>();
            console::warn("Rate limit hit. Retrying after {} ms", retry_after);
               Fiber::get()->yield(retry_after);
            return delete_message(channel_id, message_id);
        }

        if (response.status_code == 204)
        {
            console::info("Message deleted successfully. Channel: {} | Message ID: {}", channel_id, message_id);
            return true;
        }

        console::error("Failed to delete message. HTTP Status: {} | Response: {}", response.status_code, response.text);
        return false;
    }


}