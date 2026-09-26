#include <catch2/catch.hpp>
#include <pugixml.hpp>

#include "libs/xml-operations/src/xml_filedb_reader.h"

#include <sstream>
#include <string>

using namespace xmlops;

TEST_CASE("ANNO 117 InfoTip FileDB preserves newer enum values, UTF-8 expressions and empty leaves") {
    pugi::xml_document input;
    auto infos = input.append_child("InfoTips");
    auto tip = infos.append_child("InfoTipData");
    tip.append_child("Guid").text().set("123456789");

    auto element = tip.append_child("InfoElement");
    element.append_attribute("Type").set_value("26");

    element.append_child("ColorSource").text().set("{Color:Source(Test)}");
    element.append_child("FirstValue").text().set("{Value:First(Test)}");
    element.append_child("SecondValue").text().set("{Value:Second(Test)}");
    element.append_child("MaximumValue").text().set("{Value:Maximum(Test)}");

    element.append_child("Icon");
    element.append_child("LeftIcon");
    element.append_child("RightIcon");

    std::stringstream binary(
        std::ios::in | std::ios::out | std::ios::binary
    );

    // export.bin is intentional: it activates the ANNO InfoTip FileDB converter.
    FileDbWriter::write(&input, binary, "export.bin");

    const std::string bytes = binary.str();
    REQUIRE_FALSE(bytes.empty());

    std::istringstream first_input(bytes, std::ios::in | std::ios::binary);
    auto first = FileDbReader::read(first_input, "export.bin");
    REQUIRE(first != nullptr);

    auto first_element = first->select_node(
        "/InfoTips/InfoTipData[Guid='123456789']/InfoElement"
    ).node();
    REQUIRE(first_element);
    CHECK(std::string(first_element.attribute("Type").value()) == "26");
    CHECK(std::string(first_element.child_value("ColorSource")) == "{Color:Source(Test)}");
    CHECK(std::string(first_element.child_value("FirstValue")) == "{Value:First(Test)}");
    CHECK(std::string(first_element.child_value("SecondValue")) == "{Value:Second(Test)}");
    CHECK(std::string(first_element.child_value("MaximumValue")) == "{Value:Maximum(Test)}");
    CHECK(first_element.child("Icon"));
    CHECK(first_element.child("LeftIcon"));
    CHECK(first_element.child("RightIcon"));

    // Verify a second serialization cycle as well. This exercises numeric enum
    // fallback after the first reader emitted unknown enum value 26 as text.
    std::stringstream binary2(
        std::ios::in | std::ios::out | std::ios::binary
    );
    FileDbWriter::write(first.get(), binary2, "export.bin");

    const std::string bytes2 = binary2.str();
    REQUIRE_FALSE(bytes2.empty());

    std::istringstream second_input(bytes2, std::ios::in | std::ios::binary);
    auto second = FileDbReader::read(second_input, "export.bin");
    REQUIRE(second != nullptr);

    auto second_element = second->select_node(
        "/InfoTips/InfoTipData[Guid='123456789']/InfoElement"
    ).node();
    REQUIRE(second_element);
    CHECK(std::string(second_element.attribute("Type").value()) == "26");
    CHECK(std::string(second_element.child_value("ColorSource")) == "{Color:Source(Test)}");
    CHECK(std::string(second_element.child_value("FirstValue")) == "{Value:First(Test)}");
    CHECK(std::string(second_element.child_value("SecondValue")) == "{Value:Second(Test)}");
    CHECK(std::string(second_element.child_value("MaximumValue")) == "{Value:Maximum(Test)}");
    CHECK(second_element.child("Icon"));
    CHECK(second_element.child("LeftIcon"));
    CHECK(second_element.child("RightIcon"));
}