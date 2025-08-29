#include "str_helper.h"

#include <charconv>

namespace xmlops {
namespace str {

bool equals_nocase(std::string_view a, std::string_view b) {
#ifndef _WIN32
    auto strnicmp = [](auto a, auto b) { return strncasecmp(a, b); };
#endif
    if (a.size() != b.size()) {
        return false;
    }
    return strnicmp(a.data(), b.data(), a.size()) == 0;
}

std::vector<std::string> split(std::string_view input, char delimiter) {
    std::vector<std::string> result;

    int last_pos = 0;
    for (int i = 0; i < input.length(); i++) {
        if (input[i] != delimiter) {
            continue;
        }

        if (i - last_pos > 0) {
            result.emplace_back(input.substr(last_pos, i - last_pos));
        }
        last_pos = i + 1;
    }

    if (last_pos != input.length()) {
        result.emplace_back(input.substr(last_pos, input.length() - last_pos));
    }

    return result;
}

std::string join(const std::vector<std::string>& parts, char delimiter, size_t reserve) {
    if (parts.empty()) {
        return {};
    }

    if (reserve == 0) {
        reserve = (parts.size() - 1);
        for (const auto& part : parts) {
            reserve += part.size();
        }
    }

    std::string result;
    result.reserve(reserve);

    result += parts[0];
    for (size_t i = 1; i < parts.size(); ++i) {
        result += delimiter;
        result += parts[i];
    }

    return result;
}

int fromchars(const std::string_view str, const int default_number) {
    int value;

    auto [ptr, ec] = std::from_chars(str.data(), str.data() + str.size(), value);

    if (ec == std::errc()) {
        return value;
    }

    return default_number;
}

}
}