#pragma once
#include "Global.h"
#undef max 
#undef min
namespace app::console 
{
    namespace detail 
    {
        class Color
        {
        public:
            std::uint64_t m_r{}, m_g{}, m_b{};
        public:
            void ensure_max() {
                m_r = std::max(0, std::min(255, (int)m_r));
                m_g = std::max(0, std::min(255, (int)m_g));
                m_b = std::max(0, std::min(255, (int)m_b));
            }
        public:
            Color operator+(const Color& other) const {
                return Color(m_r + other.m_r, m_g + other.m_g, m_b + other.m_b);
            }
        };


        void print_colored_text(Color col, const std::string& text);
        std::string get_colored_string(Color col, char text);
        std::string get_colored_string(int id, std::string text);
        std::string gradient_text(const std::string& text);
    }

    inline std::ofstream g_console_out;
    inline std::ofstream g_file_out;

    inline std::filesystem::path g_log_path;

    inline HANDLE g_console_handle;
    inline DWORD g_original_console_mode;

    void create(const std::string_view& name);
    void destroy();

    void setup_folder();
    void sub_header(std::string text);
    void setup_console(const std::string_view& name);

    void send(const std::string& title, const std::string& message);

    void raw_send(const std::string& text);

    inline void warn(const std::string& message, const auto&... args)
    {
        std::string logMessage = util::format_message(message, args...);
        send("Warn", logMessage);
    }

    inline void info(const std::string& message, const auto&... args)
    {
        std::string logMessage = util::format_message(message, args...);
        send("Info", logMessage);
    }

    inline void error(const std::string& message, const auto&... args)
    {
        std::string logMessage = util::format_message(message, args...);
        send("Error", logMessage);
    }

    inline void grad(const std::string& title, const std::string& message, const auto&... args)
    {
        std::string logMessage = util::format_message(message, args...);
        send(title, console::detail::gradient_text(logMessage));
    }
}

