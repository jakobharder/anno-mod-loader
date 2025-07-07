
#include <fstream>
#include <map>
#include <memory>
#include <filesystem>

namespace pugi {
    class xml_document;
    class xml_node;
}

namespace xmlops {

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

    template<class T> T read() {
        T result;
        _stream.read(reinterpret_cast<char*>(&result), sizeof(result));
        return result;
    }

    bool read_data();
    bool read_names(int offset);
    void read_table(int offset);
    void construct_xml(pugi::xml_node* xml_root, Node* db_node);

    std::istream& _stream;
    size_t _size;
    Node _root;
    std::map<int32_t, std::string> _names;
};

class FileDbWriter {
public:
    static void write(const pugi::xml_document* xml_doc, const std::filesystem::path& file_path);
    static void write(const pugi::xml_document* xml_doc, std::ostream& stream, const std::filesystem::path& file_name);

    static void fix_counts(pugi::xml_document* xml_doc);

private:
    FileDbWriter(std::ostream& stream) : _stream(stream) {};

    template<class T> void write(T data) {
        _stream.write(reinterpret_cast<char*>(&data), sizeof(data));
    }

    void write_data(pugi::xml_node root);
    void write_data(pugi::xml_node root, int32_t& node_id, int32_t& attrib_id);
    int write_table(std::map<int32_t, std::string>& names);

    void write_remainder(size_t length);

    std::ostream& _stream;
    int _node_count;
    std::map<std::string, int32_t> _tag_names;
    std::map<std::string, int32_t> _attrib_names;

    std::map<int32_t, std::string> _tag_order;
    std::map<int32_t, std::string> _attrib_order;
};

}