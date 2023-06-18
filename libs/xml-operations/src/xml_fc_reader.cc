#include <charconv>
#include <codecvt>
#include <functional>
#include <fstream>
#include <iostream>
#include <locale>
#include <map>
#include <memory>
#include <sstream>

#include <spdlog/fmt/fmt.h>
#include <pugixml.hpp>

namespace fs = std::filesystem;

#include "xml_fc_reader.h"

namespace xmlops {

std::shared_ptr<pugi::xml_document> FcReader::read(const void* data, size_t size, const fs::path& file_name) {
    std::istringstream memory { std::string(reinterpret_cast<const char*>(data), size) };
    return FcReader::read((std::istream&)memory, file_name);
}

std::shared_ptr<pugi::xml_document> FcReader::read(const fs::path& file_path) {
    std::ifstream file { file_path, std::ios::binary | std::ios::ate };
    return FcReader::read((std::istream&)file, file_path);
}

std::shared_ptr<pugi::xml_document> FcReader::read(std::istream& stream, const fs::path &file_path) {
    FcReader reader { stream };
    reader._stream.seekg(0, std::ios::end);
    reader._size = reader._stream.tellg();
    reader._stream.seekg(0, std::ios::beg);

    std::stringstream true_xml;
    std::stringstream cdata;

    char ch;
    while (reader._stream >> ch) {
        true_xml << ch;
        cdata << ch;
        if (ch == '>') {
            cdata.str(std::string());
        }
        if (ch == '[' && cdata.str() == "CDATA[") {
            int32_t count = reader.read<int32_t>();
            for (int i = 0; i < count / 4; i++) {
                true_xml << " " << fmt::format("{:d}", reader.read<int32_t>());
            }
        }
    }

    auto doc = std::make_shared<pugi::xml_document>();
    doc->load_string(true_xml.str().c_str());
    return doc;
}

void FcWriter::write(const pugi::xml_document* doc, const fs::path& file_path) {
    std::ofstream file { file_path, std::ios::binary | std::fstream::out };
    write(doc, (std::ostream&)file, file_path);
}

void FcWriter::write(const pugi::xml_document* doc, std::ostream& stream, const std::filesystem::path& file_name) {
    FcWriter writer{ stream };

    std::stringstream xml;
    doc->print(xml, "  ");

    xml.seekg(0, std::ios::beg);

    std::stringstream cdata;

    char ch;
    while (xml >> ch) {
        writer._stream << ch;
        cdata << ch;
        if (ch == '>') {
            cdata.str(std::string());
        }
        if (ch == '[' && cdata.str() == "CDATA[") {
            int32_t number;
            std::vector<int32_t> numbers;
            while (xml >> number) {
                numbers.push_back(number);
                if (xml.peek() == ']')
                    break;
            }

            writer.write<int32_t>((int32_t)(numbers.size() * sizeof(int32_t)));
            for (int32_t n : numbers) {
                writer.write<int32_t>(n);
            }
        }
    }

    writer._stream.flush();
}

void FcWriter::fix_ids(pugi::xml_document* doc) {
    std::string id_counter = doc->child("IdCounter").child_value();
    int next_id = 0;
    std::from_chars(id_counter.data(), id_counter.data() + id_counter.size(), next_id);

    for (auto node : doc->select_nodes("//Id[text()='auto']")) {
        next_id++;
        node.node().first_child().set_value(fmt::format("{:d}", next_id).c_str());
    }

    doc->child("IdCounter").first_child().set_value(fmt::format("{:d}", next_id).c_str());
}

}