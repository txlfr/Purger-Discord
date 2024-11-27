#include "Global.h"
#include <windows.h>
#include <wininet.h>
#include <string>
#include <iostream>

#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "urlmon.lib")
#pragma comment(lib, "wininet.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "cpr.lib")
#pragma comment(lib, "libcurl.lib")
#pragma comment(lib, "Crypt32.lib")
#pragma comment(lib, "zlibstatic.lib")

using namespace app;

std::string token = "";
std::string selected_channel_id = "";
int delay = 1000;

using json = nlohmann::json;

void create_default_config(const std::string& config_path)
{
    nlohmann::json default_config = {
        {"token", ""},
        {"delay", 1000},
        {"channelId", ""}
    };

    std::filesystem::path config_file_path = std::filesystem::current_path() / config_path;
    std::ofstream config_file(config_file_path, std::ios::out | std::ios::trunc);
    if (!config_file.is_open())
    {
        console::error("Failed to create configuration file: {}", config_file_path.string());
        console::error("Error message: {}", std::strerror(errno));
        return;
    }

    config_file << default_config.dump(4);
    config_file.close();

    console::info("Default configuration file created at {}", config_file_path.string());
}

void load_config(const std::string& config_path)
{
    std::filesystem::path config_file_path = std::filesystem::current_path() / config_path;
    console::info("Looking for {}", config_file_path.string());
    if (!std::filesystem::exists(config_file_path))
    {
        console::info("Configuration file not found, creating default...");
        create_default_config(config_path);
        console::warn("Please fill in the configuration");
        Fiber::get()->yield(100000);
        exit(0);
    }

    std::ifstream config_file(config_file_path, std::ios::in);

    nlohmann::json config;
    config_file >> config;

    if (config.contains("token"))
    {
        token = config["token"];
    }


    if (config.contains("channelId"))
    {
        selected_channel_id = config["channelId"];
    }


    if (config.contains("delay"))
    {
        delay = config["delay"];
    }


    config_file.close();
}

void app_loop()
{
    std::string config_path = "config.json";
    load_config(config_path);

    discord::User g_user(token); 

    console::raw_send("________________________________________________\n");

    console::info("Getting messages; the more the longer it takes");

    auto messages = g_user.get_messages(selected_channel_id);

    console::info("Found {} messages", messages.size());

    for (auto& message : messages)
    {
        g_user.delete_message(selected_channel_id, message.first);
        console::info("Messsage deleted {}", message.second);
        Fiber::get()->yield(delay);
    }

    //g_user.send_message(selected_channel_id, "Poof !");
    while (true)
    {
        Fiber::get()->yield();
    }
}

int APIENTRY WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    Fiber::ensure_thread_is_a_fiber();

    console::create(APP_NAME);
  
    fiber_manager::add_fiber("Discord", &app_loop);

    while (true)
    {
        fiber_manager::tick();

        std::this_thread::sleep_for(0ms);

        if (GetAsyncKeyState(VK_ESCAPE))
        {
            break;
        }
    }

    console::destroy();
    return 0;
}
