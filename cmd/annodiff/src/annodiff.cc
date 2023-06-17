#include <iostream>
#include <fstream>
#include <memory>
#include <sstream>

#include <spdlog/fmt/fmt.h>
#include <pugixml.hpp>
#include "cxxopts.hpp"

#include "xml_operations.h"
#include "xml_auto_serializer.h"
#include "xml_filedb_reader.h"

namespace fs = std::filesystem;

std::stringstream _logss;

void _annolog(const std::string& msg, const std::string& param) {
    std::cout << msg << param << std::endl;
    //_logss << msg << std::endl;
}

class AnnoXml {
public:
    AnnoXml(const fs::path& file_path) {
        auto doc = xmlops::XmlAutoSerializer::read(file_path);
        parse_assets(*doc);
    }

    AnnoXml(const pugi::xml_document& doc) {
        parse_assets(doc);
    }

    [[nodiscard]] std::string get(const std::string& guid) const {
        auto iter = assets_.find(guid);
        if (iter == assets_.end())
            return "";

        return iter->second;
    }

    [[nodiscard]] std::string diff(const AnnoXml& patched) {
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

    [[nodiscard]] std::string get_touched() const {
        std::stringstream ss;
        for (auto& asset : original_) {
            ss << asset;
        }
        return ss.str();
    }

    [[nodiscard]] std::string get_mode() const {
        return mode_;
    }

private:
    std::map<std::string, std::string> assets_;
    std::vector<std::string> original_;
    std::string mode_;

    void parse_assets(pugi::xml_node node) {
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
};

std::string _strip_quotes(const std::string& path) {
    if (path.front() == '\"' && path.back() == '\"') {
        return path.substr(0, path.size() - 2);
    }
    return path;
}

int main(int argc, char** argv) {
    clock_t start, end;
    start = clock();

    cxxopts::Options options("annodiff");

    options.add_options("diff")
        ("verb", "verb", cxxopts::value<std::string>())
        ("target", "Target XML to patch", cxxopts::value<std::string>())
        ("patch", "Path to patch XML", cxxopts::value<std::string>())
        ("modpath", "Path to mod folder", cxxopts::value<std::string>())
        ;

    options.parse_positional({ "verb", "target", "patch", "modpath" });
    cxxopts::ParseResult params;

    try {
        params = options.parse(argc, argv);
    }
    catch (const cxxopts::exceptions::exception& e) {
        std::cerr << e.what() << std::endl;
        std::cerr << options.help() << std::endl;
        return EXIT_FAILURE;
    }

    std::string_view verb;
    if (params.count("verb") == 1) {
        verb = params["verb"].as<std::string>(); 
    }

    std::ostream& out = std::cout;
    // std::ofstream out;
    // const fs::path output_patched_path = fs::current_path() / "assets_patched_stripped.xml";
    // out.open(output_patched_path);

    /*if (verb.compare("diff") == 0) {
        AnnoXml original(result["target"].as<std::string>());
        AnnoXml patched(result["patch"].as<std::string>());
        
        const fs::path output_patched_path = fs::current_path() / "assets_patched_stripped.xml";
        std::ofstream patched_stripped;
        patched_stripped.open(output_patched_path);
        patched_stripped << "<Assets>" << std::endl << original.diff(patched) << "</Assets>" << std::endl;
        patched_stripped.close();

        const fs::path output_original_path = fs::current_path() / "assets_original_stripped.xml";
        std::ofstream original_stripped;
        original_stripped.open(output_original_path);
        original_stripped << "<Assets>" << std::endl << original.get_touched() << "</Assets>" << std::endl;
        original_stripped.close();
    }
    else*/ 
    if (verb.compare("show") == 0) {
        auto target_doc = xmlops::XmlAutoSerializer::read(params["target"].as<std::string>());
        const auto xpath = "//Asset[Values/Standard/GUID='" + params["patch"].as<std::string>() + "']";
        const auto nodes = target_doc->select_nodes(xpath.c_str());
        for (pugi::xpath_node node : nodes) {
            node.node().print(out, "\t");
            out << std::endl;
        }
    }
    // else if (verb.compare("strip") == 0) {
    //     auto target_doc = read_xml(params["target"].as<std::string>());
    //     const std::string xpath = "//Asset[Values/Standard/GUID]";
    //     const auto nodes = target_doc->select_nodes(xpath.c_str());
    //     for (pugi::xpath_node node : nodes) {
    //         node.node().print(out, "\t");
    //         out << std::endl;
    //     }
    // }
    else if (verb.compare("measure") == 0) {
        auto target_doc = xmlops::XmlAutoSerializer::read(params["target"].as<std::string>());
        const std::string output_file = params["modpath"].as<std::string>();
        const fs::path mods_folder = params["patch"].as<std::string>();

        
    }
    else if (verb.compare("patchdiff") == 0) {
        auto target_doc = xmlops::XmlAutoSerializer::read(params["target"].as<std::string>());
        if (!target_doc) {
            std::cout << "stop" << std::endl;
            return 0;
        }
        const std::string game_path = ""; // only for logging
        const std::string mod_path = params["modpath"].as<std::string>();
        const fs::path file_path = _strip_quotes(params["patch"].as<std::string>());
        const std::string mod_name = file_path.filename().string(); // only for logging

        std::cout << fmt::format("Patch {}", params["target"].as<std::string>()) << std::endl;

        std::string patch_content;
        for (std::string line; std::getline(std::cin, line); ) {
            patch_content += line + "\n";
        }

        auto context = std::make_shared<XmlOperationContext>(patch_content.c_str(), patch_content.size(), file_path, mod_name,
            [&mod_path, &mod_name](const fs::path& file_path) {
                return std::make_shared<XmlOperationContext>(file_path, mod_path, mod_name);
            }
        );
        auto operations = XmlOperation::GetXmlOperations(context, game_path);

        auto start = std::chrono::high_resolution_clock::now();
        for (auto& operation : operations) {
            operation.Apply(target_doc);
        }
        auto end = std::chrono::high_resolution_clock::now();

        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        std::cout << fmt::format("ModOp time: {:.3f}s", duration.count() / 1000.0f) << std::endl;

        const auto inner_extension = file_path.stem().extension();
        xmlops::XmlAutoSerializer::fix(target_doc.get(), file_path.stem());
        if (inner_extension == L".cfg" || inner_extension == L".fc") {
            const std::string open_mode = "<Config>";
            const std::string close_mode = "</Config>";

            out << "##annodiff##";
            target_doc->print(out, "  ");

            out << "##annodiff##";
            xmlops::XmlAutoSerializer::read(params["target"].as<std::string>())->print(out, "  ");
        }
        else {
            AnnoXml original(params["target"].as<std::string>());
            AnnoXml patched(*target_doc);

            const std::string open_mode = "<" + patched.get_mode() + "s>";
            const std::string close_mode = "</" + patched.get_mode() + "s>";

            out << "##annodiff##";
            out << open_mode << std::endl << original.diff(patched) << close_mode << std::endl;

            out << "##annodiff##";
            out << open_mode << std::endl << original.get_touched() << close_mode << std::endl;
        }
    }
    else {
        std::cout << options.help() << std::endl;
    }

    // out.close();
}
