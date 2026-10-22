#pragma once

#include <string>
#include <vector>
#include "01-S_LogicBlockSupport.h"

class LogicBlockDefinition
{
public:
    virtual ~LogicBlockDefinition();

    std::string typeName;          // "AND", "MUX2to1", "MyCounter"
    std::vector<Pin> pinTemplate;
    Symbol symbol;

    virtual bool isPrimitive() const = 0;
    virtual std::vector<bool> evaluate(const std::vector<bool>& inputs) const = 0;
};
