
#include <fstream>
#include <map>
#include <memory>
#include <filesystem>

namespace pugi {
    class xml_document;
    class xml_node;
}

namespace xmlops {

using ElementWithParent = std::pair<std::string_view, std::string_view>;

class FileDbReader {
public:
    static std::shared_ptr<pugi::xml_document> read(const void* data, size_t size, const std::filesystem::path& file_name);
    static std::shared_ptr<pugi::xml_document> read(const std::filesystem::path& file_path);
    static std::shared_ptr<pugi::xml_document> read(std::istream& stream, const std::filesystem::path& file_name);

private:
    FileDbReader(std::istream& stream) : _stream(stream) {};

    struct Node {
        int32_t id = 0;
        Node* parent = nullptr;
        std::vector<Node> children;
        std::vector<char> content;

        Node() {}
        Node(int32_t id, Node* parent) {
            this->id = id;
            this->parent = parent;
        }
    };

    template<class T> T _read() {
        T result;
        _stream.read(reinterpret_cast<char*>(&result), sizeof(result));
        return result;
    }

    [[nodiscard]] bool _read_data();
    [[nodiscard]] bool _read_names(int offset);
    void _read_table(int offset);
    void _construct_xml(pugi::xml_node* xml_root, Node* db_node);

    std::istream& _stream;
    size_t _size;
    Node _root;
    std::map<int32_t, std::string> _names;
};

class FileDbWriter {
public:
    static void write(const pugi::xml_document* xml_doc, const std::filesystem::path& file_path);
    static void write(const pugi::xml_document* xml_doc, std::ostream& stream, const std::filesystem::path& file_name);

private:
    using IdMap = std::map<std::string, int32_t, std::less<>>;
    using OrderMap = std::map<int32_t, std::string>;

    FileDbWriter(std::ostream& stream) : _stream(stream) {};

    template<typename ...T> void _write(T... data) {
        (..., _stream.write(reinterpret_cast<char*>(&data), sizeof(data)));
    };

    static void _set_value(pugi::xml_node node, const int number);

    void _write_root(pugi::xml_node root);
    void _write_node(pugi::xml_node node, int32_t& node_id, int32_t& attrib_id);

    [[nodiscard]] int _write_table(std::map<int32_t, std::string>& names);
    [[nodiscard]] int32_t _get_id(const std::string_view name, IdMap& names, OrderMap& order, int32_t& current_id);
    void _write_attrib(const ElementWithParent element, const std::vector<char>& buffer, int32_t& attrib_id, int32_t& node_id);
    void _write_remainder(size_t length);

    std::ostream& _stream;
    int _node_count;
    IdMap _tag_names;
    IdMap _attrib_names;

    OrderMap _tag_order;
    OrderMap _attrib_order;
};

}