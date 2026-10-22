#pragma once

#include <string>
#include "05_Support.h"

class Definition;

class Instance
{
public:
    Instance(const std::string& name, const std::string& definitionId, const Definition* definition, Point position);

    std::string name;
    std::string definitionId;
    const Definition* definition;
    Point position;
};
