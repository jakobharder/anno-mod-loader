
#include <filesystem>
#include <map>
#include <string>

#include "pugixml.hpp"

class AnnoXml {
public:
    AnnoXml(const std::filesystem::path& file_path);

    AnnoXml(const pugi::xml_document& doc);

    [[nodiscard]] std::string get(const std::string& guid) const;

    [[nodiscard]] std::string diff(const AnnoXml& patched);

    [[nodiscard]] std::string get_touched() const;

    [[nodiscard]] std::string get_mode() const {
        return mode_;
    }

private:
    std::map<std::string, std::string> assets_;
    std::vector<std::string> original_;
    std::string mode_;

    void parse_assets(pugi::xml_node node);
};

