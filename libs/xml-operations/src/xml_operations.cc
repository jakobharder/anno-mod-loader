#include "xml_operations.h"

#include "spdlog/spdlog.h"
#include "str_helper.h"
#include "xml_pugi_helper.h"

#include <charconv>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <map>
#include <regex>
#include <string>

namespace xmlops {

XmlOperationContext::XmlOperationContext(const fs::path& mod_relative_path,
                                         const fs::path& mod_base_path,
                                         std::string_view mod_id,
                                         const std::map<std::string, std::string>* variables,
                                         std::string_view mod_name) : mod_id_(mod_id)
{
    mod_name_ = mod_name.empty() ? mod_base_path.filename().string() : mod_name;
    mod_base_path_ = mod_base_path;
    variables_ = variables;

    include_loader_ = [this, &mod_base_path](const fs::path& file_path) {
        std::vector<char> buffer;
        size_t size;
        if (!ReadFile(mod_base_path / file_path, buffer, size)) {
            const auto file_path_str = file_path.string();
            spdlog::error("{}: Failed to open {}", this->mod_name_, file_path_str);
            return std::shared_ptr<XmlOperationContext>{};
        }
        return std::make_shared<XmlOperationContext>(buffer.data(), size, file_path,
            this->mod_id_, this->variables_,
            this->mod_name_, *this->include_loader_);
    };

    const auto loaded = (*include_loader_)(mod_relative_path);
    if (loaded)
    {
        doc_ = loaded->doc_;
        offset_data_ = loaded->offset_data_;
        doc_path_ = loaded->doc_path_;
    }
}

XmlOperationContext::XmlOperationContext(const char* buffer, size_t size,
                                         const fs::path& doc_path,
                                         std::string_view mod_id,
                                         const std::map<std::string, std::string>* variables,
                                         std::string_view mod_name,
                                         std::optional<include_loader_t> include_loader) : mod_id_(mod_id)
{
    mod_name_ = mod_name;
    variables_ = variables;

    include_loader_ = include_loader;
    doc_path_ = doc_path.generic_string();

    offset_data_ = BuildOffsetData(buffer, size);
    doc_ = std::make_shared<pugi::xml_document>();

    auto parse_result = doc_->load_buffer(buffer, size);
    if (!parse_result) {
        const auto line = this->GetLine(parse_result.offset);
        const auto desc = parse_result.description();
        spdlog::error("{}: Failed to parse: {} ({}:{})",
                      mod_name, desc, doc_path_, line);
    }
}

std::shared_ptr<XmlOperationContext> XmlOperationContext::OpenInclude(const fs::path& file_path) const
{
    if (!include_loader_) {
        return {};
    }

    auto include = (*include_loader_)(file_path);
    if (include) {
        include->mod_name_ = mod_name_;
        include->include_loader_ = this->include_loader_;
    }
    return include;
}

size_t XmlOperationContext::GetLine(ptrdiff_t offset) const
{
    auto it = std::lower_bound(offset_data_.begin(), offset_data_.end(), offset);
    return (it - offset_data_.begin()) + 1;
}

pugi::xml_node XmlOperationContext::GetRoot() const
{
    auto root = doc_ ? doc_->root() : pugi::xml_node{};
    if (!root) {
        Error("Failed to get root element");
        return {};
    }

    if (!root.first_child() || !pugih::equals(root.first_child(), "ModOps")) {
        Error("Doesn't contain ModOps root node");
        return {};
    }

    return root.first_child();
}

template<typename... Args>
void XmlOperationContext::Debug(std::string_view msg, const Args &... args) const
{
    spdlog::debug(msg, args...);
}

void XmlOperationContext::Debug(std::string_view msg, pugi::xml_node node) const
{
    const auto line = node ? GetLine(node) : 0;
    spdlog::debug("{}: {} ({}:{})", mod_name_, msg, doc_path_, line);
}

void XmlOperationContext::Warn(std::string_view msg, pugi::xml_node node) const
{
    const auto line = node ? GetLine(node) : 0;
    spdlog::warn("{}: {} ({}:{})", mod_name_, msg, doc_path_, line);
}

void XmlOperationContext::Error(std::string_view msg, pugi::xml_node node) const
{
    const auto line = node ? GetLine(node) : 0;
    spdlog::error("{}: {} ({}:{})", mod_name_, msg, doc_path_, line);
}

bool XmlOperationContext::ReadFile(const fs::path& file_path, std::vector<char>& buffer, size_t& size)
{
    std::ifstream ifs(file_path, std::ios::in | std::ios::ate);
    if (!ifs) {
        return false;
    }

    size = ifs.tellg();
    ifs.seekg(0, std::ios::beg);
    buffer.resize(size);
    ifs.read(buffer.data(), size);
    return true;
}

XmlOperationContext::offset_data_t XmlOperationContext::BuildOffsetData(const char* buffer, size_t size)
{
    offset_data_t result;
    for (size_t i = 0; i < size; ++i) {
        if (buffer[i] == '\n') {
            result.push_back(i);
        }
    }
    return result;
}

XmlLookup::XmlLookup() { }

XmlLookup::XmlLookup(const std::string& path,
                     const std::string& guid,
                     const std::string& property,
                     const std::string& templ,
                     pugi::xpath_variable_set* variables,
                     std::shared_ptr<XmlOperationContext> context,
                     pugi::xml_node node,
                     const bool skip_values,
                     const XmlPatchType patch_type)
{
    context_ = context;
    node_ = node;
    variables_ = variables;

    empty_path_ = path.empty();
    negative_ = !path.empty() && path[0] == '!';

    std::string read_path = negative_ ? path.substr(1) : path;

    if (!read_path.empty() && (read_path.front() == '$' || read_path.front() == '#')) {
        path_ = read_path.substr(0);
        ReplaceStaticVariables(path_);

        const auto var = read_path.substr(1);
        if (!variables_->get(var.c_str())) {
            variables_->set(var.c_str(), false);
        }
        return;
    }
    else if (!read_path.empty() && read_path[0]== '~') {
        // TODO deprecate
        read_path = read_path.substr(1);
        guid_ = guid;
        template_ = templ;
    }
    else {
        if (read_path.length() >= 2 &&
            ((read_path[0] == '/' && read_path[1] == '/') ||
            (read_path[0] == '@'))) {
            guid_ = {};
            template_ = {};
            property_ = {};
        }
        else {
            guid_ = guid;
            template_ = templ;
            property_ = property;
        }
    }

    ReplaceStaticVariables(read_path);
    ReadPath(read_path, guid_, property_, template_, skip_values, patch_type);
}

void XmlLookup::ReplaceStaticVariables(std::string& path)
{
    const auto variables = context_->GetVariables();
    if (!variables) {
        return;
    }

    const auto& mod_id = context_->GetModID();

    std::regex varRegex(R"([\$#][a-zA-Z_][\w\-\.]*)");
    std::smatch match;

    std::string result;
    std::string::const_iterator searchStart(path.cbegin());

    while (std::regex_search(searchStart, path.cend(), match, varRegex)) {
        result.append(searchStart, match[0].first);

        const auto match_str = match.str();
        const bool id_check = match_str.rfind("#", 0) == 0;
        const bool is_local = match_str.find('.') == match_str.npos;
        const auto full_name = (is_local && !id_check) ? (std::string{mod_id} + "." + match_str.substr(1)) : match_str.substr(1);

        const auto var = variables->find(full_name);
        if (var == variables->end()) {
            result += "false()";
            context_->Debug("Variable \'" + match_str.substr(0, 1) + full_name + "\' not found " + match_str, node_);
        }
        else if (str::equals_nocase(var->second, "false")) {
            result += "false()";
        }
        else if (str::equals_nocase(var->second, "true")) {
            result += "true()";
        }
        else {
            result += var->second;
        }

        searchStart = match[0].second;
    }

    if (!result.empty()) {
        if (searchStart != path.cend()) {
            result.append(searchStart, path.cend());
        }
        path = result;
    }
}

void XmlLookup::ReadPath(std::string prop_path,
                         std::string guid,
                         std::string property,
                         std::string temp,
                         bool skip_values,
                         const XmlPatchType patch_type)
{
    if (prop_path.find('&') != std::string::npos)
    {
        prop_path = std::regex_replace(prop_path, std::regex{"&gt;"}, ">");
        prop_path = std::regex_replace(prop_path, std::regex{"&lt;"}, "<");
    }

    if (guid.empty() && property.empty()) {
        // Rewrite path to use faster GUID lookup
        int g;
        // Matches stuff like this and extracts GUID //Assets[Asset/Values/Standard/GUID='102119']
        if (sscanf(prop_path.c_str(), "@%d", &g) > 0) {
            const auto match = std::string("@") + std::to_string(g);
            if (prop_path.rfind(match, 0) == 0) {
                guid = std::to_string(g);
                guid_ = std::to_string(g);
                speculative_path_type_ = SpeculativePathType::VALUES_CONTAINER;
                prop_path = prop_path.substr(match.length());
                if (patch_type == XmlPatchType::InfoTips) {
                    path_ = "//InfoTipData[Guid='" + guid + "']";
                }
                else {
                    path_ = "//Values[Standard/GUID='" + guid + "']";
                }
            }
            else {
                context_->Warn("Failed to construct speculative path lookup: \"" + prop_path + "\"", node_);
            }
        }
        else if (sscanf(prop_path.c_str(), "//Assets[Asset/Values/Standard/GUID='%d']", &g) > 0) {
            if (std::string("//Assets[Asset/Values/Standard/GUID='") + std::to_string(g) + "']"
                == prop_path) {
                guid                   = std::to_string(g);
                guid_                  = std::to_string(g);
                speculative_path_type_ = SpeculativePathType::ASSET_CONTAINER;
            }
        }
        else if (sscanf(prop_path.c_str(), "//Asset/Values[Standard/GUID='%d']", &g) > 0) {
            const auto match = std::string("//Asset/Values[Standard/GUID='") + std::to_string(g) + "']";
            if (prop_path.rfind(match, 0) == 0) {
                guid = std::to_string(g);
                guid_ = std::to_string(g);
                speculative_path_type_ = SpeculativePathType::VALUES_CONTAINER;
                prop_path = prop_path.substr(match.length());
                path_ = "//Asset/Values[Standard/GUID='" + guid + "']";
            }
            else {
                context_->Warn("Failed to construct speculative path lookup: \"" + prop_path + "\"", node_);
            }
        }
        else if (sscanf(prop_path.c_str(), "//Values[Standard/GUID='%d']", &g) > 0) {
            const auto match = std::string("//Values[Standard/GUID='") + std::to_string(g) + "']";
            if (prop_path.rfind(match, 0) == 0) {
                guid = std::to_string(g);
                guid_ = std::to_string(g);
                speculative_path_type_ = SpeculativePathType::VALUES_CONTAINER;
                prop_path = prop_path.substr(match.length());
                path_ = "//Values[Standard/GUID='" + guid + "']";
            }
            else {
                context_->Warn("Failed to construct speculative path lookup: \"" + prop_path + "\"", node_);
            }
        }
    }
    else if (!guid.empty()) {
        if (patch_type == XmlPatchType::InfoTips) {
            speculative_path_type_ = SpeculativePathType::VALUES_CONTAINER;
            path_                  = "//InfoTipData[Guid='" + guid + "']";
        }
        else if (skip_values) {
            speculative_path_type_ = SpeculativePathType::VALUES_CONTAINER;
            path_                  = "//Values[Standard/GUID='" + guid + "']";
        }
        else {
            speculative_path_type_ = SpeculativePathType::SINGLE_ASSET;
            path_                  = "//Asset[Values/Standard/GUID='" + guid + "']";
        }
    }
    else if (!property.empty()) {
        speculative_path_type_ = SpeculativePathType::VALUES_CONTAINER;
        path_                  = "//" + property + "[../../Values]";
    }

    if (prop_path.empty()) {
        prop_path = "/";
    }

    if (temp.empty()) {
        char g[256];
        if (sscanf(prop_path.c_str(), "//Template[Name='%255[^']']", g) == 1) {
            if (std::string("//Template[Name='" + std::string(g) + "']") == prop_path) {
                temp                   = g;
                template_              = g;
                speculative_path_type_ = SpeculativePathType::TEMPLATE_CONTAINER;
            }
        }
    } else {
        speculative_path_type_ = SpeculativePathType::SINGLE_TEMPLATE;
        path_                  = "//Template[Name='" + temp + "']";
    }

    if ((!property.empty() || !guid.empty())  && prop_path.at(0) != '/') {
        if (prop_path.size() > 1 && prop_path.at(0) == '.' && prop_path.at(1) == '[') {
            // Support XPath 2.0 style .[Condition] by replacing it with self::node()[Condition]
            path_ += "/self::node()" + prop_path.substr(1);
        }
        else {
            path_ += "/" + prop_path;
        }
    }
    else {
        path_ += prop_path;
    }
    if (path_ == "/") {
        path_ = "/*";
    }
    if (path_.length() > 0) {
        if (path_[path_.length() - 1] == '/') {
            path_ = path_.substr(0, path_.length() - 1);
        }
    }

    if (!guid.empty() || !temp.empty() || !property.empty()) {
        if (speculative_path_type_ == SpeculativePathType::ASSET_CONTAINER
            || speculative_path_type_ == SpeculativePathType::TEMPLATE_CONTAINER) {
            speculative_path_ = "/";
        } else {
            speculative_path_ += prop_path;
        }

        if (speculative_path_ == "/") {
            speculative_path_ = "self::node()";
        }

        if (speculative_path_.length() > 0) {
            if (speculative_path_[speculative_path_.length() - 1] == '/') {
                speculative_path_ = speculative_path_.substr(0, speculative_path_.length() - 1);
            }
        }

        if (speculative_path_.find("/") == 0) {
            speculative_path_ = speculative_path_.substr(1);
        }
    }
}

XmlLookup::Result XmlLookup::Select(std::shared_ptr<pugi::xml_document> doc,
    std::optional<pugi::xml_node> lookup_origin,
    std::optional<pugi::xml_node>* assetNode) const
{
    pugi::xpath_query query;
    auto node = PrepareLookupNode(doc, lookup_origin, &query, assetNode);
    if (node) {
        if (query.return_type() == pugi::xpath_type_node_set) {
            return node->select_nodes(query);
        }
        else if (query.return_type() == pugi::xpath_type_boolean) {
            return XmlLookup::Result{ query.evaluate_boolean(*node) };
        }
        else if (query.return_type() == pugi::xpath_type_number ||
                query.return_type() == pugi::xpath_type_string) {
            pugi::xml_node wrapper = doc->root().append_child("ModOpEval");
            wrapper.text().set(query.evaluate_string(*node).c_str());
            auto result = wrapper.select_nodes("self::node()/text()");
            return XmlLookup::Result{ result, doc->root(), wrapper };
        }
    }

    return {};
}

XmlOperation::XmlOperation(std::shared_ptr<XmlOperationContext> context, pugi::xml_node node,
                           const std::string& guid,
                           const std::string& property,
                           const std::string& templ) : context_(context)
{
    node_     = node;
    guid_     = guid;
    template_ = templ;
    property_ = property;
    variables_ = {};

    ReadType(node_);
}

void XmlOperation::ReadType(pugi::xml_node node)
{
    type_ = Type::None;
    skip_values_ = false;

    if (pugih::equals(node, "Include") ||
        pugih::equals(node, "Group")) {
        type_ = Type::Group;
    }
    else if (pugih::equals(node, "Asset")) {
        type_ = Type::Asset;
    }
    else if (pugih::equals(node, "Assets")) {
        type_ = Type::Assets;
    }

    bool path_set = false;

    const auto setType = [this, &path_set](Type type, const char* path) {
        if (path_set) {
            context_->Error("Cannot specify \"Path\" twice.", node_);
            return;
        }
        if (type_ != Type::None) {
            context_->Error("Cannot specify \"Type\" twice.", node_);
            return;
        }

        path_set = true;
        path_attribute_ = path;
        skip_values_ = true;
        type_ = type;
    };

    for (const auto& attr : node.attributes()) {
        if (pugih::equals(attr, "Type")) {
            const auto type = attr.as_string();

            if (type_ != Type::None) {
                context_->Error("Cannot specify \"Type\" twice.", node_);
                continue;
            }

            skip_values_ = false;
            if (str::equals_nocase(type, "add")) {
                type_ = Type::Add;
            } else if (str::equals_nocase(type, "assets")) {
                type_ = Type::Assets;
            } else if (str::equals_nocase(type, "addAfter")) {
                type_ = Type::AddNextSibling;
            } else if (str::equals_nocase(type, "addBefore")) {
                type_ = Type::AddPrevSibling;
            } else if (str::equals_nocase(type, "addNextSibling")) {
                type_ = Type::AddNextSibling;
            } else if (str::equals_nocase(type, "addPrevSibling")) {
                type_ = Type::AddPrevSibling;
            } else if (str::equals_nocase(type, "remove")) {
                type_ = Type::Remove;
            } else if (str::equals_nocase(type, "replace")) {
                type_ = Type::Replace;
            } else if (str::equals_nocase(type, "merge")) {
                type_ = Type::Merge;
            } else {
                type_ = Type::None;
                context_->Error(std::string{ "Unknown ModOp " } + type, node_);
            }
        }
        else if (pugih::equals(attr, "Path")) {
            if (path_set) {
                context_->Error("Cannot specify \"Path\" twice.", node_);
                continue;
            }

            path_attribute_ = attr.as_string();
        }
        else if (pugih::equals(attr, "Add")) {
            setType(Type::Add, attr.as_string());
        }
        else if (pugih::equals(attr, "Append")) {
            setType(Type::AddNextSibling, attr.as_string());
        }
        else if (pugih::equals(attr, "Prepend")) {
            setType(Type::AddPrevSibling, attr.as_string());
        }
        else if (pugih::equals(attr, "Remove")) {
            setType(Type::Remove, attr.as_string());
        }
        else if (pugih::equals(attr, "Replace")) {
            setType(Type::Replace, attr.as_string());
        }
        else if (pugih::equals(attr, "Merge")) {
            setType(Type::Merge, attr.as_string());
        }
    }

    if (type_ == Type::Add && path_attribute_.empty() && guid_.empty() && property_.empty() && template_.empty()) {
        type_ = Type::Assets;
    }
}

void XmlOperation::CreateQueries(const XmlPatchType patch_type)
{
    if (type_ != Type::Remove) {
        nodes_ = node_.children();
    }

    if (type_ == Type::Assets || type_ == Type::Asset) {
        path_ = XmlLookup{"/AssetList/Groups[last()]/Group[last()]/Assets[last()]", {}, {}, {}, &variables_, context_, node_, skip_values_, patch_type};
    }
    else {
        path_ = XmlLookup{path_attribute_, guid_, property_, template_, &variables_, context_, node_, skip_values_, patch_type};
    }

    condition_ = XmlLookup{pugih::attrib(node_, "Condition").as_string(), guid_, property_, template_, &variables_, context_, node_, skip_values_, patch_type};
    allow_no_match_ = pugih::attrib(node_, "AllowNoMatch");
    if (const auto max_repeat = pugih::attrib(node_, "MaxRepeat"); max_repeat) {
        max_repeat_ = std::clamp(str::fromchars(max_repeat.as_string(), 1), 1, 1000);
    }

    if (type_ != Type::Remove) {
        content_ = XmlLookup{pugih::attrib(node_, "Content").as_string(), guid_, property_, template_, &variables_, context_, node_, skip_values_, patch_type};
    }
}

std::optional<pugi::xml_node> XmlLookup::FindAsset(const std::string& guid, pugi::xml_node node, int speculate_position) const
{
    if (pugih::equals(node, "Asset")) {
        auto values = pugih::child(node, "Values");
        if (!values) {
            return {};
        }

        auto standard = pugih::child(values, "Standard");
        if (!standard) {
            return {};
        }

        auto GUID = pugih::child(standard, "GUID");
        if (!GUID) {
            return {};
        }

        if (GUID.text().get() != guid) {
            return {};
        }
        if (speculative_path_type_ == SpeculativePathType::ASSET_CONTAINER) {
            auto parent = node.parent();
            while (parent && !pugih::equals(parent, "Assets")) {
                parent = parent.parent();
            }
            if (pugih::equals(parent, "Assets")) {
                return parent;
            }
            return {};
        } else if (speculative_path_type_ == SpeculativePathType::VALUES_CONTAINER) {
            return values;
        } else {
            return node;
        }
    }

    if (speculate_position == 0) {
        // first group level
        static std::optional<pugi::xml_node> last_search;
        if (last_search) {
            for (pugi::xml_node n : node.children()) {
                if (n == last_search) {
                    if (auto found = FindAsset(guid, n, speculate_position - 1); found) {
                        last_search = n;
                        return found;
                    }
                }
            }
        }
        for (pugi::xml_node n : node.children()) {
            if (n != last_search) {
                if (auto found = FindAsset(guid, n, speculate_position - 1); found) {
                    last_search = n;
                    return found;
                }
            }
        }
    }
    else {
        // normal asset finding
        for (pugi::xml_node n : node.children()) {
            if (auto found = FindAsset(guid, n, speculate_position - 1); found) {
                return found;
            }
        }
    }

    return {};
}

std::optional<pugi::xml_node> XmlLookup::FindTemplate(const std::string& temp, pugi::xml_node node) const
{
    if (pugih::equals(node, "Template")) {
        auto template_name = pugih::child(node, "Name");
        if (!template_name) {
            return {};
        }

        if (template_name.text().get() != temp) {
            return {};
        }

        if (speculative_path_type_ == SpeculativePathType::TEMPLATE_CONTAINER) {
            auto parent = node.parent();
            while (parent && !pugih::equals(parent, "Templates")) {
                parent = parent.parent();
            }
            if (pugih::equals(parent, "Templates")) {
                return parent;
            }
            return {};
        } else {
            return node;
        }
    }

    for (pugi::xml_node n : node.children()) {
        if (auto found = FindTemplate(temp, n); found) {
            return found;
        }
    }

    return {};
}

std::optional<pugi::xml_node> XmlLookup::FindTemplate(std::shared_ptr<pugi::xml_document> doc,
                                                      const std::string& temp) const
{
    return FindTemplate(temp, doc->root());
}

std::optional<pugi::xml_node> XmlLookup::PrepareLookupNode(
    std::shared_ptr<pugi::xml_document> doc,
    std::optional<pugi::xml_node> lookup_origin,
    pugi::xpath_query* query,
    std::optional<pugi::xml_node>* assetNode) const
{
    std::optional<pugi::xml_node> root{};

    if (!guid_.empty()) {
        auto cached = (assetNode && *assetNode) ? FindAsset(guid_, **assetNode) : std::optional<pugi::xml_node>{};
        auto node = cached ? cached : FindAsset(guid_, doc->root());

        if (assetNode) {
            *assetNode = node;
        }
        root = node;
    }
    else if (!template_.empty()) {
        root = FindTemplate(doc, template_);
    }

    auto& path = root ? speculative_path_ : path_;
    if (!root) {
        root = lookup_origin ? *lookup_origin : *doc;
    }

    try {
        *query = pugi::xpath_query{ path.c_str(), variables_ };
    }
    catch (const pugi::xpath_exception& e) {
        context_->Error("Failed to parse path \"" + path_ + "\": " + e.what(), node_);
        return {};
    }

    return root;
}

pugi::xpath_node_set XmlLookup::ReadGuidNodes(std::shared_ptr<pugi::xml_document> doc, std::optional<pugi::xml_node>* assetNode) const
{
    pugi::xpath_node_set results;
    std::optional<pugi::xml_node> node;

    if (!guid_.empty()) {
        try {
            auto cached = (assetNode && *assetNode) ? FindAsset(guid_, **assetNode) : std::optional<pugi::xml_node>{};
            node = cached ? cached : FindAsset(guid_, doc->root());
            if (node) {
                if (speculative_path_ != "*") {
                    results = node->select_nodes(speculative_path_.c_str());
                }
            }
        } catch (const pugi::xpath_exception& e) {
            context_->Error("Speculative path failed to find node with path \"" + GetPath() +
                       "\" " + speculative_path_, node_);
            context_->Error(e.what());
        }
    }

    if (assetNode) {
        *assetNode = node;
    }
    return results;
}

pugi::xpath_node_set XmlLookup::ReadTemplateNodes(std::shared_ptr<pugi::xml_document> doc) const
{
    pugi::xpath_node_set results;
    if (!template_.empty()) {
        try {
            auto node = FindTemplate(doc, template_);
            if (node && speculative_path_ != "*") {
                results = node->select_nodes(speculative_path_.c_str());
            }
        } catch (const pugi::xpath_exception& e) {
            context_->Error("Speculative path failed to find node with path \"" + GetPath() +
                       "\" " + speculative_path_, node_);
            context_->Error(e.what());
        }
    }
    return results;
}

void XmlOperation::Apply(std::shared_ptr<pugi::xml_document> doc, XmlPatchType patch_type, const bool log_times)
{
    auto start = std::chrono::high_resolution_clock::now();
    auto logTime = [&start, &log_times, this](const char* group = "ModOp") {
        if (log_times) {
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
            this->context_->Debug("Time: {}ms {} ({}:{})", duration, group,
                this->context_->GetGenericPath(), this->context_->GetLine(node_));
        }
    };

    CreateQueries(patch_type);

    std::optional<pugi::xml_node> cached_node;
    if (GetType() == XmlOperation::Type::None || !CheckCondition(doc, cached_node)) {
        return logTime(type_ == Type::Group ? "Group" : type_ == Type::Asset ? "Asset" : "ModOp");
    }

    if (type_ == Type::Group) {
        int remaining_repeats = max_repeat_;
        do {
            const bool log_times_group = log_times_group && remaining_repeats == max_repeat_;
            for (auto& modop : group_) {
                modop.Apply(doc, patch_type, log_times_group);
            }
            remaining_repeats--;
        } while (remaining_repeats > 0 && CheckCondition(doc, cached_node));

        if (!group_.empty() && group_[0].context_ != this->context_) {
            // special handling for top-level ModOps of an included file
            // the time is logged twice, once for the include and once for top-level of the original file
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
            this->context_->Debug("Time: {}ms {} ({}:{})", duration, "Group", group_[0].context_->GetGenericPath(), 0);
        }

        logTime("Group");
        return;
    }

    std::optional<pugi::xml_node> wrapper;
    std::vector<pugi::xml_node> content_nodes;
    if (content_.IsEmpty() && nodes_) {
        content_nodes.insert(content_nodes.end(), nodes_->begin(), nodes_->end());
    }
    else if (GetType() != XmlOperation::Type::Remove
        && !content_.IsEmpty()
        && !FetchModOpContent(content_, doc, {}, nodes_, content_nodes, wrapper, ".//ModOpContent")) {
        return logTime(type_ == Type::Asset ? "Asset" : "ModOp");
    }

    xmlops::XmlLookup::Result results;
    context_->Debug("Looking up {}", path_.GetPath());
    try {
        results = path_.Select(doc, {}, &cached_node);
    }
    catch (const pugi::xpath_exception &e) {
        context_->Error("Failed to select path \"" + path_.GetPath() + "\": " + e.what());
        if (wrapper) {
            doc->remove_child(*wrapper);
        }
        return logTime(type_ == Type::Asset ? "Asset" : "ModOp");
    }

    if (results.IsEmpty()) {
        if (allow_no_match_) {
            context_->Debug("No matching node for Path \"{}\"", path_.GetPath());
        }
        else {
            context_->Warn("No matching node for Path \"" + path_.GetPath() + "\"", node_);
        }
        if (wrapper) {
            doc->remove_child(*wrapper);
        }
        return logTime(type_ == Type::Asset ? "Asset" : "ModOp");
    }

    for (pugi::xpath_node xnode : results.Nodes()) {
        pugi::xml_node game_node = xnode.node();

        if (GetType() == XmlOperation::Type::Merge) {
            ModOpMerge(doc, game_node, content_nodes, cached_node, patch_type);
        } else if (GetType() == XmlOperation::Type::AddNextSibling) {
            for (auto &&node : content_nodes) {
                game_node = pugih::copy_after(node, game_node);
                ResolveModValue(doc, game_node, cached_node, patch_type);
            }
        } else if (GetType() == XmlOperation::Type::AddPrevSibling) {
            for (auto &&node : content_nodes) {
                auto result_node = pugih::copy_before(node, game_node);
                ResolveModValue(doc, result_node, cached_node, patch_type);
            }
        } else if (GetType() == XmlOperation::Type::Add || GetType() == XmlOperation::Type::Assets) {
            ModOpAdd(doc, game_node, content_nodes, cached_node, patch_type);
        } else if (GetType() == XmlOperation::Type::Asset) {
            // remove attributes like Condition
            pugi::xml_node parent = content_nodes[0].parent();
            parent.remove_attributes();

            std::vector<pugi::xml_node> asset = { content_nodes[0].parent() };
            ModOpAdd(doc, game_node, asset, cached_node, patch_type);
        } else if (GetType() == XmlOperation::Type::Remove) {
            pugih::remove(game_node);
        } else if (GetType() == XmlOperation::Type::Replace) {
            for (auto &node : content_nodes) {
                auto result_node = pugih::copy_after(node, game_node);
                ResolveModValue(doc, result_node, cached_node, patch_type);
            }
            pugih::remove(game_node);
        }
    }

    if (wrapper) {
        doc->remove_child(*wrapper);
    }
    logTime(type_ == Type::Asset ? "Asset" : "ModOp");
}

std::vector<XmlOperation> XmlOperation::GetXmlOperations(
    std::shared_ptr<XmlOperationContext> doc,
    const fs::path& game_path,
    std::optional<pugi::xml_object_range<pugi::xml_node_iterator>> nodes)
{
    if (!doc) {
        return {};
    }

    if (!nodes) {
        auto root = doc->GetRoot();
        if (!root) {
            return {};
        }
        nodes = root.children();
    }

    std::vector<XmlOperation> mod_operations;
    for (pugi::xml_node node : *nodes) {
        if (node.type() == pugi::xml_node_type::node_element) {
            if (node.attribute("Skip")) {
                continue;
            }

            if (pugih::equals(node, "ModOp")) {
                const auto guid = GetXmlPropString(node, "GUID");
                const auto temp = GetXmlPropString(node, "Template");
                const auto property = GetXmlPropString(node, "Property");
                std::vector<std::string> guids;
                if (!temp.empty() + !guid.empty() + !property.empty() > 1) {
                    doc->Error("You can use only one of `GUID`, `Property` or `Template`", node);
                }
                if (!guid.empty()) {
                    std::vector<std::string> guids = str::split(guid, ',');
                    for (auto g : guids) {
                        mod_operations.emplace_back(doc, node, g.data(), "", "");
                    }
                }
                else if (!property.empty()) {
                    mod_operations.emplace_back(doc, node, "", property, "");
                }
                else {
                    mod_operations.emplace_back(doc, node, "", "", temp);
                }
            }
            else if (pugih::equals(node, "Asset") || pugih::equals(node, "Assets")) {
                mod_operations.emplace_back(doc, node, "", "", "");
            }
            else if (pugih::equals(node, "Group")) {
                auto group_op = XmlOperation{doc, node};
                group_op.group_ = GetXmlOperations(doc, game_path, node.children());
                mod_operations.push_back(group_op);
            }
            else if (pugih::equals(node, "Include")) {
                const auto file = GetXmlPropString(node, "File");
                fs::path relative_include_path;
                if (file.rfind("/", 0) == 0) {
                    relative_include_path = file.substr(1);
                } else {
                    relative_include_path = (doc->GetPath().parent_path() / file).lexically_normal();
                }

                auto group_op = XmlOperation{doc, node};
                const auto include_context = doc->OpenInclude(relative_include_path);
                if (!include_context || include_context->GetGenericPath().empty()) {
                    doc->Error("Include file missing or empty: " + relative_include_path.string(), node);
                }
                else {
                    group_op.group_ = GetXmlOperations(include_context, game_path);
                    mod_operations.push_back(group_op);
                }
            }
        }
    }

    return mod_operations;
}

std::vector<XmlOperation> XmlOperation::GetXmlOperationsFromFile(const fs::path&    file_path,
                                                                 std::string_view   mod_name,
                                                                 std::string_view   mod_id,
                                                                 const std::map<std::string, std::string>* variables,
                                                                 const fs::path&    game_path,
                                                                 const fs::path&    mod_path)
{
    const auto mod_relative_path = file_path.lexically_relative(mod_path);
    return GetXmlOperations(std::make_shared<XmlOperationContext>
        (mod_relative_path, mod_path, mod_id, variables, mod_name),
        game_path);
}

static void MergeFlags(pugi::xml_node node, const pugi::char_t* insert, bool remove = false)
{
    const std::string_view originalFlags = node.child_value();

    const auto insertFlags = str::split(insert, ';');
    auto flags = str::split(originalFlags, ';');

    for (const auto& insertFlag : insertFlags) {
        const auto iter = std::find(flags.begin(), flags.end(), insertFlag);
        const auto found = iter != flags.end();

        if (!found && !remove) {
            flags.emplace_back(insertFlag);
        }
        else if (found && remove) {
            flags.erase(iter);
        }
    }

    if (originalFlags.empty()) {
        node.prepend_child(pugi::xml_node_type::node_pcdata);
    }
    node.first_child().set_value(str::join(flags, ';', 50).c_str());
}

static void MergeProperties(pugi::xml_node game_node, pugi::xml_node patching_node)
{
    for (pugi::xml_attribute &attr : patching_node.attributes()) {
        if (auto at = game_node.find_attribute(
                [attr](auto x) { return std::string(x.name()) == attr.name(); });
            at) {
            game_node.remove_attribute(at);
        }

        game_node.append_attribute(attr.name()).set_value(attr.value());
    }
}

pugi::xml_node XmlOperation::ResolveModValue(std::shared_ptr<pugi::xml_document> doc,
    pugi::xml_node game_node, std::optional<pugi::xml_node>& cached_node, const XmlPatchType patch_type,
    const bool first_level) {

    const bool is_mod_value = game_node.first_attribute() && pugih::equals(game_node, "ModValue");

    if (!is_mod_value && game_node.first_child()) {
        auto child = game_node.first_child();

        while (child) {
            child = ResolveModValue(doc, child, cached_node, patch_type, false);
        }

        return game_node.next_sibling();
    }
    else if (is_mod_value) {
        for (pugi::xml_attribute &attr : game_node.attributes()) {
            if (pugih::equals(attr, "Merge")) {
                MergeFlags(game_node.parent(), attr.as_string());
                return pugih::remove(game_node);
            }
            else if (pugih::equals(attr, "Remove")) {
                MergeFlags(game_node.parent(), attr.as_string(), true);
                return pugih::remove(game_node);
            }
            else if (pugih::equals(attr, "Insert")) {
                auto lookup = XmlLookup{attr.as_string(), {}, {}, {}, &variables_, context_, node_, true, patch_type};

                std::optional<pugi::xml_node> wrapper;
                std::vector<pugi::xml_node> content_nodes;
                // TODO cached_node
                if (!FetchModOpContent(lookup, doc, game_node.parent(), game_node.children(), content_nodes, wrapper, ".//ModValueContent")) {
                    return pugih::remove(game_node);
                }

                auto parent = game_node.parent();

                pugih::remove_non_element_children(parent, "ModValue");

                if (!content_nodes.empty()) {
                    auto node_to_insert = content_nodes.begin();

                    if (!game_node.first_child()) {
                        pugih::copies_after(*node_to_insert, game_node);
                    }
                    else {
                        // Note: it's a bit hacky, but we know that a game_node with childs means we're wrapped, and that meands node_to_insert siblings can be merged
                        RecursiveMerge(doc, parent, *node_to_insert, cached_node, patch_type);
                    }
                }
                else {
                    context_->Warn("Nothing matches \"" + lookup.GetPath() + "\"");
                }

                if (wrapper) {
                    doc->remove_child(*wrapper);
                }

                return pugih::remove(game_node);
            }
        }

        return game_node.next_sibling();
    }

    return {};
}

void XmlOperation::RecursiveMerge(std::shared_ptr<pugi::xml_document> doc,
    pugi::xml_node game_node, pugi::xml_node patching_node, std::optional<pugi::xml_node>& cached_node, const XmlPatchType patch_type)
{
    if (!patching_node) {
        return;
    }

    const auto find_node_with_name = [](pugi::xml_node game_node, auto name, int index) -> pugi::xml_node {
        int found = 0;
        auto children = game_node.children();
        for (pugi::xml_node cur_node : children) {
            if (pugih::equals(cur_node, name)) {
                if (found == index) {
                    return cur_node;
                }
                else {
                    found++;
                }
            }
        }
        return {};
    };

    if (pugih::contains_type(patching_node, pugi::xml_node_type::node_element)) {
        // TODO create tests for comment behavior
        patching_node = pugih::skip_type(patching_node, pugi::xml_node_type::node_pcdata);
    }
    if (pugih::contains_type(game_node, pugi::xml_node_type::node_element)) {
        game_node = pugih::skip_type(game_node, pugi::xml_node_type::node_pcdata);
    }

    auto root_node = game_node;
    std::map<std::string, int> indexer;
    bool indexing_allowed = true;
    for (auto cur_node = patching_node; cur_node; cur_node = cur_node.next_sibling()) {
        const auto name = cur_node.name();
        indexer.try_emplace(name, 0);
        const int index = indexer[name]++;

        bool append_missing = true;

        if (cur_node.first_attribute() && pugih::equals(cur_node, "ModItem")) {
            if (const auto& item_xpath = pugih::attrib(cur_node, "Merge"); item_xpath) {
                indexing_allowed = false;

                // construct "Item[Key='Value']" from either Key or Key='Value'
                const std::string_view item_xpath_str = item_xpath.as_string();
                const bool short_path = item_xpath_str.find("=") == std::string_view::npos;
                std::string xpath_query;

                if (short_path) {
                    const auto& key_child = pugih::child(cur_node, item_xpath_str.data());
                    const auto& value = std::string{ key_child.child_value() };
                    xpath_query = str::concat("Item[", item_xpath_str, "='", value, "']");
                }
                else {
                    xpath_query = str::concat("Item[", item_xpath_str, "]");
                }

                try {
                    game_node = root_node.select_node(xpath_query.c_str()).node();
                }
                catch (const std::exception& e) {
                    // TODO not found is OK, are there other errors?
                    context_->Warn("ModItem \"" + xpath_query + "\" not found: " + e.what(), cur_node);
                }

                cur_node.remove_attribute("Merge");

                // when there's no target, cur_node is copied so rename it
                if (!game_node) {
                    if (const auto& append = pugih::attrib(cur_node, "Append"); append) {
                        pugi::xml_node inserter;

                        xpath_query = str::concat("Item[", append.as_string(), "]");
                        cur_node.remove_attribute("Append");
                        try {
                            inserter = root_node.select_node(xpath_query.c_str()).node();
                        }
                        catch (const std::exception& e) {
                            // TODO possible?
                            context_->Warn("ModItem Append=\"" + xpath_query + "\" not found: " + e.what(), cur_node);
                        }
                        if (inserter) {
                            pugih::copy_after(cur_node, inserter).set_name("Item");
                            append_missing = false;
                        }
                        else {
                            context_->Warn("ModItem Append=\"" + xpath_query + "\" not found", cur_node);
                        }
                    }
                    else if (const auto& prepend = pugih::attrib(cur_node, "Prepend"); prepend) {
                        pugi::xml_node inserter;

                        xpath_query = str::concat("Item[", prepend.as_string(), "]");
                        cur_node.remove_attribute("Prepend");
                        try {
                            inserter = root_node.select_node(xpath_query.c_str()).node();
                        }
                        catch (const std::exception& e) {
                            // TODO possible?
                            context_->Warn("ModItem Append=\"" + xpath_query + "\" not found: " + e.what(), cur_node);
                        }
                        if (inserter) {
                            pugih::copy_before(cur_node, inserter).set_name("Item");
                            append_missing = false;
                        }
                        else {
                            context_->Warn("ModItem Append=\"" + xpath_query + "\" not found", cur_node);
                        }
                    }
                    else {
                        cur_node.set_name("Item");
                    }
                }
            }
            else {
                context_->Warn("ModItem needs attribute 'Merge'", cur_node);
            }
        }
        else if (!indexing_allowed) {
            context_->Warn(std::string{name} + " is not allowed after ModItem", cur_node);
            continue;
        }
        else {
            game_node = find_node_with_name(root_node, cur_node.name(), index);
        }

        if (game_node) {
            if (cur_node.type() == pugi::xml_node_type::node_pcdata) {
                game_node.set_value(cur_node.value());
            } else {
                MergeProperties(game_node, cur_node);
                RecursiveMerge(doc, game_node, cur_node.first_child(), cached_node, patch_type);
            }
        }
        else if (append_missing) {
            ResolveModValue(doc, root_node.append_copy(cur_node), cached_node, patch_type);
        }
        else {
            // do nothing
        }
    }
}

void XmlOperation::ModOpAdd(std::shared_ptr<pugi::xml_document> doc,
    pugi::xml_node game_node,
    const std::vector<pugi::xml_node>& content_nodes,
    std::optional<pugi::xml_node>& cached_node,
    const XmlPatchType patch_type)
{
    for (auto& node : content_nodes) {
        // TODO if node.name == Asset
        if (auto base_asset = node.child("BaseAssetGUID"); base_asset) {
            auto base_guid = base_asset.child_value();
            auto lookup = XmlLookup{{}, base_guid, {}, {}, nullptr, context_, node_, false, XmlPatchType::Assets};

            context_->Debug("Looking up {}", lookup.GetPath());
            xmlops::XmlLookup::Result results;
            try {
                results = lookup.Select(doc, {}, &cached_node);
            }
            catch (const pugi::xpath_exception &e) {
                context_->Error("Failed to select path \"" + lookup.GetPath() + "\": " + e.what());
                continue;
            }

            if (results.IsEmpty()) {
                context_->Warn("BaseAssetGUID not found \"" + lookup.GetPath() + "\"", base_asset);
            }

            for (auto& base_node : results.Nodes()) {
                auto result_node = pugih::copy_after(node, base_node.node());
                ResolveModValue(doc, result_node, cached_node, patch_type);
            }
        }
        else {
            auto result_node = game_node.append_copy(node);
            ResolveModValue(doc, result_node, cached_node, patch_type);
        }
    }
}

void XmlOperation::ModOpMerge(std::shared_ptr<pugi::xml_document> doc,
    pugi::xml_node game_node,
    const std::vector<pugi::xml_node>& content_nodes,
    std::optional<pugi::xml_node>& cached_node,
    const XmlPatchType patch_type) {
    if (!content_nodes.empty() && content_nodes.size() == 1 &&
        pugih::equals(*content_nodes.begin(), game_node.name())) {
        // legacy merge
        // skip single container if it's named same as the target node
        RecursiveMerge(doc, game_node.parent(), *content_nodes.begin(), cached_node, patch_type);
    }
    else if (!content_nodes.empty()) {
        RecursiveMerge(doc, game_node, *content_nodes.begin(), cached_node, patch_type);
    }
}

[[nodiscard]] bool XmlOperation::FetchModOpContent(XmlLookup lookup,
    std::shared_ptr<pugi::xml_document> doc, std::optional<pugi::xml_node> lookup_origin,
    std::optional<pugi::xml_object_range<pugi::xml_node_iterator>> wrapping_nodes,
    std::vector<pugi::xml_node>& output, std::optional<pugi::xml_node>& output_wrapper,
    const char* content_tag) {

    if (lookup.IsEmpty() || !content_tag) {
        return false;
    }

    auto result = lookup.Select(doc, lookup_origin);
    if (result.IsEmpty()) {
        context_->Warn("Content \"" + lookup.GetPath() + "\" not found", node_); // TODO node_
        return false;
    }

    if (!wrapping_nodes || wrapping_nodes->begin() != wrapping_nodes->end()) {
        output_wrapper = doc->append_child("ModOpTemp");
        for (auto& node : result.Nodes()) {
            for (auto wrapper_node = wrapping_nodes->begin(); wrapper_node != wrapping_nodes->end(); wrapper_node++) {

                output_wrapper->append_copy(*wrapper_node);
            }
            auto mod_op_content = output_wrapper->select_node(content_tag);
            if (!mod_op_content) {
                context_->Warn("ModOps with 'Content' attribute must be empty or contain '<ModOpContent />'", node_); // TODO node_
                break;
            }
            else {
                auto skip_parent = pugih::attrib(mod_op_content.node(), "SkipParent");
                if (skip_parent) {
                    pugih::copies_after(node.node().first_child(), mod_op_content.node());
                }
                else {
                    pugih::copy_after(node.node(), mod_op_content.node());
                }

                pugih::remove(mod_op_content.node());
            }
        }
        output.insert(output.end(), output_wrapper->children().begin(), output_wrapper->children().end());
    }
    else {
        for (auto& node : result.Nodes()) {
            output.push_back(node.node());
        }
    }

    return true;
}

bool XmlOperation::CheckCondition(std::shared_ptr<pugi::xml_document> doc, std::optional<pugi::xml_node>& cachedNode)
{
    if (condition_.IsEmpty()) {
        return true;
    }

    const bool matching = condition_.Select(doc, {}, &cachedNode).IsMatch();
    if (condition_.IsNegative() == matching) {
        context_->Debug("Condition {} ({}) doesn't match in {} ({}:{})", condition_.GetPath(), matching, context_->GetName(),
                   context_->GetGenericPath(), context_->GetLine(node_));
        return false;
    }

    return true;
}

}
