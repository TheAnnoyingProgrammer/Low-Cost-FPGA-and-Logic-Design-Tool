#include "00_JsonConvert.h"

namespace
{
std::string pinTypeToString(PinType type)
{
    return type == PinType::Output ? "output" : "input";
}

PinType pinTypeFromString(const std::string& text)
{
    return text == "output" ? PinType::Output : PinType::Input;
}
}

Json toJson(const Pin& pin)
{
    return { {"name", pin.name}, {"type", pinTypeToString(pin.type)},
            {"x", pin.offset.x}, {"y", pin.offset.y} };
}

Json toJson(const Symbol& symbol)
{
    return { {"width", symbol.width}, {"height", symbol.height}, {"shape", symbol.shape} };
}

Json toJson(const Wire& wire)
{
    return { {"x1", wire.start.x}, {"y1", wire.start.y},
            {"x2", wire.end.x},   {"y2", wire.end.y} };
}

Json toJson(const Instance& instance)
{
    return { {"name", instance.name}, {"definition", instance.definitionId},
            {"x", instance.position.x}, {"y", instance.position.y} };
}

Json toJson(const Definition& definition)
{
    return { {"name", definition.name},
            {"pins", toJsonArray(definition.pins)},
            {"symbol", toJson(definition.symbol)},
            {"instances", toJsonArray(definition.instances)},
            {"wires", toJsonArray(definition.wires)} };
}

Pin pinFromJson(const Json& json)
{
    return { json.at("name"), pinTypeFromString(json.at("type")),
            { json.value("x", 0.0), json.value("y", 0.0) } };
}

Symbol symbolFromJson(const Json& json)
{
    return { json.value("width", 0.0), json.value("height", 0.0), json.value("shape", "") };
}

Wire wireFromJson(const Json& json)
{
    return { { json.at("x1"), json.at("y1") }, { json.at("x2"), json.at("y2") } };
}

Instance instanceFromJson(const Json& json)
{
    return Instance(json.at("name"), json.at("definition"), nullptr,
                    { json.at("x"), json.at("y") });
}

Definition definitionFromJson(const Json& json)
{
    Definition definition = Definition::makeCustom(json.at("name"));
    definition.pins = fromJsonArray<Pin>(json.value("pins", Json::array()), pinFromJson);
    definition.symbol = symbolFromJson(json.value("symbol", Json::object()));
    definition.instances = fromJsonArray<Instance>(json.value("instances", Json::array()), instanceFromJson);
    definition.wires = fromJsonArray<Wire>(json.value("wires", Json::array()), wireFromJson);
    return definition;
}