#pragma once

#include <vector>
#include "json.hpp"
#include "02_BackEnd/00_ClassStructures/02_Definition.h"

using Json = nlohmann::ordered_json;

Json toJson(const Pin& pin);
Json toJson(const Symbol& symbol);
Json toJson(const Wire& wire);
Json toJson(const Instance& instance);
Json toJson(const Definition& definition);

Pin pinFromJson(const Json& json);
Symbol symbolFromJson(const Json& json);
Wire wireFromJson(const Json& json);
Instance instanceFromJson(const Json& json);
Definition definitionFromJson(const Json& json);

template <typename T>
Json toJsonArray(const std::vector<T>& items)
{
    Json array = Json::array();
    for (const T& item: items)
        array.push_back(toJson(item));
    return array;
}

template <typename T, typename Reader>
std::vector<T> fromJsonArray(const Json& array, Reader read)
{
    std::vector<T> items;
    for (const Json& item: array)
        items.push_back(read(item));
    return items;
}