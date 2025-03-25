
#include <fstream>
#include <memory>
#include <filesystem>

namespace pugi {
    class xml_document;
}

namespace xmlops {

class FcReader {
public:
    [[nodiscard]] static std::shared_ptr<pugi::xml_document> read(const void* data, size_t size, const std::filesystem::path& file_name);
    [[nodiscard]] static std::shared_ptr<pugi::xml_document> read(const std::filesystem::path& file_path);
    [[nodiscard]] static std::shared_ptr<pugi::xml_document> read(std::istream& stream, const std::filesystem::path& file_name);

private:
    FcReader(std::istream& stream) : _stream(stream) {};

    template<class T> [[nodiscard]] T read() {
        T result;
        _stream.read(reinterpret_cast<char*>(&result), sizeof(result));
        return result;
    }

    std::istream& _stream;
    size_t _size;
};

class FcWriter {
public:
    static void write(const pugi::xml_document* xml_doc, const std::filesystem::path& file_path);
    static void write(const pugi::xml_document* xml_doc, std::ostream& stream, const std::filesystem::path& file_name);

    static void fix_ids(pugi::xml_document* xml_doc);

private:
    FcWriter(std::ostream& stream) : _stream(stream) {};

    template<class T> void write(T data) {
        _stream.write(reinterpret_cast<char*>(&data), sizeof(data));
    }

    std::ostream& _stream;
};

}