#pragma once

#include "pugixml.hpp"

#include <filesystem>
#include <functional>
#include <map>
#include <optional>
#include <set>
#include <string>
#include <vector>

namespace fs = std::filesystem;

namespace xmlops {

class XmlOperationContext
{
public:
    using offset_data_t = std::vector<ptrdiff_t>;
    using include_loader_t = std::function<std::shared_ptr<XmlOperationContext>(const fs::path&)>;

    XmlOperationContext(const fs::path& mod_relative_path,
                        const fs::path& mod_base_path,
                        std::string_view mod_id,
                        const std::map<std::string, std::string>* variables,
                        std::string_view mod_name = {});
    XmlOperationContext(const char* buffer, size_t size,
                        const fs::path& doc_path,
                        std::string_view mod_id,
                        const std::map<std::string, std::string>* variables,
                        std::string_view mod_name = {},
                        std::optional<include_loader_t> include_loader = {});

    [[nodiscard]] std::shared_ptr<XmlOperationContext> OpenInclude(const fs::path& file_path) const;

    void SetLoader(include_loader_t loader) { include_loader_ = loader; }

    [[nodiscard]] size_t GetLine(pugi::xml_node node) const { return GetLine(node.offset_debug()); }
    [[nodiscard]] size_t GetLine(ptrdiff_t offset) const;

    [[nodiscard]] std::shared_ptr<pugi::xml_document> GetDoc() const { return doc_; }
    [[nodiscard]] pugi::xml_node GetRoot() const;
    [[nodiscard]] fs::path GetPath() const { return doc_path_; }
    [[nodiscard]] const std::string& GetGenericPath() const { return doc_path_; }
    [[nodiscard]] const std::string& GetName() const { return mod_name_; }
    [[nodiscard]] const std::map<std::string, std::string>* GetVariables() const { return variables_; }

    template<typename... Args> void Debug(std::string_view msg, const Args &... args) const;
    void Debug(std::string_view msg, pugi::xml_node node) const;
    void Warn(std::string_view msg, pugi::xml_node node = {}) const;
    void Error(std::string_view msg, pugi::xml_node node = {}) const;

    [[nodiscard]] static bool ReadFile(const fs::path& file_path, std::vector<char>& buffer, size_t& size);

private:
    std::string mod_name_;
    std::filesystem::path mod_base_path_;

    const std::string mod_id_;
    const std::map<std::string, std::string>* variables_;

    std::shared_ptr<pugi::xml_document> doc_;
    offset_data_t offset_data_;
    std::optional<include_loader_t> include_loader_;
    std::string doc_path_;

    [[nodiscard]] static offset_data_t BuildOffsetData(const char* buffer, size_t size);
};

class XmlLookup
{
public:
    class Result
    {
    public:
        Result() {};
        Result(bool value) : boolean_(value) {};
        Result(
            pugi::xpath_node_set nodes,
            std::shared_ptr<pugi::xml_document> doc = nullptr,
            std::optional<pugi::xml_node> temporary = {}) :
            doc_(doc), nodes_(nodes), temporary_(temporary) {};
        ~Result()
        {
            if (temporary_ && doc_) {
                doc_->remove_child(*temporary_);
            }
        };

        [[nodiscard]] bool IsEmpty() const { return nodes_.empty(); };
        [[nodiscard]] const pugi::xpath_node_set& Nodes() const { return nodes_; };
        [[nodiscard]] bool IsMatch() const { return boolean_ || !nodes_.empty(); };

    private:
        std::shared_ptr<pugi::xml_document> doc_ = nullptr;
        pugi::xpath_node_set nodes_;
        std::optional<pugi::xml_node> temporary_;
        bool boolean_ = false;
    };

    XmlLookup();
    XmlLookup(const std::string& path,
              const std::string& guid,
              const std::string& property,
              const std::string& templ,
              pugi::xpath_variable_set* variables,
              std::shared_ptr<XmlOperationContext> context,
              pugi::xml_node node);

    /// @brief Select XPath nodes.
    /// @param assetNode Start search here. Resulting asset is stored back.
    [[nodiscard]] XmlLookup::Result Select(std::shared_ptr<pugi::xml_document> doc,
        std::optional<pugi::xml_node>* assetNode = nullptr) const;

    [[nodiscard]] bool IsEmpty() const { return empty_path_; };
    [[nodiscard]] bool IsNegative() const { return negative_; };
    [[nodiscard]] const std::string& GetPath() const { return path_; };
    [[nodiscard]] bool IsModId() const { return mod_id_; };

private:
    std::shared_ptr<XmlOperationContext> context_;
    pugi::xml_node node_;
    pugi::xpath_variable_set* variables_;

    bool empty_path_;
    bool negative_;
    std::string path_;
    std::string guid_;
    std::string property_;
    std::string template_;
    bool mod_id_ = false;

    enum SpeculativePathType {
        NONE,
        SINGLE_ASSET,
        VALUES_CONTAINER,
        ASSET_CONTAINER,
        SINGLE_TEMPLATE,
        TEMPLATE_CONTAINER,
    };

    std::string speculative_path_;
    SpeculativePathType speculative_path_type_ = SpeculativePathType::NONE;

    void ReplaceStaticVariables(std::string& path);
    void ReadPath(std::string path, std::string guid, std::string property, std::string templ);

    [[nodiscard]] std::optional<pugi::xml_node> FindAsset(const std::string& guid, pugi::xml_node node, int speculate_position = 2) const;
    [[nodiscard]] std::optional<pugi::xml_node> FindTemplate(const std::string& temp, pugi::xml_node node) const;
    [[nodiscard]] std::optional<pugi::xml_node> FindTemplate(std::shared_ptr<pugi::xml_document> doc, const std::string& templ) const;

    [[nodiscard]] std::optional<pugi::xml_node> PrepareLookupNode(
        std::shared_ptr<pugi::xml_document> doc,
        pugi::xpath_query* query,
        std::optional<pugi::xml_node>* assetNode) const;

    /// @brief Select XPath nodes via Values/Standard/GUID.
    /// @param assetNode Start search here. Resulting asset is stored back.
    [[nodiscard]] pugi::xpath_node_set ReadGuidNodes(std::shared_ptr<pugi::xml_document> doc,
        std::optional<pugi::xml_node>* assetNode) const;
    [[nodiscard]] pugi::xpath_node_set ReadTemplateNodes(std::shared_ptr<pugi::xml_document> doc) const;
};

class XmlOperation
{
public:
    enum Type { None, Add, AddNextSibling, AddPrevSibling, Remove, Replace, Merge, Group };

    XmlOperation(std::shared_ptr<XmlOperationContext> doc, pugi::xml_node node,
                 const std::string& guid = "",
                 const std::string& property = "",
                 const std::string& templ = "");

    [[nodiscard]] Type GetType() const { return type_; }

    void Apply(std::shared_ptr<pugi::xml_document> doc);

public:
    static std::vector<XmlOperation> GetXmlOperations(
        std::shared_ptr<XmlOperationContext> doc,
        const fs::path&     game_path,
        std::optional<pugi::xml_object_range<pugi::xml_node_iterator>> nodes = {});
    static std::vector<XmlOperation> GetXmlOperationsFromFile(
        const fs::path&     file_path,
        std::string_view    mod_name,
        std::string_view    mod_id,
        const std::map<std::string, std::string>* variables,
        const fs::path&     game_path,
        const fs::path&     mod_path);

private:
    Type        type_;
    XmlLookup   path_;
    bool        allow_no_match_ = false;
    XmlLookup   condition_;
    XmlLookup   content_;
    pugi::xpath_variable_set variables_;

    std::optional<pugi::xml_object_range<pugi::xml_node_iterator>> nodes_;

    std::shared_ptr<XmlOperationContext> doc_;
    pugi::xml_node node_;
    std::string guid_;
    std::string property_;
    std::string template_;

    std::vector<XmlOperation> group_;

    [[nodiscard]] static std::string GetXmlPropString(pugi::xml_node node, const std::string& prop_name)
    {
        return node.attribute(prop_name.c_str()).as_string();
    }
    void RecursiveMerge(pugi::xml_node game_node, pugi::xml_node patching_node);

    void CreateQueries();
    void ReadType(pugi::xml_node node);

    /// @brief Replace ModOpContent with #option or XPath selection
    void InsertContent(std::vector<pugi::xml_node>& content_nodes);

    /// @brief Check Condition XPath. Can use GUID attribute.
    //         True when nodes are found.
    //         Can be negated with `!`.
    /// @param assetNode Returns GUID asset if found.
    [[nodiscard]] bool CheckCondition(std::shared_ptr<pugi::xml_document> doc, std::optional<pugi::xml_node>& assetNode);
};

}
