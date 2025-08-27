#include "xml_pugi_helper.h"

namespace pugih {

bool str::equals_nocase(std::string_view a, std::string_view b) {
#ifndef _WIN32
    auto strnicmp = [](auto a, auto b) { return strncasecmp(a, b); };
#endif
    if (a.size() != b.size()) {
        return false;
    }
    return strnicmp(a.data(), b.data(), a.size()) == 0;
}

bool equals(const pugi::xml_node& node, const std::string_view name) {
  return str::equals_nocase(node.name(), name);
}

bool equals(const pugi::xml_attribute& attribute, const std::string_view name) {
  return str::equals_nocase(attribute.name(), name);
}

void copies_after(const pugi::xml_node& nodes_to_insert, pugi::xml_node position_hint) {
    for (auto insert_child = nodes_to_insert; insert_child && position_hint; insert_child = insert_child.next_sibling()) {
        position_hint = position_hint.parent().insert_copy_after(insert_child, position_hint);
    }
}

pugi::xml_node copy_after(const pugi::xml_node& node_to_insert, pugi::xml_node position_hint) {
    return position_hint.parent().insert_copy_after(node_to_insert, position_hint);
}

pugi::xml_node copy_before(const pugi::xml_node& node_to_insert, pugi::xml_node position_hint) {
    return position_hint.parent().insert_copy_before(node_to_insert, position_hint);
}

pugi::xml_node remove(const pugi::xml_node& child_to_remove){
    auto next = child_to_remove.next_sibling();
    child_to_remove.parent().remove_child(child_to_remove);
    return next;
}

void remove_non_element_children(const pugi::xml_node& parent, const char* stop_element) {
  bool has_elements = false;
  for (auto& child : parent.children()) {
      if (child.type() == pugi::node_element && !str::equals_nocase(child.name(), stop_element)) {
          has_elements = true;
          break;
      }
  }

  if (!has_elements) {
      for (pugi::xml_node child = parent.first_child(); child; ) {
          child = child.type() != pugi::node_element ? pugih::remove(child) : child.next_sibling();
      }
  }
}

[[nodiscard]] const bool contains_type(const pugi::xml_node& first, const pugi::xml_node_type type)
{
  for (auto node = first; node; node = node.next_sibling()) {
    if (node.type() == type) {
        return true;
    }
  }
  return false;
}

const pugi::xml_node skip_type(const pugi::xml_node& node, const pugi::xml_node_type type) {
  pugi::xml_node next = node;
  while (next && next.type() == type) {
    next = next.next_sibling();
  }
  return next;
}

[[nodiscard]] pugi::xml_node child(const pugi::xml_node& node, const std::string_view name) {
  for (auto child = node.first_child(); child; child = child.next_sibling()) {
    if (pugih::equals(child, name)) {
      return child;
    }
  }

  return {};
}

pugi::xml_attribute attrib(const pugi::xml_node& node, const std::string_view name) {
  for (auto attrib = node.first_attribute(); attrib; attrib = attrib.next_attribute()) {
    if (pugih::equals(attrib, name)) {
      return attrib;
    }
  }

  return {};
}

}