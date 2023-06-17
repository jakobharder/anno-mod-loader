#include <charconv>
#include <codecvt>
#include <functional>
#include <fstream>
#include <iostream>
#include <locale>
#include <map>
#include <memory>
#include <sstream>
#include <stack>
#include <string_view>

#include <spdlog/fmt/fmt.h>
#include <pugixml.hpp>

namespace fs = std::filesystem;

#include "xml_filedb_reader.h"

namespace xmlops {

const int OFFSET_TO_OFFSETS = 16;
const int ATTRIB_BLOCK_SIZE = 8;

const int FIRST_TAG = 1;
const int FIRST_ATTRIB = 32768;
const std::string ANONYMOUS_NODE = "None";

enum FileDbAttributeType {
    Boolean,
    Int32,
    Float,
    Utf8,
    Utf16,
    Hex
};

class FileDbConverter {
public:
    static FileDbAttributeType get_converter(const std::string& name) {
        const auto& conv = converter.find(name);
        if (conv == converter.end()) {
            return default_converter;
        }
        return conv->second;
    }

    static std::string read(const std::vector<char>& buffer, const std::string& name) {
        return read(buffer, get_converter(name));
    }

    static std::string read(const std::vector<char>& buffer, FileDbAttributeType converter) {
        switch (converter) {
        case FileDbAttributeType::Boolean: {
            int32_t number = buffer.size() == 4 ?
                *reinterpret_cast<const int32_t*>(buffer.data()) :
                *reinterpret_cast<const char*>(buffer.data());
            return number != 0 ? "True" : "False";
        }
        case FileDbAttributeType::Int32: {
            int32_t number = *reinterpret_cast<const int32_t*>(buffer.data());
            return fmt::format("{}", number).c_str();
        }
        case FileDbAttributeType::Float: {
            float number = *reinterpret_cast<const float*>(buffer.data());
            return fmt::format("{}", number).c_str();
        }
        case FileDbAttributeType::Utf8: {
            std::stringstream ss;
            ss.write(buffer.data(), buffer.size());
            return ss.str();
        }
        case FileDbAttributeType::Utf16: {
            std::wstringstream ss;
            ss.write(reinterpret_cast<const wchar_t*>(buffer.data()), buffer.size() / 2);
            std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
            return converter.to_bytes(ss.str());
        }
        case FileDbAttributeType::Hex: {
            std::stringstream ss;
            for (char ch : buffer) {
                ss << fmt::format("{:X}", ch);
            }
            return ss.str();
        }
        default:
            return "";
        }
    }

    static void write(const char* data, const std::string& name, std::vector<char>& buffer) {
        write(data, buffer, get_converter(name));
    }

    static void write(const std::string& value, std::vector<char>& buffer, FileDbAttributeType converter) {
        switch (converter) {
        case FileDbAttributeType::Boolean: {
            buffer.resize(1);
            std::string out;
            std::transform(value.begin(), value.end(), std::back_inserter(out), [](unsigned char c){ return std::tolower(c); });
            if (out == "true" || out == "1") {
                *reinterpret_cast<char*>(buffer.data()) = 1;
            }
            else {
                *reinterpret_cast<char*>(buffer.data()) = 0;
            }
            break;
        }
        case FileDbAttributeType::Int32: {
            buffer.resize(4);
            *reinterpret_cast<int32_t*>(buffer.data()) = 0;
            std::from_chars(value.data(), value.data() + value.size(), *reinterpret_cast<int32_t*>(buffer.data()));
            break;
        }
        case FileDbAttributeType::Float: {
            buffer.resize(4);
            *reinterpret_cast<float*>(buffer.data()) = 0;
            std::from_chars(value.data(), value.data() + value.size(), *reinterpret_cast<float*>(buffer.data()));
            break;
        }
        case FileDbAttributeType::Utf8: {
            buffer.resize(value.size());
            value.copy(buffer.data(), value.size(), 0);
            break;
        }
        case FileDbAttributeType::Utf16: {
            std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
            std::wstring wide = converter.from_bytes(value);
            buffer.resize(wide.size() * sizeof(wchar_t));
            wide.copy(reinterpret_cast<wchar_t*>(buffer.data()), wide.size());
            break;
        }
        case FileDbAttributeType::Hex: {
            // std::stringstream ss;
            // for (char ch : buffer) {
            //     ss << fmt::format("{:X}", ch);
            // }
            // return ss.str();
            break;
        }
        default:
            break;
        }
    }

    static void setup(const fs::path& file_name) {
        converter.clear();
        if (file_name.filename() == L"export.bin") {
            default_converter = FileDbAttributeType::Int32;
            converter.emplace("Text", FileDbAttributeType::Utf16);
            converter.emplace("Condition", FileDbAttributeType::Utf8);
            converter.emplace("ValueText", FileDbAttributeType::Utf16);
            converter.emplace("IconText", FileDbAttributeType::Utf16);
            converter.emplace("ExpectedValueFloat", FileDbAttributeType::Float);
            converter.emplace("ExpectedValueBool", FileDbAttributeType::Boolean);
            converter.emplace("HeadlineText", FileDbAttributeType::Utf16);
            converter.emplace("SublineText", FileDbAttributeType::Utf16);
            converter.emplace("ColorText", FileDbAttributeType::Utf16);
            converter.emplace("Source", FileDbAttributeType::Utf16);
            converter.emplace("Name", FileDbAttributeType::Utf16);
            converter.emplace("IsTemplate", FileDbAttributeType::Boolean);
        }
        else {
            default_converter = FileDbAttributeType::Hex;
        }
    }

private:
    static FileDbAttributeType default_converter;
    static std::map<std::string, FileDbAttributeType> converter;
};

FileDbAttributeType FileDbConverter::default_converter;
std::map<std::string, FileDbAttributeType> FileDbConverter::converter;

std::shared_ptr<pugi::xml_document> FileDbReader::read(const void* data, size_t size, const fs::path& file_name) {
    std::istringstream memory { std::string(reinterpret_cast<const char*>(data), size) };
    return FileDbReader::read((std::istream&)memory, file_name);
}

std::shared_ptr<pugi::xml_document> FileDbReader::read(const fs::path& file_path) {
    std::ifstream file { file_path, std::ios::binary | std::ios::ate };
    return FileDbReader::read((std::istream&)file, file_path);
}

std::shared_ptr<pugi::xml_document> FileDbReader::read(std::istream& stream, const fs::path& file_name) {
    FileDbConverter::setup(file_name);
    FileDbReader reader{ stream };
    reader._stream.seekg(0, std::ios::end);
    reader._size = reader._stream.tellg();
    reader._stream.seekg(0, std::ios::beg);
    if (!reader.read_data() || !reader.read_names(OFFSET_TO_OFFSETS)) {
        std::cout << "error parsing dom" << std::endl;
        return {};
    }

    auto doc = std::make_shared<pugi::xml_document>();
    pugi::xml_node root = doc->append_child("Content");
    reader.construct_xml(&root, &reader._root);

    return doc;
}

bool FileDbReader::read_data() {
    int current_level = 0;
    Node* current_node = &_root;

    while (current_level >= 0 && current_node != nullptr)
    {
        int32_t content_size = read<int32_t>();
        int32_t id = read<int32_t>();

        bool terminator = id <= 0;
        bool attrib = id >= FIRST_ATTRIB;

        if (content_size > _size - _stream.tellg()) {
            return false;
        }

        if (terminator) {
            current_level--;
            current_node = current_node->parent;
        }
        else {
            current_node->children.emplace_back(id, current_node);
            Node& node = current_node->children.back();

            if (attrib) {
                node.content.resize(content_size);
                _stream.read(node.content.data(), content_size);
                int unaligned_count = content_size % ATTRIB_BLOCK_SIZE;
                if (unaligned_count > 0) {
                    _stream.seekg(ATTRIB_BLOCK_SIZE - unaligned_count, std::ios_base::cur);
                }
            }
            else {
                current_node = &node;
                current_level++;
            }
        }
    }

    return true;
}

bool FileDbReader::read_names(int offset) {
    _stream.seekg(_size - offset);

    int32_t tag_table = read<int32_t>();
    int32_t attrib_table = read<int32_t>();

    read_table(tag_table);
    read_table(attrib_table);

    return true;
}

void FileDbReader::read_table(int offset)
{
    _stream.seekg(offset);

    int32_t count = read<int32_t>();
    std::vector<uint16_t> ids;
    ids.resize(count);

    for (int i = 0; i < count; i++) {
        ids[i] = read<uint16_t>();
    }
    for (int i = 0; i < count; i++)
    {
        std::stringstream name_stream;
        char ch;
        do {
            _stream.read(&ch, 1);
            if (ch != 0) {
                name_stream << ch;
            }
        } while (ch != 0);
        _names.emplace(ids[i], name_stream.str());
    }
}

void FileDbReader::construct_xml(pugi::xml_node* xml_root, Node* db_node) {
    for (auto& db_child : db_node->children) {
        auto name = _names[db_child.id];
        auto xml_child = xml_root->append_child((name.empty() ? ANONYMOUS_NODE : name).c_str());
        if (!db_child.content.empty()) {
            xml_child.append_child(pugi::node_pcdata).set_value(
                FileDbConverter::read(db_child.content, name).c_str()
            );
        }
        else if (!db_child.children.empty()) {
            construct_xml(&xml_child, &db_child);
        }
    }
}

void FileDbWriter::write(const pugi::xml_document* doc, const fs::path& file_path) {
    std::ofstream file { file_path, std::ios::binary | std::fstream::out };
    write(doc, (std::ostream&)file, file_path);
}

void FileDbWriter::write(const pugi::xml_document* doc, std::ostream& stream, const std::filesystem::path& file_name) {
    FileDbConverter::setup(file_name);

    FileDbWriter writer{ stream };
    writer.write_data(doc->child("Content"));
    int tag_offset = writer.write_table(writer._tag_order);
    int attrib_offset = writer.write_table(writer._attrib_order);

    { // v3
        writer.write<int32_t>(0);
        writer.write<int32_t>(writer._node_count);
    }

    writer.write<int32_t>(tag_offset);
    writer.write<int32_t>(attrib_offset);

    uint8_t magic[] = { 0x08, 0x00, 0x00, 0x00, 0xFD, 0xFF, 0xFF, 0xFF };
    writer._stream.write(reinterpret_cast<char*>(magic), sizeof(magic));

    writer._stream.flush();
}

void FileDbWriter::fix_counts(pugi::xml_document* doc) {
    int infotips = 0;
    int templates = 0;

    for (auto node : doc->select_nodes("/Content/InfoTipData")) {
        infotips++;
    }
    for (auto node : doc->select_nodes("/Content/InfoTipData[IsTemplate='True']")) {
        templates++;
        infotips--;
    }

    doc->child("Content").child("InfoTipCount").first_child().set_value(fmt::format("{:d}", infotips).c_str());
    doc->child("Content").child("TemplateCount").first_child().set_value(fmt::format("{:d}", templates).c_str());
}

void FileDbWriter::write_data(pugi::xml_node root) {

    int32_t node_id = FIRST_TAG;
    int32_t attrib_id = FIRST_ATTRIB;
    _tag_names.emplace(ANONYMOUS_NODE, node_id);
    _attrib_names.emplace(ANONYMOUS_NODE, attrib_id);

    _node_count = 1;
    write_data(root, node_id, attrib_id);
}

void FileDbWriter::write_data(pugi::xml_node node, int32_t& node_id, int32_t& attrib_id) {
    for (auto& child : node.children()) {
        _node_count++;

        auto has_children = child.begin() != child.end();
        if (has_children && child.first_child().type() == pugi::node_pcdata) {
            std::vector<char> buffer;
            FileDbConverter::write(child.child_value(), child.name(), buffer);

            int32_t id;
            auto reuse_id = _attrib_names.find(child.name());
            if (reuse_id != _attrib_names.end()) {
                id = reuse_id->second;
            }
            else {
                _attrib_names.emplace(child.name(), id = ++attrib_id);
                _attrib_order.emplace(id, child.name());
            }

            write<int32_t>((int32_t)buffer.size());
            write<int32_t>(id);

            _stream.write(buffer.data(), buffer.size());
            write_remainder(buffer.size());
        }
        else /*if (has_children)*/ {
            int32_t id;
            auto reuse_id = _tag_names.find(child.name());
            if (reuse_id != _tag_names.end()) {
                id = reuse_id->second;
            }
            else {
                _tag_names.emplace(child.name(), id = ++node_id);
                _tag_order.emplace(id, child.name());
            }

            write<int32_t>(0);
            write<int32_t>(id);
            write_data(child, node_id, attrib_id);
        }
    }

    // close tag
    write<int32_t>(0);
    write<int32_t>(0);
}

int FileDbWriter::write_table(std::map<int32_t, std::string>& names) {
    size_t offset = _stream.tellp();

    write<int32_t>((int32_t)names.size());
    size_t written = sizeof(int32_t) + names.size() * sizeof(uint16_t);

    for (auto& entry : names) {
        write<uint16_t>(entry.first);
    }
    for (auto& entry : names) {
        // write including zero
        _stream.write(entry.second.c_str(), entry.second.size() + 1);
        written += entry.second.size() + 1;
    }

    write_remainder(written);
    return (int)offset;
}

void FileDbWriter::write_remainder(size_t size) {
    int unaligned_count = size % ATTRIB_BLOCK_SIZE;
    if (unaligned_count > 0) {
        for (int i = unaligned_count; i < ATTRIB_BLOCK_SIZE; i++) {
            _stream << (char)0;
        }
    }
}

}