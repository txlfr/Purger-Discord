#include "Global.h"
#include "Util.h"

namespace app::util
{
	std::filesystem::path get_path() 
    {
        char path[MAX_PATH];
        if (SHGetFolderPathA(NULL, CSIDL_PERSONAL, NULL, 0, path) == S_OK) 
        {
            return std::filesystem::path(path).append(APP_NAME);
        }
        else 
        {
            error_box("Failed to get documents path");
            return std::filesystem::path("");
        }
	}

    uint8_t char_to_byte(char c)
    {
        constexpr uint8_t offset_0 = '0';
        constexpr uint8_t offset_a = 'a' - 10;
        constexpr uint8_t offset_A = 'A' - 10;

        c |= 0x20; // Convert uppercase to lowercase

        if (c >= '0' && c <= '9') return static_cast<uint8_t>(c - offset_0);
        if (c >= 'a' && c <= 'f') return static_cast<uint8_t>(c - offset_a);
        return 0;
    }

    std::optional<std::uint8_t> char_to_hex(const char c)
    {
        if (c >= 'a' && c <= 'f')
            return static_cast<std::uint8_t>(static_cast<std::uint32_t>(c) - 87);
        if (c >= 'A' && c <= 'F')
            return static_cast<std::uint8_t>(static_cast<std::uint32_t>(c) - 55);
        if (c >= '0' && c <= '9')
            return static_cast<std::uint8_t>(static_cast<std::uint32_t>(c) - 48);
        return {};
    }

    uint32_t joaat(const std::string_view& text)
    {
        uint32_t hash = 0;
        for (const char c : text)
        {
            hash += c;
            hash += (hash << 10);
            hash ^= (hash >> 6);
        }
        hash += (hash << 3);
        hash ^= (hash >> 11);
        hash += (hash << 15);
        return hash;
    }

    void error_box(std::string error, std::string title)
    {
        MessageBoxA(nullptr, error.c_str(), title.c_str(), MB_OK);
    }

    std::string convert_to_title_case(const std::string& text) 
    {
        std::string result = text;
        bool capitalize_next = true;
        for (char& c : result)
        {
            if (std::isalpha(c)) 
            {
                c = capitalize_next ? std::toupper(c) : std::tolower(c);
                capitalize_next = false;
            }
            else
            {
                capitalize_next = true;
            }
        }
        return result;
    }

    std::string convert_to_small_case(const std::string& text)
    {
        std::string result = text;
        std::transform(result.begin(), result.end(), result.begin(),
            [](unsigned char c) { return std::tolower(c); });
        return result;
    }

    std::string convert_to_caps(const std::string& text) 
    {
        std::string result = text;
        std::transform(result.begin(), result.end(), result.begin(),
            [](unsigned char c) { return std::toupper(c); });
        return result;
    }

    std::string convert(const std::string& text, Convert mode) 
    {
        switch (mode) {
        case Convert::title_case:
            return convert_to_title_case(text);
        case Convert::small_case:
            return convert_to_small_case(text);
        case Convert::caps:
            return convert_to_caps(text);
        default:
            return text;
        }
    }

    std::string hackify(const std::string& text, Convert mode)
    {
        static const std::array<char, 256> hack_map = []() {
            std::array<char, 256> map{};
            for (int i = 0; i < 256; ++i) // hate visual studio warnings so lol 
            {
                map[i] = static_cast<char>(i);
            }
            map['I'] = map['i'] = '1';
            map['E'] = map['e'] = '3';
            map['A'] = map['a'] = '4';
            map['O'] = map['o'] = '0';
            return map;
            }();

        std::string result = text;
        for (char& c : result)
        {
            c = hack_map[static_cast<unsigned char>(c)]; // safe index to the map
        }

        return convert(result, mode);
    }
}