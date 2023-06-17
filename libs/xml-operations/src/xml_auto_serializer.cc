
#include <fstream>

#include <pugixml.hpp>

namespace fs = std::filesystem;

#include "xml_auto_serializer.h"
#include "xml_filedb_reader.h"
#include "xml_fc_reader.h"

namespace xmlops {

std::shared_ptr<pugi::xml_document> XmlAutoSerializer::read(const fs::path& file_path) {
    std::ifstream file{ file_path, std::ios::binary | std::ios::ate };
    std::streampos fileSize = file.tellg();
    file.seekg(0, std::ios::beg);
    std::vector<char> buffer(fileSize);
    if (!file.read(buffer.data(), fileSize)) {
      return {};
    }

    return read(buffer.data(), fileSize, file_path);
}

std::shared_ptr<pugi::xml_document> XmlAutoSerializer::read(const void* data, size_t size, const fs::path& file_name) {
    if (file_name.filename() == "export.bin") {
      return xmlops::FileDbReader::read(data, size, file_name);
    }
    else if (file_name.extension() == ".fc") {
        return xmlops::FcReader::read(data, size, file_name);
    }
    else {
        auto doc = std::make_shared<pugi::xml_document>();
        doc->load_buffer(data, size);
        return doc;
    }
}

void XmlAutoSerializer::fix(pugi::xml_document* doc, const fs::path& file_name) {
    if (file_name.filename() == "export.bin") {
        FileDbWriter::fix_counts(doc);
    }
    else if (file_name.extension() == ".fc") {
        FcWriter::fix_ids(doc);
    }
    else {
    }
}

void XmlAutoSerializer::write(pugi::xml_document* doc, const fs::path& file_path, bool format) {
    std::ofstream file { file_path, std::ios::binary | std::fstream::out };
    write(doc, (std::ostream&)file, file_path, format);
}

void XmlAutoSerializer::write(pugi::xml_document* doc, std::ostream& stream, const std::filesystem::path& file_name, bool format) {
    if (file_name.filename() == "export.bin") {
        FileDbWriter::fix_counts(doc);
        return FileDbWriter::write(doc, stream, file_name);
    }
    else if (file_name.extension() == ".fc") {
        return FcWriter::write(doc, stream, file_name);
    }
    else {
        doc->print(stream, format ? "  " : "", format ? pugi::format_default : pugi::format_raw);
    }
}

}