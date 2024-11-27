#include "Global.h"
#include "Console.h"
#undef max
#undef min

namespace app
{
    std::string get_current_time() 
    {
        std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
        std::time_t now_c = std::chrono::system_clock::to_time_t(now);

        tm now_tm{};
        if (localtime_s(&now_tm, &now_c) == 0) 
        {
            return std::format("{:02}:{:02}:{:02}", now_tm.tm_hour, now_tm.tm_min, now_tm.tm_sec);
        }

        return "Error retrieving time";
    }

    std::string get_time()
    {
        auto const current_time = std::chrono::system_clock::now();
        auto const time = std::chrono::system_clock::to_time_t(current_time);

        tm local_time{};

        if (localtime_s(&local_time, &time) == 0) {
            return std::format("{:0>2}-{:0>2}-{}-{:0>2}-{:0>2}-{:0>2}",
                local_time.tm_mon + 1, local_time.tm_mday, local_time.tm_year + 1900,
                local_time.tm_hour, local_time.tm_min, local_time.tm_sec
            );
        }

        return "Error retrieving time";
    }

    namespace console
    {
        namespace detail
        {
            void print_colored_text(Color col, const std::string& text) 
            {
                std::cout << std::format("\033[38;2;{};{};{}m{}\033[0m\n", col.m_r, col.m_g, col.m_b, text);
            }

            std::string get_colored_string(Color col, char text)
            {
                return std::format("\033[38;2;{};{};{}m{}\033[0m", col.m_r, col.m_g, col.m_b, text); 
            }

            std::string get_colored_string(int id, std::string text) {
                return std::format("\033[38;5;{}m{}\033[0m", id, text);
            }

            std::string gradient_text(const std::string& text)
            {
                std::string tmp{};
                std::uint64_t fraction{ 3 / std::min((std::uint64_t)text.length(), (std::uint64_t)50) };
                Color indigo{ 120, 137, 225 };
                Color delta{ 15 * fraction, 5 * fraction, 15 * fraction };

                for (std::uint64_t i{}; i < text.length(); ++i)
                {
                    Color current{ indigo.m_r + i * delta.m_r, indigo.m_g + i * delta.m_g, indigo.m_b + i * delta.m_b };
                    current.ensure_max();
                    tmp += get_colored_string(current, text[i]);
                }
                return tmp;
            }

            void print_lines_with_gradient(std::vector<std::string> lines)
            {
                Color indigo{ 120, 137, 225 }; // Gradient Color
                Color delta{ 8, 3, 8 };
                for (std::uint64_t i{}; i < lines.size(); ++i) 
                {
                    Color current{ indigo.m_r + (i * delta.m_r), indigo.m_g + (i * delta.m_g), indigo.m_b + (i * delta.m_b) };
                    current.ensure_max();
                    print_colored_text(current, lines[i]);
                }
            }
        }

        void setup_folder()
        {
            if (!std::filesystem::exists(g_log_path))
            {
                std::filesystem::create_directories(g_log_path);
            }

            g_log_path /= std::filesystem::path(std::format("{}.txt", get_time()));
        }

        void sub_header(std::string text)
        {
            g_console_out << std::format("{}+{} {}", "["_gradient, "]"_gradient, text) << std::endl;
        }

        void setup_console(const std::string_view& name)
        {
            if (AllocConsole())
            {
                if (g_console_handle = GetStdHandle(STD_OUTPUT_HANDLE); g_console_handle != nullptr)
                {
                    SetConsoleTitleA(name.data());
                    SetConsoleOutputCP(CP_UTF8);

                    DWORD console_mode{};
                    if (GetConsoleMode(g_console_handle, &console_mode)) 
                    {
                        g_original_console_mode = console_mode;

                        console_mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING | DISABLE_NEWLINE_AUTO_RETURN | ENABLE_PROCESSED_OUTPUT | ENABLE_WRAP_AT_EOL_OUTPUT;
                        console_mode &= ~(ENABLE_QUICK_EDIT_MODE);

                        if (!SetConsoleMode(g_console_handle, console_mode))
                        {
                            util::error("Failed to set console mode\n {}", GetLastError());
                        }
                    }
                }
            }
            else 
            {
                util::error("Failed to allocate console \n {}", GetLastError());
            }
        }

        void create(const std::string_view& name)
        {
            g_log_path = util::get_path().append("logs");
            g_console_handle = nullptr;
            g_original_console_mode = 0;

            setup_folder();
            setup_console(name);
            g_console_out.open("CONOUT$", std::ios_base::out | std::ios_base::app);

            if (!g_console_out.is_open()) {
                util::error("Failed to open console output stream");
            }
            g_file_out.open(g_log_path, std::ios_base::out | std::ios_base::app);

            if (!g_file_out.is_open()) {
                util::error("Failed to open log file output stream");
            }
        }

        void destroy()
        {
            if (g_file_out.is_open())
            {
                g_file_out.close();
            }

            if (g_console_out.is_open())
            {
                g_console_out.close();
            }

            if (g_console_handle)
            {
                SetConsoleMode(g_console_handle, g_original_console_mode);
                FreeConsole();
            }
        }

        void send(const std::string& title, const std::string& message)
        {
            g_file_out << get_current_time() << " | [" << util::hackify(title, Convert::caps) << "] > " << message << std::endl;
            g_console_out << detail::gradient_text(get_current_time()) << " | [" << util::hackify(title, Convert::caps) << "] > " << message << std::endl;
        }


        void raw_send(const std::string& text) 
        {
            g_console_out << text << std::endl;
        }
    }
}
