
#include <memory>
#include <filesystem>

namespace pugi {
    class xml_document;
    class xml_node;
}

namespace xmlops {

class XmlAutoSerializer {
public:
    /// @brief Detect format from file name.
    [[nodiscard]] static std::shared_ptr<pugi::xml_document> read(const std::filesystem::path& file_path);

    /// @brief Detect format from file name.
    [[nodiscard]] static std::shared_ptr<pugi::xml_document> read(const void* data, size_t size, const std::filesystem::path& file_name);

    /// @brief Fixes InfoTip counts, world map ids, and so on.
    static void fix(pugi::xml_document* doc, const std::filesystem::path& file_path);

    /// @brief Detect format from file name. Includes fix() to correct common count, id issues.
    [[nodiscard]] static bool write(pugi::xml_document* doc, const std::filesystem::path& file_path, bool format = false);

    /// @brief Detect format from file name. Includes fix() to correct common count, id issues.
    static void write(pugi::xml_document* doc, std::ostream& stream, const std::filesystem::path& file_name, bool format = false);
};

}