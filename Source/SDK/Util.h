#pragma once
#include "Global.h"
#include "SDK/Fibers/Fiber.h"

namespace app 
{
	enum class Convert
	{
		title_case,
		small_case,
		caps
	};
}

namespace app::util
{
	std::filesystem::path get_path();

	uint8_t char_to_byte(char c);
	std::optional<std::uint8_t> char_to_hex(const char c);

	void error_box(std::string error, std::string title = "App");

	std::string convert(const std::string& text, Convert mode);
	std::string hackify(const std::string& text, Convert mode);

	template <typename T>
	concept Callable = std::is_invocable_v<T>;
	template <Callable Action>
	void do_timed(DWORD delay, Action&& action) {
		auto const start = std::chrono::high_resolution_clock::now();
		action();
		auto const end = std::chrono::high_resolution_clock::now();

		Fiber::get()->yield(std::chrono::milliseconds(delay) - (end - start));
	}

	uint32_t joaat(const std::string_view& text);

	// Used for formatting stuff    
	// I Have to use inline because of the template 
	template<typename... Args>
	inline std::string format_message(const std::string& message, const Args&... args)
	{
		return std::vformat(message, std::make_format_args(args...));
	} 

	inline void error(const std::string& message, const auto&... args)
	{
		std::string logMessage = format_message(message, args...);
		error_box(logMessage);
	}

	inline std::unordered_map<std::type_index, bool> g_executed_map; 

    #define do_once(block)                     \
    do {                                   \
        static std::once_flag init_flag;   \
        std::call_once(init_flag, block);  \
    } while (0)

	// Ik They are in std::string but sometimes if you are working with const char* :/
	// Thanks bomuld <3 -- Fixed a memory leak
	template <typename... Args>
	std::string combine_strings(Args&&... args) {
		static auto combine = [](const auto&... strs) -> std::string {
			return (std::string(strs) + ...);
	    };
		return combine(std::forward<Args>(args)...);
	}
}
