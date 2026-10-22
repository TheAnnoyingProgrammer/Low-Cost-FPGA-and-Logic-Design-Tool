#include "01_Library.h"

Library Library::makePrimitive()
{
    Library primitive("Primitive");
    for (GateType type : { GateType::NOT, GateType::AND, GateType::OR, GateType::XOR,
                          GateType::NAND, GateType::NOR, GateType::XNOR })
        primitive.add(Definition::makeGate(type));
    return primitive;
}

Library::Library(const std::string& name)
    : name(name)
{
}

void Library::add(const Definition& definition)
{
    definitions.insert_or_assign(definition.name, definition);
}

const Definition* Library::find(const std::string& definitionName) const
{
    auto found = definitions.find(definitionName);
    return found == definitions.end() ? nullptr : &found->second;
}

const std::map<std::string, Definition>& Library::getDefinitions() const
{
    return definitions;
}

std::map<std::string, Definition>& Library::getDefinitions()
{
    return definitions;
}