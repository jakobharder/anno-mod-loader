#pragma once

#include <string>
#include <vector>

namespace xmlops {

namespace str {

[[nodiscard]] bool equals_nocase(std::string_view a, std::string_view b);
[[nodiscard]] std::vector<std::string> split(std::string_view input, char delimiter);
[[nodiscard]] std::string join(const std::vector<std::string>& parts, char delimiter, size_t reserve = 0);

template<typename... Args>
std::string concat(const Args&... args) {
    size_t total_size = (0 + ... + std::string_view(args).size());
    std::string result;
    result.reserve(total_size);
    (result.append(args), ...);
    return result;
}

}

}