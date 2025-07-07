#include "xml_operations.h"
#include "xml_auto_serializer.h"

#include "absl/strings/str_cat.h"
#include "pugixml.hpp"
#include "spdlog/spdlog.h"

#include "anno_xml.h"
#include "parse_args.h"

#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

using namespace xmlops;

bool path_equal(const std::filesystem::path& a, const std::filesystem::path& b) {
#ifndef _WIN32
    auto stricmp = [](auto a, auto b) { return strcasecmp(a, b); };
#endif
    return stricmp(a.string().c_str(), b.string().c_str()) == 0;
}

void apply_patch(std::shared_ptr<pugi::xml_document> doc, const fs::path& modPath, const fs::path& patchPath)
{
    fs::path mainPatchFile = patchPath;
    if (patchPath.filename() != "export.bin.xml"
        && patchPath.stem().extension() != ".fc"
        && patchPath.stem().extension() != ".cfg") {
        if (patchPath.filename().string().find("template") == std::string::npos) {
            mainPatchFile = "data/config/export/main/asset/assets.xml";
        }
        else {
            mainPatchFile = "data/config/export/main/asset/templates.xml";
        }
    }
    const fs::path fullPath = modPath / mainPatchFile;
    if (!fs::exists(fullPath)) {
        return;
    }
    spdlog::info("Prepatch: {}", fullPath.string());

    auto operations = XmlOperation::GetXmlOperationsFromFile(fullPath,
        modPath.filename().string(),
        mainPatchFile,
        fs::absolute(modPath));
    for (auto& operation : operations) {
        operation.Apply(doc);
    }
}

int command_show(const XmltestParameters& params, std::ostream& out) {
    auto target_doc = XmlAutoSerializer::read(params.targetPath);
    const auto xpath = "//Asset[Values/Standard/GUID='" + params.patchPath.string() + "']";
    const auto nodes = target_doc->select_nodes(xpath.c_str());
    for (pugi::xpath_node node : nodes) {
        node.node().print(out, "\t");
        out << std::endl;
    }

    return 0;
}

std::shared_ptr<pugi::xml_document> _get_prepatched(const XmltestParameters& params, bool hide = false) {
    // disable debug as we don't want that for prepatch files
    spdlog::set_level(hide ? spdlog::level::critical : spdlog::level::info);
    auto doc = xmlops::XmlAutoSerializer::read(params.targetPath);
    auto patch_game_path = fs::relative(params.patchPath, params.modPaths.front());
    for (auto dep : params.prepatchPaths) {
        apply_patch(doc, dep, patch_game_path);
    }
    spdlog::set_level(params.verbose ? spdlog::level::debug : spdlog::level::info);
    return doc;
}

std::shared_ptr<pugi::xml_document> _patch(std::shared_ptr<pugi::xml_document> doc,
    const XmltestParameters& params, const std::string& patch_content) {

    const auto mod_path = fs::absolute(params.modPaths.front());
    const auto game_path = fs::absolute(params.patchPath).lexically_relative(mod_path);
    spdlog::debug("Game Path: {}", game_path.string());
    const auto mod_name = "xmltest";

    auto loader = [&mod_path, &mod_name, &patch_content, &game_path, &params](const fs::path& file_path) {
        std::vector<char> buffer;
        size_t size;
        spdlog::debug("Include: {}", file_path.string());

        // handle additional paths
        fs::path search_path = mod_path;
        for (auto& path : params.modPaths) {
            if (fs::exists(path / file_path)) {
                search_path = path;
                break;
            }
        }

        if (params.useStdin && path_equal(file_path, params.stdinPath)) {
            return std::make_shared<XmlOperationContext>(patch_content.data(), patch_content.size(), file_path, mod_name);
        }

        // read found (or just mod_path)
        if (!XmlOperationContext::ReadFile(search_path / file_path, buffer, size)) {
            spdlog::error("{}: Failed to open {}", mod_name, file_path.string());
            return std::make_shared<XmlOperationContext>();
        }
        return std::make_shared<XmlOperationContext>(buffer.data(), size, file_path, mod_name);
    };
    auto context = (params.useStdin && path_equal(game_path, params.stdinPath)) ?
        std::make_shared<XmlOperationContext>(patch_content.data(), patch_content.size(), game_path, mod_name, loader) :
        std::make_shared<XmlOperationContext>(game_path, mod_path, mod_name);
    context->SetLoader(loader);

    auto start = std::chrono::high_resolution_clock::now();
    auto operations = XmlOperation::GetXmlOperations(context, game_path);
    for (auto& operation : operations) {
        operation.Apply(doc);
    }

    XmlAutoSerializer::fix(doc.get(), params.patchPath.stem());

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    spdlog::debug("Time: {}ms {} ({}:{})", duration, "Group", context->GetGenericPath(), 0);
    std::cout << fmt::format("ModOp time: {:.3f}s", duration / 1000.0f) << std::endl;
    return doc;
}

int command_diff(const XmltestParameters& params, const std::string& patch_content, std::ostream& out) {
    spdlog::debug("Target: {}", params.targetPath.string());
    spdlog::debug("Patch: {}", params.patchPath.string());
    for (auto& path : params.modPaths) {
        spdlog::debug("Mod path: {}", path.string());
    }

    auto doc = _get_prepatched(params);
    doc = _patch(doc, params, patch_content);
    auto prepatched_doc = _get_prepatched(params, true);

    const auto inner_extension = params.patchPath.stem().extension();
    if (inner_extension == L".cfg" || inner_extension == L".fc") {
        const std::string open_mode = "<Config>";
        const std::string close_mode = "</Config>";

        out << "##annodiff##";
        doc->print(out, "  ");

        out << "##annodiff##";
        prepatched_doc->print(out, "  ");
    }
    else {
        AnnoXml original(*prepatched_doc);
        AnnoXml patched(*doc);

        const std::string open_mode = "<" + patched.get_mode() + "s>";
        const std::string close_mode = "</" + patched.get_mode() + "s>";

        out << "##annodiff##";
        out << open_mode << std::endl << original.diff(patched) << close_mode << std::endl;

        out << "##annodiff##";
        out << open_mode << std::endl << original.get_touched() << close_mode << std::endl;
    }

    return 0;
}

int command_patch(const XmltestParameters& params, const std::string& patch_content) {
    spdlog::debug("Target: {}", params.targetPath.string());
    spdlog::debug("Patch: {}", params.patchPath.string());
    for (auto& path : params.modPaths) {
        spdlog::debug("Mod path: {}", path.string());
    }

    auto doc = _get_prepatched(params);
    doc = _patch(doc, params, patch_content);

    if (!params.skipOutput) {
        if (!XmlAutoSerializer::write(doc.get(), params.outputFile, true)) {
            printf("Could not open file for writing\n");
        }
    }

    return 0;
}

int main(int argc, const char **argv)
{
    XmltestParameters params;
    if (!parseArguments(argc, argv, params)) {
        return -1;
    }

    std::string patch_content;
    if (params.useStdin) {
        for (std::string line; std::getline(std::cin, line); ) {
            patch_content += line + "\n";
        }
    }

    spdlog::set_level(params.verbose ? spdlog::level::debug : spdlog::level::info);

    if (params.command == XmltestParameters::Command::Show) {
        return command_show(params, std::cout);
    }
    else if (params.command == XmltestParameters::Command::Diff) {
        return command_diff(params, patch_content, std::cout);
    }
    else {
        return command_patch(params, patch_content);
    }

    return 0;
}
