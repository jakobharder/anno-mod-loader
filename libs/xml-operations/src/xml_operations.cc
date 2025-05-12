#include "xml_operations.h"

#include "spdlog/spdlog.h"

#include <charconv>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <map>
#include <regex>
#include <string>

namespace xmlops {

std::vector<std::string> StrSplit(const std::string& input, char delimiter) {
    std::vector<std::string> result;

    int last_pos = 0;
    for (int i = 0; i < input.length(); i++) {
        if (input[i] != delimiter) {
            continue;
        }

        if (i - last_pos > 0) {
            result.emplace_back(input.substr(last_pos, i - last_pos));
        }
        last_pos = i + 1;
    }

    if (last_pos != input.length()) {
        result.emplace_back(input.substr(last_pos, input.length() - last_pos));
    }

    return result;
}

XmlOperationContext::XmlOperationContext() { }

XmlOperationContext::XmlOperationContext(const fs::path& mod_relative_path,
                                         const fs::path& mod_base_path,
                                         std::string_view mod_name)
{
    mod_name_ = mod_name.empty() ? mod_base_path.filename().string() : mod_name;
    mod_base_path_ = mod_base_path;

    include_loader_ = [this, &mod_base_path](const fs::path& file_path) {
        std::vector<char> buffer;
        size_t size;
        if (!ReadFile(mod_base_path / file_path, buffer, size)) {
            spdlog::error("{}: Failed to open {}",
                          this->mod_name_,
                          file_path.string());
            return std::shared_ptr<XmlOperationContext>{};
        }
        return std::make_shared<XmlOperationContext>(buffer.data(), size, file_path,
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
                                         std::string_view mod_name,
                                         std::optional<include_loader_t> include_loader)
{
    mod_name_ = mod_name;
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
    include->mod_name_ = mod_name_;
    include->include_loader_ = this->include_loader_;
    return include;
}

size_t XmlOperationContext::GetLine(ptrdiff_t offset) const
{
    auto it = std::lower_bound(offset_data_.begin(), offset_data_.end(), offset);
    return (it - offset_data_.begin()) + 1;
}

pugi::xml_node XmlOperationContext::GetRoot() const
{
#ifndef _WIN32
    auto stricmp = [](auto a, auto b) { return strcasecmp(a, b); };
#endif

    auto root = doc_ ? doc_->root() : pugi::xml_node{};
    if (!root) {
        Error("Failed to get root element");
        return {};
    }

    if (!root.first_child() || stricmp(root.first_child().name(), "ModOps") != 0) {
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
    spdlog::debug("{}: {} ({}:{})", mod_name_, msg, doc_path_, node ? GetLine(node) : 0);
}

void XmlOperationContext::Warn(std::string_view msg, pugi::xml_node node) const
{
    spdlog::warn("{}: {} ({}:{})", mod_name_, msg, doc_path_, node ? GetLine(node) : 0);
}

void XmlOperationContext::Error(std::string_view msg, pugi::xml_node node) const
{
    spdlog::error("{}: {} ({}:{})", mod_name_, msg, doc_path_, node ? GetLine(node) : 0);
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
                     pugi::xml_node node)
{
    context_ = context;
    node_ = node;
    variables_ = variables;

    empty_path_ = path.empty();
    negative_ = !path.empty() && path[0] == '!';

    std::string read_path = negative_ ? path.substr(1) : path;

    if (!read_path.empty() && read_path.front() == '#') {
        path_ = read_path.substr(1);
        mod_id_ = true;
        return;
    }
    else if (!read_path.empty() && read_path.front() == '$') {
        path_ = read_path.substr(0);

        const auto var = read_path.substr(1);
        if (!variables_->get(var.c_str())) {
            variables_->set(var.c_str(), false);
        }
        return;
    }
    else if (!read_path.empty() && read_path[0]== '~') {
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

    ReadPath(read_path, guid_, property_, template_);
}

void XmlLookup::ReadPath(std::string prop_path,
                         std::string guid,
                         std::string property,
                         std::string temp)
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
                path_ = "//Values[Standard/GUID='" + guid + "']";
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
        speculative_path_type_ = SpeculativePathType::VALUES_CONTAINER;
        path_                  = "//Values[Standard/GUID='" + guid + "']";
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
        if (sscanf(prop_path.c_str(), "//Template[Name='%s']", g) > 0) {
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
        path_ += "/" + prop_path;
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
    std::optional<pugi::xml_node>* assetNode) const
{
    pugi::xpath_query query;
    auto node = PrepareLookupNode(doc, &query, assetNode);
    if (node) {
        if (query.return_type() == pugi::xpath_type_node_set) {
            return node->select_nodes(query);
        }
        else if (query.return_type() == pugi::xpath_type_boolean) {
            return XmlLookup::Result{ query.evaluate_boolean(*node) };
        }
        else if (query.return_type() == pugi::xpath_type_number ||
                query.return_type() == pugi::xpath_type_string) {
            pugi::xml_node wrapper = doc->append_child("ModOpEval");
            wrapper.text().set(query.evaluate_string(*node).c_str());
            auto result = wrapper.select_nodes("self::node()/text()");
            return XmlLookup::Result{ result, doc, wrapper };
        }
    }

    return {};
}

XmlOperation::XmlOperation(std::shared_ptr<XmlOperationContext> doc, pugi::xml_node node,
                           const std::string& guid,
                           const std::string& property,
                           const std::string& templ) : doc_(doc)
{
    node_     = node;
    guid_     = guid;
    template_ = templ;
    property_ = property;
    variables_ = {};
}

void XmlOperation::CreateQueries()
{
    ReadType(node_);
    if (type_ != Type::Remove) {
        nodes_ = node_.children();
    }

    const auto& path = GetXmlPropString(node_, "Path");
    if (type_ == Type::Add && guid_.empty() && template_.empty() && property_.empty() && path.empty()) {
        path_ = XmlLookup{"//Groups[1]/Group[1]/Assets[1]", {}, {}, {}, &variables_, doc_, node_};
    }
    else {
        path_ = XmlLookup{path, guid_, property_, template_, &variables_, doc_, node_};
    }

    condition_ = XmlLookup{node_.attribute("Condition").as_string(), guid_, property_, template_, &variables_, doc_, node_};
    allow_no_match_ = node_.attribute("AllowNoMatch");

    if (type_ != Type::Remove) {
        content_ = XmlLookup{node_.attribute("Content").as_string(), guid_, property_, template_, &variables_, doc_, node_};
    }
}

void XmlOperation::ReadType(pugi::xml_node node)
{
#ifndef _WIN32
    auto stricmp = [](auto a, auto b) { return strcasecmp(a, b); };
#endif
    auto type = GetXmlPropString(node, "Type");

    if (stricmp(node.name(), "Include") == 0 ||
        stricmp(node.name(), "Group") == 0) {
        type_ = Type::Group;
    } else if (stricmp(type.c_str(), "add") == 0) {
        type_ = Type::Add;
    } else if (stricmp(type.c_str(), "addAfter") == 0) {
        type_ = Type::AddNextSibling;
    } else if (stricmp(type.c_str(), "addBefore") == 0) {
        type_ = Type::AddPrevSibling;
    } else if (stricmp(type.c_str(), "remove") == 0) {
        type_ = Type::Remove;
    } else if (stricmp(type.c_str(), "replace") == 0) {
        type_ = Type::Replace;
    } else if (stricmp(type.c_str(), "merge") == 0) {
        type_ = Type::Merge;
    } else if (stricmp(type.c_str(), "enum") == 0) {
        type_ = Type::AddEnum;
    } else if (stricmp(type.c_str(), "removeEnum") == 0) {
        type_ = Type::RemoveEnum;
    } else {
        type_ = Type::None;
        doc_->Error("Unknown ModOp " + type, node_);
    }
}

std::optional<pugi::xml_node> XmlLookup::FindAsset(const std::string& guid, pugi::xml_node node, int speculate_position) const
{
#ifndef _WIN32
    auto stricmp = [](auto a, auto b) { return strcasecmp(a, b); };
#endif

    //
    if (stricmp(node.name(), "Asset") == 0) {
        auto values = node.child("Values");
        if (!values) {
            return {};
        }

        auto standard = values.child("Standard");
        if (!standard) {
            return {};
        }

        auto GUID = standard.child("GUID");
        if (!GUID) {
            return {};
        }

        if (GUID.text().get() != guid) {
            return {};
        }
        if (speculative_path_type_ == SpeculativePathType::ASSET_CONTAINER) {
            auto parent = node.parent();
            while (parent && stricmp(parent.name(), "Assets") != 0) {
                parent = parent.parent();
            }
            if (stricmp(parent.name(), "Assets") == 0) {
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
#ifndef _WIN32
    auto stricmp = [](auto a, auto b) { return strcasecmp(a, b); };
#endif
    //
    if (stricmp(node.name(), "Template") == 0) {
        auto template_name = node.child("Name");
        if (!template_name) {
            return {};
        }

        if (template_name.text().get() != temp) {
            return {};
        }

        if (speculative_path_type_ == SpeculativePathType::TEMPLATE_CONTAINER) {
            auto parent = node.parent();
            while (parent && stricmp(parent.name(), "Templates") != 0) {
                parent = parent.parent();
            }
            if (stricmp(parent.name(), "Templates") == 0) {
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
        root = *doc;
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

void XmlOperation::InsertContent(std::vector<pugi::xml_node>& content_nodes, const std::map<std::string, std::string>& mod_options) {
    for (auto& content_node : content_nodes) {
        auto inserter = content_node.select_node(".//ModValue");
        if (inserter) {
            const auto& option_attr = inserter.node().attribute("Path");
            if (!option_attr) {
                if (inserter.node().attribute("MergeFlags")) {
                    continue;
                }
                doc_->Warn("ModValue used without 'Path' or `MergeFlags`", inserter.node());
                continue;
            }

            const auto& option_path = std::string{ option_attr.as_string() };

            if (0 != option_path.rfind("$", 0)) {
                doc_->Warn(std::string{ option_attr.as_string() } + " must start with $", inserter.node());
                continue;
            }

            const auto& option = mod_options.find(option_path.substr(1));
            if (option == mod_options.end()) {
                doc_->Warn("Variable " + option_path + " not found", inserter.node());
                continue;
            }

            auto content = inserter.parent().insert_child_after(pugi::xml_node_type::node_pcdata, inserter.node());
            content.set_value(option->second.c_str());
            inserter.parent().remove_child(inserter.node());
        }
    }
}

bool AddTypedVariable(pugi::xpath_variable_set& vars, const std::string& name, const std::string& value) {
    // Trim the input (optional, but helps with whitespace)
    auto trimmed = value;
    trimmed.erase(0, trimmed.find_first_not_of(" \t\n\r"));
    trimmed.erase(trimmed.find_last_not_of(" \t\n\r") + 1);

    // Try boolean (case-insensitive match)
    std::string lower;
    lower.resize(trimmed.size());
    std::transform(trimmed.begin(), trimmed.end(), lower.begin(), [](unsigned char c) { return std::tolower(c); });

    if (lower == "true" || lower == "false") {
        bool b = (lower == "true");
        vars.add(name.c_str(), pugi::xpath_type_boolean);
        vars.get(name.c_str())->set(b);
        return true;
    }

    // Try number (std::from_chars is fast and non-allocating)
    double number = 0;
    auto [ptr, ec] = std::from_chars(trimmed.data(), trimmed.data() + trimmed.size(), number);
    if (ec == std::errc() && ptr == trimmed.data() + trimmed.size()) {
        vars.add(name.c_str(), pugi::xpath_type_number);
        vars.get(name.c_str())->set(number);
        return true;
    }

    // Fallback: treat as string
    vars.add(name.c_str(), pugi::xpath_type_string);
    vars.get(name.c_str())->set(value.c_str());
    return true;
}

void XmlOperation::Apply(std::shared_ptr<pugi::xml_document> doc, const std::map<std::string, std::string>& mod_options)
{
    auto start = std::chrono::high_resolution_clock::now();
    auto logTime = [&start, this](const char* group = "ModOp") {
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        this->doc_->Debug("Time: {}ms {} ({}:{})", duration, group,
            this->doc_->GetGenericPath(), this->doc_->GetLine(node_));
    };

    CreateQueries();

    for (const auto& option : mod_options) {
        AddTypedVariable(variables_, option.first, option.second);
    }

    std::optional<pugi::xml_node> cachedNode;
    if (GetType() == XmlOperation::Type::None || !CheckCondition(doc, cachedNode, mod_options)) {
        return logTime(type_ == Type::Group ? "Group" : "ModOp");
    }

    if (type_ == Type::Group) {
        // logTime();
        for (auto& modop : group_) {
            modop.Apply(doc, mod_options);
        }
        logTime("Group");
        return;
    }

    std::optional<pugi::xml_node> wrapper;

    std::vector<pugi::xml_node> content_nodes;
    if (type_ != Type::Remove && !content_.IsEmpty()) {
        auto result = content_.Select(doc);
        if (result.IsEmpty()) {
            doc_->Warn("Content \"" + content_.GetPath() + "\" not found", node_);
            return logTime();
        }
        if (!nodes_ || nodes_->begin() != nodes_->end()) {
            wrapper = doc->append_child("ModOpTemp");
            for (auto& node : result.Nodes()) {
                for (auto wrapper_node = nodes_->begin(); wrapper_node != nodes_->end(); wrapper_node++) {

                    wrapper->append_copy(*wrapper_node);
                }
                auto inserter = wrapper->select_node(".//ModOpContent");
                if (!inserter) {
                    doc_->Warn("ModOps with 'Content' attribute must be empty or contain '<ModOpContent />'", node_);
                    break;
                }
                else {
                    inserter.parent().insert_copy_after(node.node(), inserter.node());
                    inserter.parent().remove_child(inserter.node());
                }
            }
            content_nodes.insert(content_nodes.end(), wrapper->children().begin(), wrapper->children().end());
        }
        else {
            for (auto& node : result.Nodes()) {
                content_nodes.push_back(node.node());
            }
        }
    }
    if (content_.IsEmpty() && nodes_) {
        content_nodes.insert(content_nodes.end(), nodes_->begin(), nodes_->end());
    }

    InsertContent(content_nodes, mod_options);

    try {
        doc_->Debug("Looking up {}", path_.GetPath());
        auto results = path_.Select(doc, &cachedNode);
        if (results.IsEmpty()) {
            if (allow_no_match_) {
                doc_->Debug("No matching node for Path \"{}\"", path_.GetPath());
            }
            else {
                doc_->Warn("No matching node for Path \"" + path_.GetPath() + "\"", node_);
            }
            if (wrapper) {
                doc->remove_child(*wrapper);
            }
            return logTime();
        }

        for (pugi::xpath_node xnode : results.Nodes()) {
            pugi::xml_node game_node = xnode.node();
            if (GetType() == XmlOperation::Type::Merge) {
                if (!content_nodes.empty() && content_nodes.size() == 1 &&
                    strcmp(content_nodes.begin()->name(), game_node.name()) == 0) {
                    // legacy merge
                    // skip single container if it's named same as the target node
                    RecursiveMerge(game_node.parent(), *content_nodes.begin());
                }
                else if (!content_nodes.empty()) {
                    RecursiveMerge(game_node, *content_nodes.begin());
                }
            } else if (GetType() == XmlOperation::Type::AddNextSibling) {
                for (auto &&node : content_nodes) {
                    game_node = game_node.parent().insert_copy_after(node, game_node);
                }
            } else if (GetType() == XmlOperation::Type::AddPrevSibling) {
                for (auto &&node : content_nodes) {
                    game_node.parent().insert_copy_before(node, game_node);
                }
            } else if (GetType() == XmlOperation::Type::Add) {
                for (auto &node : content_nodes) {
                    game_node.append_copy(node);
                }
            } else if (GetType() == XmlOperation::Type::Remove) {
                game_node.parent().remove_child(game_node);
            } else if (GetType() == XmlOperation::Type::Replace) {
                for (auto &node : content_nodes) {
                    game_node.parent().insert_copy_after(node, game_node);
                }
                game_node.parent().remove_child(game_node);
            }
        }
    } catch (const pugi::xpath_exception &e) {
        doc_->Error("Failed to parse path \"" + path_.GetPath() + "\": " + e.what());
    }

    if (wrapper) {
        doc->remove_child(*wrapper);
    }
    logTime();
}

std::vector<XmlOperation> XmlOperation::GetXmlOperations(
    std::shared_ptr<XmlOperationContext> doc,
    const fs::path& game_path,
    std::optional<pugi::xml_object_range<pugi::xml_node_iterator>> nodes)
{
#ifndef _WIN32
    auto stricmp = [](auto a, auto b) { return strcasecmp(a, b); };
#endif

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

            if (stricmp(node.name(), "ModOp") == 0) {
                const auto guid = GetXmlPropString(node, "GUID");
                const auto temp = GetXmlPropString(node, "Template");
                const auto property = GetXmlPropString(node, "Property");
                std::vector<std::string> guids;
                if (!temp.empty() + !guid.empty() + !property.empty() > 1) {
                    doc->Error("You can use only one of `GUID`, `Property` or `Template`", node);
                }
                if (!guid.empty()) {
                    std::vector<std::string> guids = StrSplit(guid, ',');
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
            else if (stricmp(node.name(), "Group") == 0) {
                auto group_op = XmlOperation{doc, node};
                group_op.group_ = GetXmlOperations(doc, game_path, node.children());
                mod_operations.push_back(group_op);
            }
            else if (stricmp(node.name(), "Include") == 0) {
                const auto file = GetXmlPropString(node, "File");
                fs::path relative_include_path;
                if (file.rfind("/", 0) == 0) {
                    relative_include_path = file.substr(1);
                } else {
                    relative_include_path = (doc->GetPath().parent_path() / file).lexically_normal();
                }

                auto group_op = XmlOperation{doc, node};
                const auto include_context = doc->OpenInclude(relative_include_path);
                if (include_context->GetGenericPath().empty()) {
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
                                                                 std::string        mod_name,
                                                                 const fs::path&    game_path,
                                                                 const fs::path&    mod_path)
{
    const auto mod_relative_path = file_path.lexically_relative(mod_path);
    return GetXmlOperations(std::make_shared<XmlOperationContext>(mod_relative_path, mod_path, mod_name), game_path);
}

void MergeEnum(pugi::xml_node enum_node, const pugi::char_t* insert)
{
    // TODO properly tokenize by ;
    const auto enum_value = std::string_view{ enum_node.child_value() };
    const size_t found = enum_value.find(insert);

    const bool empty = enum_value.empty();
    if (empty || found == std::string_view::npos) {
        const auto new_value = empty ? std::string{ insert } : std::string{ enum_value } + std::string{ ";" } + std::string{ insert };

        if (!enum_node.first_child()) {
            enum_node.append_child(pugi::xml_node_type::node_pcdata);
        }
        enum_node.first_child().set_value(new_value.c_str());
    }
}

void MergeProperties(pugi::xml_node game_node, pugi::xml_node patching_node)
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

static bool HasNonTextNode(pugi::xml_node node)
{
    while (node) {
        if (node.type() != pugi::xml_node_type::node_pcdata) {
            return true;
        }
        node = node.next_sibling();
    }
    return false;
}

void RecursiveExpandEnum(pugi::xml_node game_node) {
    for (auto& child : game_node.children()) {
        RecursiveExpandEnum(child);
    }

    if (!game_node.first_child()) {
        for (pugi::xml_attribute &attr : game_node.attributes()) {
            if (stricmp(attr.name(), "MergeFlags") == 0) {
                const auto& text = game_node.parent().child_value();
                if (strlen(text) > 0) {
                    MergeEnum(game_node.parent(), attr.as_string());
                }
                else {
                    game_node.parent().append_child(pugi::xml_node_type::node_pcdata).set_value(attr.as_string());
                }
                game_node.parent().remove_child(game_node);
                continue;
            }
        }
    }
}

void XmlOperation::RecursiveMerge(pugi::xml_node game_node, pugi::xml_node patching_node)
{
    if (!patching_node) {
        return;
    }

    const auto find_node_with_name = [](pugi::xml_node game_node, auto name, int index) -> pugi::xml_node {
        int found = 0;
        auto children = game_node.children();
        for (pugi::xml_node cur_node : children) {
            if (strcmp(cur_node.name(), name) == 0) {
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

    if (HasNonTextNode(patching_node)) {
        while (patching_node && patching_node.type() == pugi::xml_node_type::node_pcdata) {
            patching_node = patching_node.next_sibling();
        }
    }

    if (HasNonTextNode(game_node)) {
        while (game_node && game_node.type() == pugi::xml_node_type::node_pcdata) {
            game_node = game_node.next_sibling();
        }
    }

    auto root_node = game_node;
    std::map<std::string, int> indexer;
    bool indexing_allowed = true;
    for (auto cur_node = patching_node; cur_node; cur_node = cur_node.next_sibling()) {
        const auto name = cur_node.name();
        indexer.try_emplace(name, 0);
        const int index = indexer[name]++;

        const auto& item_xpath = cur_node.attribute("ModMergeItem");
        if (item_xpath) {
            indexing_allowed = false;

            // TODO better string format
            const auto xpath_query = std::string{ cur_node.name() } + "[" + item_xpath.as_string() + "]";
            cur_node.remove_attribute("ModMergeItem");
            try
            {
                const auto& xpath_node = root_node.select_node(xpath_query.c_str());
                if (xpath_node) {
                    game_node = xpath_node.node();
                }
                else {
                    game_node = {};
                }
            }
            catch (const std::exception& e)
            {
                doc_->Warn("ModMergeItem \"" + xpath_query + "\" not found", cur_node);
            }
        }
        else if (!indexing_allowed) {
            doc_->Warn(std::string{name} + " without ModMergeItem is not allowed after ModMergeItem usage", cur_node);
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
                RecursiveMerge(game_node, cur_node.first_child());
            }
        }
        else {
            RecursiveExpandEnum(root_node.append_copy(cur_node));
        }
    }
}

bool XmlOperation::CheckCondition(std::shared_ptr<pugi::xml_document> doc, std::optional<pugi::xml_node>& cachedNode,
    const std::map<std::string, std::string>& mod_options)
{
    if (condition_.IsEmpty()) {
        return true;
    }

    bool matching = false;
    // if (condition_.IsModId()) {
    //     const auto& option = mod_options.find(condition_.GetPath());
    //     if (option != mod_options.end()) {
    //         matching = 0 != stricmp(option->second.c_str(), "0")
    //             && 0 != stricmp(option->second.c_str(), "false");

    //         if (condition_.IsNegative() == matching) {
    //             doc_->Debug("Condition {} is '{}' in {} ({}:{})", condition_.GetPath(), option->second, doc_->GetName(),
    //                     doc_->GetGenericPath(), doc_->GetLine(node_));
    //             return false;
    //         }
    //     }
    //     else {
    //         matching = false;
    //     }
    // }
    // else {
        matching = condition_.Select(doc, &cachedNode).IsMatch();
    // }

    if (condition_.IsNegative() == matching) {
        doc_->Debug("Condition {} doesn't match in {} ({}:{})", condition_.GetPath(), doc_->GetName(),
                   doc_->GetGenericPath(), doc_->GetLine(node_));
        return false;
    }

    return true;
}

XmlOperation::Type XmlOperation::GetType() const
{
    return type_;
}

}
