#pragma once

#include "pugixml.hpp"

namespace pugih {

[[nodiscard]] bool equals(const pugi::xml_node& node, const std::string_view name);
[[nodiscard]] bool equals(const pugi::xml_attribute& attribute, const std::string_view name);

void copies_after(const pugi::xml_node& nodes_to_insert, pugi::xml_node position_hint);

pugi::xml_node copy_after(const pugi::xml_node& node_to_insert, pugi::xml_node position_hint);
pugi::xml_node copy_before(const pugi::xml_node& node_to_insert, pugi::xml_node position_hint);

pugi::xml_node remove(const pugi::xml_node& child_to_remove);
void remove_non_element_children(const pugi::xml_node& parent, const char* stop_element);

[[nodiscard]] const bool contains_type(const pugi::xml_node& first, const pugi::xml_node_type type);
[[nodiscard]] const pugi::xml_node skip_type(const pugi::xml_node& node, const pugi::xml_node_type type);

[[nodiscard]] pugi::xml_node child(const pugi::xml_node& node, const std::string_view name);
[[nodiscard]] pugi::xml_attribute attrib(const pugi::xml_node& node, const std::string_view name);

}