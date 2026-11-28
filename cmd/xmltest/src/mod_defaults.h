#pragma once

#include <filesystem>
#include <map>
#include <string>
#include <vector>

struct ModEnvironment {
    std::map<std::string, std::string> variables;
    std::map<std::filesystem::path, std::string> mod_ids;

    std::string ModID(const std::filesystem::path& root) const;
};

ModEnvironment ReadModDefaults(const std::filesystem::path& root,
                              const std::vector<std::filesystem::path>& dependencies);
