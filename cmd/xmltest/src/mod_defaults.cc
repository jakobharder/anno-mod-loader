#include "mod_defaults.h"

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

#include <fstream>
#include <stdexcept>

namespace {

namespace fs = std::filesystem;
using nlohmann::json;

fs::path ModRoot(const fs::path& root) {
    return fs::absolute(root).lexically_normal();
}

std::string XPathString(const std::string& value) {
    if (value.find('\'') == std::string::npos) {
        return "'" + value + "'";
    }
    if (value.find('"') == std::string::npos) {
        return "\"" + value + "\"";
    }
    std::string result = "concat(";
    size_t start = 0;
    for (size_t end; (end = value.find('\'', start)) != std::string::npos; start = end + 1) {
        result += "'" + value.substr(start, end - start) + "',\"'\",";
    }
    return result + "'" + value.substr(start) + "')";
}

// XPath 1.0 number literals do not support JSON's exponent notation.
std::string XPathNumber(const json& number) {
    auto value = number.dump();
    const auto exponent = value.find_first_of("eE");
    if (exponent == std::string::npos) {
        return value;
    }
    const bool negative = value.front() == '-';
    auto digits = value.substr(negative ? 1 : 0, exponent - (negative ? 1 : 0));
    const auto dot = digits.find('.');
    const int decimal = static_cast<int>(dot == std::string::npos ? digits.size() : dot)
        + std::stoi(value.substr(exponent + 1));
    if (dot != std::string::npos) {
        digits.erase(dot, 1);
    }
    if (decimal <= 0) {
        digits = "0." + std::string(-decimal, '0') + digits;
    }
    else if (decimal >= static_cast<int>(digits.size())) {
        digits.append(decimal - digits.size(), '0');
    }
    else {
        digits.insert(decimal, ".");
    }
    return (negative ? "-" : "") + digits;
}

std::string DefaultExpression(const json& option) {
    if (!option.is_object() || !option.contains("default")) {
        throw std::runtime_error("option requires a default");
    }
    const auto& value = option.at("default");
    const bool toggle = option.contains("type") && option.at("type") == "toggle";
    if (value.is_boolean()) {
        return value.get<bool>() ? "true" : "false";
    }
    if (value.is_string()) {
        const auto text = value.get<std::string>();
        if (text == "true" || text == "false") {
            return text;
        }
        if (toggle) {
            throw std::runtime_error("toggle default must be true or false (lowercase), not a number or other string");
        }
        const auto number = json::parse(text, nullptr, false);
        if (number.is_number()) {
            return XPathNumber(number);
        }
        return XPathString(text);
    }
    if (!toggle && value.is_number()) {
        return XPathNumber(value);
    }
    throw std::runtime_error(toggle ? "toggle default must be true or false (lowercase), not a number or other string"
                                    : "default must be a string, number, or boolean");
}

void AddMod(ModEnvironment& environment, const fs::path& path) {
    const auto root = ModRoot(path);
    if (environment.mod_ids.count(root)) {
        return;
    }
    auto filename = root / "modinfo.jsonc";
    if (!fs::exists(filename)) {
        filename = root / "modinfo.json";
    }
    if (!fs::exists(filename)) {
        return;
    }
    try {
        std::ifstream input(filename);
        if (!input) {
            throw std::runtime_error("cannot read modinfo");
        }
        const auto info = json::parse(input, nullptr, true, true);
        if (!info.is_object()) {
            throw std::runtime_error("modinfo must be an object");
        }
        if (!info.contains("ModID") && !info.contains("Options")) {
            return;
        }
        if (!info.contains("ModID") || !info.at("ModID").is_string()
            || info.at("ModID").get<std::string>().empty()) {
            throw std::runtime_error("a non-empty ModID is required for option defaults");
        }
        const auto id = info.at("ModID").get<std::string>();
        if (environment.variables.count(id)) {
            throw std::runtime_error("duplicate ModID: " + id);
        }
        environment.mod_ids[root] = id;
        environment.variables[id] = "true";
        if (!info.contains("Options")) {
            return;
        }
        if (!info.at("Options").is_object()) {
            throw std::runtime_error("Options must be an object");
        }
        for (const auto& option : info.at("Options").items()) {
            const auto name = id + "." + option.key();
            try {
                environment.variables[name] = DefaultExpression(option.value());
            }
            catch (const std::exception& error) {
                throw std::runtime_error(name + ": " + error.what());
            }
            spdlog::debug("Default ${}={}", name, environment.variables.at(name));
        }
    }
    catch (const std::exception& error) {
        throw std::runtime_error(filename.string() + ": " + error.what());
    }
}

} // namespace

std::string ModEnvironment::ModID(const std::filesystem::path& root) const {
    const auto entry = mod_ids.find(ModRoot(root));
    return entry == mod_ids.end() ? "" : entry->second;
}

ModEnvironment ReadModDefaults(const std::filesystem::path& root,
                              const std::vector<std::filesystem::path>& dependencies) {
    ModEnvironment environment;
    AddMod(environment, root);
    for (const auto& dependency : dependencies) {
        AddMod(environment, dependency);
    }
    return environment;
}
