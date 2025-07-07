
#include <sstream>

#include "pugixml.hpp"

#include "anno_xml.h"
#include "xml_auto_serializer.h"

namespace fs = std::filesystem;

AnnoXml::AnnoXml(const fs::path& file_path) {
    auto doc = xmlops::XmlAutoSerializer::read(file_path);
    parse_assets(*doc);
}

AnnoXml::AnnoXml(const pugi::xml_document& doc) {
    parse_assets(doc);
}

[[nodiscard]] std::string AnnoXml::get(const std::string& guid) const {
    auto iter = assets_.find(guid);
    if (iter == assets_.end())
        return "";

    return iter->second;
}

[[nodiscard]] std::string AnnoXml::diff(const AnnoXml& patched) {
    original_.clear();
    std::stringstream ss;

    // const int max_assets = 20;
    // int encountered_assets = 0;

    for (auto& asset : assets_) {
        const auto& guid = asset.first;
        const auto& original_text = asset.second;

        auto patched_text = patched.get(guid);
        if (patched_text != original_text) {
            // encountered_assets ++;
            // if (encountered_assets < max_assets) 
            {
                original_.push_back(original_text);
                ss << patched_text;
            }
        }
    }

    // TODO make new items position correct
    for (auto& asset : patched.assets_) {
        const auto& guid = asset.first;
        const auto& patched_text = asset.second;

        const auto& original_text = get(guid);
        if (original_text.empty()) {
            // encountered_assets ++;
            // if (encountered_assets < max_assets) 
            {
                ss << patched_text;
            }
        }
    }

    return ss.str();
}

[[nodiscard]] std::string AnnoXml::get_touched() const {
    std::stringstream ss;
    for (auto& asset : original_) {
        ss << asset;
    }
    return ss.str();
}

void AnnoXml::parse_assets(pugi::xml_node node) {
    for (auto& child : node.children()) {
        auto name = std::string_view(child.name());
        std::string id;
        if (name == "Asset") {
            const auto guid_node = child.child("Values").child("Standard").child("GUID");
            id = guid_node.text().as_string();
        }
        else if (name == "Text") {
            const auto guid_node = child.child("GUID");
            id = guid_node.text().as_string();
        }
        else if (name == "Template") {
            const auto guid_node = child.child("Name");
            id = guid_node.text().as_string();
        }
        else if (name == "InfoTipData") {
            const auto guid_node = child.child("Guid");
            id = guid_node.text().as_string();
        }

        if (!id.empty()) {
            std::stringstream ss;
            child.print(ss, "\t");
            assets_[id] = ss.str();

            if (mode_.empty()) {
                mode_ = name;
            }
        }
        else {
            parse_assets(child);
        }
    }
}
