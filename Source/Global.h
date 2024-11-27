#pragma once
// C++ Headers
#include <windows.h>
#include <shlobj.h>
#include <immintrin.h>
#include <intrin.h> 
#include <Psapi.h>
#include <wtypes.h>
#include <d3d11.h>
#include <wrl/client.h>
#include <d3dcompiler.h>
// Other
#include <cstdint>
#include <type_traits>
#include <string>
#include <filesystem>
#include <ranges>
#include <span>
#include <functional> 
#include <stdexcept> 
#include <mutex>
#include <deque>
#include <iostream>
#include <unordered_set>
#include <fstream>
#include <map>
#include <array>
#include <typeindex>
#define APP_NAME "Purger"
//SDK
#include "SDK/Util.h"
#include "SDK/Console/Console.h"
#include "SDK/Fibers/Fiber.h"
#include "SDK/Fibers/Manager.h"
#include "SDK/Fibers/Queue.h"
// Libraries 
#include <json.hpp>
#include <curl/curl.h>
#include <cpr/cpr.h>
// App
#include "Discord/Discord.h"


inline std::string operator ""_gradient(const char* str, size_t)
{
	return app::console::detail::gradient_text(str);
}

namespace app // Global namespace
{
	inline discord::User g_user{};
}

using namespace std::chrono_literals;