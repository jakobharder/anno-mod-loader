#include <array>
#include <charconv>
#include <codecvt>
#include <functional>
#include <fstream>
#include <iostream>
#include <locale>
#include <map>
#include <memory>
#include <set>
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

enum FileDbAttributeType { Boolean, Int32, Float, Utf8, Utf16, Hex };

using EnumeratorInfo = std::vector<std::string>;
using ElementWithParent = std::pair<std::string_view, std::string_view>;

const EnumeratorInfo VISIBILITY_OPERATORS = {
    "And", "Or"
};

const EnumeratorInfo VISIBILITY_RESULT_TYPES = {
    "Bool", "Int", "Float", "Dataset"
};

const EnumeratorInfo VISIBILITY_COMPARE_OPERATORS = {
    "Equal", "Not", "Smaller", "SmallerOrEqual", "GreaterOrEqual", "Greater"
};

const EnumeratorInfo VISIBILITY_ELEMENT_TYPE = {
    "None", "Condition", "Group"
};

class FileDbConverter {
public:
    static FileDbAttributeType get_converter(const std::string_view name) {
        const auto& conv = converter.find(name);
        if (conv == converter.end()) {
            return default_converter;
        }
        return conv->second;
    }

    static const EnumeratorInfo* get_enumerator(const ElementWithParent& element) {
        const auto& enumInfo = enumerator.find(element);
        if (enumInfo == enumerator.end()) {
            return nullptr;
        }
        return &enumInfo->second;
    }

    static bool is_nested(const std::string_view name, const std::string_view parent) {
        return nester.end() != nester.find(ElementWithParent{name, parent});
    }

    static std::string read(const std::vector<char>& buffer, const std::string_view name, const std::string_view parent) {
        return read(buffer, get_converter(name), get_enumerator({name, parent}));
    }

    static std::string read(const std::vector<char>& buffer, FileDbAttributeType converter, const EnumeratorInfo* enumerator) {
        if (enumerator) {
            int32_t number = *reinterpret_cast<const int32_t*>(buffer.data());
            if (number < 0 || number >= enumerator->size()) {
                number = 0;
            }
            return enumerator->at(number);
        }

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

    static void write(const char* data, const ElementWithParent element, std::vector<char>& buffer) {
        write(data, buffer, get_converter(element.first), get_enumerator(element));
    }

    static void write(const std::string& value, std::vector<char>& buffer,
        FileDbAttributeType converter, const EnumeratorInfo* enumerator) {

        if (enumerator) {

            int index = 0; // default is 0
            for (int i = 0; i < enumerator->size(); i++) {
                if (value.compare(enumerator->at(i)) == 0) {
                    index = i;
                }
            }

            buffer.resize(4);
            *reinterpret_cast<int32_t*>(buffer.data()) = index;
            return;
        }

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

    static bool construct_xml(const std::string_view name, pugi::xml_node* xml_root, const std::vector<char>& content) {
        if (name.compare("ElementType") == 0) {
            xml_root->append_attribute("Type").set_value(
                FileDbConverter::read(content, name, xml_root->name()).c_str()
            );
            return true;
        }
        else if (name.compare("ChildCount") == 0 ||
            name.compare("InfoTipCount") == 0 ||
            name.compare("TemplateCount") == 0) {
            // skip counters
            return true;
        }

        return false;
    }

    static void setup(const fs::path& file_name) {
        converter.clear();
        enumerator.clear();
        nester.clear();
        if (file_name.filename() == L"export.bin") {
            default_converter = FileDbAttributeType::Int32;
            converter.emplace("Text", FileDbAttributeType::Utf8);
            converter.emplace("Condition", FileDbAttributeType::Utf8);
            converter.emplace("ValueText", FileDbAttributeType::Utf8);
            converter.emplace("IconText", FileDbAttributeType::Utf8);
            converter.emplace("ExpectedValueFloat", FileDbAttributeType::Float);
            converter.emplace("ExpectedValueBool", FileDbAttributeType::Boolean);
            converter.emplace("HeadlineText", FileDbAttributeType::Utf8);
            converter.emplace("SublineText", FileDbAttributeType::Utf8);
            converter.emplace("ColorText", FileDbAttributeType::Utf8);
            converter.emplace("Source", FileDbAttributeType::Utf8);
            converter.emplace("Name", FileDbAttributeType::Utf8);
            converter.emplace("IsTemplate", FileDbAttributeType::Boolean);
            converter.emplace("IsWarning", FileDbAttributeType::Boolean);
            converter.emplace("Indentation", FileDbAttributeType::Boolean);

            enumerator.emplace(ElementWithParent{"OperatorType", "VisibilityElement"}, VISIBILITY_OPERATORS);
            enumerator.emplace(ElementWithParent{"ResultType", "VisibilityElement"}, VISIBILITY_RESULT_TYPES);
            enumerator.emplace(ElementWithParent{"CompareOperator", "VisibilityElement"}, VISIBILITY_COMPARE_OPERATORS);
            enumerator.emplace(ElementWithParent{"ElementType", "VisibilityElement"}, VISIBILITY_ELEMENT_TYPE);

            nester.emplace(ElementWithParent{"ElementType", "VisibilityElement"});
            nester.emplace(ElementWithParent{"OperatorType", "VisibilityElement"});
            nester.emplace(ElementWithParent{"ResultType", "VisibilityElement"});
            nester.emplace(ElementWithParent{"CompareOperator", "VisibilityElement"});
        }
        else {
            default_converter = FileDbAttributeType::Hex;
        }
    }

private:
    static FileDbAttributeType default_converter;
    static std::map<std::string_view, FileDbAttributeType> converter;
    static std::map<ElementWithParent, EnumeratorInfo> enumerator;
    static std::set<ElementWithParent> nester;

    // structural conversions
public:
    template<typename WriteAction>
    static void attributes_and_counts(pugi::xml_node node, WriteAction write_action) {
        const pugi::xml_attribute attrib = node.attribute("Type");
        if (attrib) {
            std::vector<char> buffer;
            FileDbConverter::write(attrib.value(), ElementWithParent{"ElementType", node.name()}, buffer);
            write_action("ElementType", buffer);
        }
        node.remove_attributes();

        FileDbConverter counter{node.name()};
        for (auto& child : node.children()) {
            counter.check(child);
        }
        counter._count_elements(write_action);
    }

    FileDbConverter(const std::string_view name) {
        if (0 == name.compare("InfoElement")) {
            _counter_name = "ChildCount";
        }
        else if (0 == name.compare("InfoTips")) {
            _counter_name = "InfoTipCount";
            _secondary_name = "TemplateCount";
        }
        _counter = 0;
        _secondary = 0;
    }

    void check(const pugi::xml_node& node) {
        if (_counter_name.empty()) {
            return;
        }

        const auto& name = node.name();
        if (0 == std::strcmp("InfoElement", name)) {
            _counter++;
        }
        else if (0 == std::strcmp("InfoTipData", name)) {
            if (0 == std::strcmp("True", node.child_value("IsTemplate"))) {
                _secondary++;
            }
            else {
                _counter++;
            }
        }
    }

    template<typename WriteAction>
    void _count_elements(WriteAction write_action) {
        if (!_counter_name.empty()) {
            std::vector<char> buffer{sizeof(_counter)};
            std::memcpy(buffer.data(), &_counter, sizeof(_counter));
            write_action(_counter_name, buffer);
        }
        if (!_secondary_name.empty()) {
            std::vector<char> buffer{sizeof(_secondary)};
            std::memcpy(buffer.data(), &_secondary, sizeof(_secondary));
            write_action(_secondary_name, buffer);
        }
    }

private:
    std::string _counter_name;
    int _counter;
    std::string _secondary_name;
    int _secondary;
};

FileDbAttributeType FileDbConverter::default_converter;
std::map<std::string_view, FileDbAttributeType> FileDbConverter::converter;
std::map<ElementWithParent, EnumeratorInfo> FileDbConverter::enumerator;
std::set<ElementWithParent> FileDbConverter::nester;

std::shared_ptr<pugi::xml_document> FileDbReader::read(const void* data, size_t size, const fs::path& file_name) {
    std::istringstream memory { std::string(reinterpret_cast<const char*>(data), size) };
    return FileDbReader::read(memory, file_name);
}

std::shared_ptr<pugi::xml_document> FileDbReader::read(const fs::path& file_path) {
    std::ifstream file { file_path, std::ios::binary | std::ios::ate };
    return FileDbReader::read(file, file_path);
}

std::shared_ptr<pugi::xml_document> FileDbReader::read(std::istream& stream, const fs::path& file_name) {
    FileDbConverter::setup(file_name);
    FileDbReader reader{ stream };
    reader._stream.seekg(0, std::ios::end);
    reader._size = reader._stream.tellg();
    reader._stream.seekg(0, std::ios::beg);
    if (!reader._read_data() || !reader._read_names(OFFSET_TO_OFFSETS)) {
        std::cout << "error parsing dom" << std::endl;
        return {};
    }

    auto doc = std::make_shared<pugi::xml_document>();
    pugi::xml_node root = doc->append_child("InfoTips");
    reader._construct_xml(&root, &reader._root);

    return doc;
}

bool FileDbReader::_read_data() {
    int current_level = 0;
    Node* current_node = &_root;

    while (current_level >= 0 && current_node != nullptr)
    {
        int32_t content_size = _read<int32_t>();
        int32_t id = _read<int32_t>();

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

bool FileDbReader::_read_names(int offset) {
    _stream.seekg(_size - offset);

    int32_t tag_table = _read<int32_t>();
    int32_t attrib_table = _read<int32_t>();

    _read_table(tag_table);
    _read_table(attrib_table);

    return true;
}

void FileDbReader::_read_table(int offset)
{
    _stream.seekg(offset);

    int32_t count = _read<int32_t>();
    std::vector<uint16_t> ids;
    ids.resize(count);

    for (int i = 0; i < count; i++) {
        ids[i] = _read<uint16_t>();
    }
    for (int i = 0; i < count; i++) {
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

void FileDbReader::_construct_xml(pugi::xml_node* xml_root, Node* db_node) {
    for (auto& db_child : db_node->children) {
        auto name = _names[db_child.id];
        auto& content = db_child.content;

        // nested attribute
        if (FileDbConverter::is_nested(name, xml_root->name()) &&
            db_child.children.size() == 1 && name.compare(_names[db_child.children[0].id]) == 0
            && db_child.children[0].children.empty() && !db_child.children[0].content.empty()
        ) {
            content = db_child.children[0].content;
        }

        if (FileDbConverter::construct_xml(name, xml_root, content)) {
            // done
        }
        else if (!content.empty()) {
            auto xml_child = xml_root->append_child((name.empty() ? ANONYMOUS_NODE : name).c_str());
            xml_child.append_child(pugi::node_pcdata).set_value(
                FileDbConverter::read(content, name, xml_root->name()).c_str()
            );
        }
        else if (!db_child.children.empty()) {
            auto xml_child = xml_root->append_child((name.empty() ? ANONYMOUS_NODE : name).c_str());
            _construct_xml(&xml_child, &db_child);
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
    writer._write_root(doc->child("InfoTips"));
    int tag_offset = writer._write_table(writer._tag_order);
    int attrib_offset = writer._write_table(writer._attrib_order);

    { // v3
        writer._write<int32_t>(0, writer._node_count);
    }

    writer._write<int32_t>(tag_offset, attrib_offset);

    uint8_t magic[] = { 0x08, 0x00, 0x00, 0x00, 0xFD, 0xFF, 0xFF, 0xFF };
    writer._stream.write(reinterpret_cast<char*>(magic), sizeof(magic));

    writer._stream.flush();
}

void FileDbWriter::_set_value(pugi::xml_node node, const int number) {
    std::array<char, 12> buffer;
    auto [ptr, ec] = std::to_chars(buffer.data(), buffer.data() + buffer.size(), number);
    node.append_child(pugi::xml_node_type::node_pcdata).set_value(ptr);
}

void FileDbWriter::_write_root(pugi::xml_node root) {
    int32_t node_id = FIRST_TAG;
    int32_t attrib_id = FIRST_ATTRIB;
    _tag_names.emplace(ANONYMOUS_NODE, node_id);
    _attrib_names.emplace(ANONYMOUS_NODE, attrib_id);

    _node_count = 1;
    _write_node(root, node_id, attrib_id);
}

void FileDbWriter::_write_node(pugi::xml_node node, int32_t& node_id, int32_t& attrib_id) {
    FileDbConverter::attributes_and_counts(node, [this, &attrib_id](const std::string_view name, const std::vector<char>& buffer) {
        this->_write_attrib(std::string{ name }, buffer, attrib_id);
    });

    for (auto& child : node.children()) {
        _node_count++;

        auto has_children = child.begin() != child.end();
        if (has_children && child.first_child().type() == pugi::node_pcdata) {

            if (FileDbConverter::is_nested(child.name(), node.name())) {
                int32_t id = _get_id(child.name(), _tag_names, _tag_order, node_id);
                _write<int32_t>(0, id);
            }

            std::vector<char> buffer;
            FileDbConverter::write(child.child_value(), ElementWithParent{child.name(), node.name()}, buffer);
            _write_attrib(child.name(), buffer, attrib_id);
        }
        else {
            int32_t id = _get_id(child.name(), _tag_names, _tag_order, node_id);
            _write<int32_t>(0, id);
            _write_node(child, node_id, attrib_id);
        }
    }

    // close tag
    _write<int32_t>(0, 0);
}

int FileDbWriter::_write_table(std::map<int32_t, std::string>& names) {
    size_t offset = _stream.tellp();

    _write<int32_t>((int32_t)names.size());
    size_t written = sizeof(int32_t) + names.size() * sizeof(uint16_t);

    for (auto& entry : names) {
        _write<uint16_t>(entry.first);
    }
    for (auto& entry : names) {
        // write including zero
        _stream.write(entry.second.c_str(), entry.second.size() + 1);
        written += entry.second.size() + 1;
    }

    _write_remainder(written);
    return (int)offset;
}

int32_t FileDbWriter::_get_id(const std::string& name, IdMap& names, OrderMap& order, int32_t& current_id) {
    int32_t id;
    auto reuse_id = names.find(name);
    if (reuse_id != names.end()) {
        id = reuse_id->second;
    }
    else {
        names.emplace(name, id = ++current_id);
        order.emplace(id, name);
    }
    return id;
}

void FileDbWriter::_write_attrib(const std::string& name, const std::vector<char>& buffer, int32_t& attrib_id) {
    const int32_t id = _get_id(name, _attrib_names, _attrib_order, attrib_id);

    _write<int32_t>(static_cast<int32_t>(buffer.size()), id);

    _stream.write(buffer.data(), buffer.size());
    _write_remainder(buffer.size());
}

void FileDbWriter::_write_remainder(size_t size) {
    int unaligned_count = size % ATTRIB_BLOCK_SIZE;
    if (unaligned_count > 0) {
        for (int i = unaligned_count; i < ATTRIB_BLOCK_SIZE; i++) {
            _stream << (char)0;
        }
    }
}

}