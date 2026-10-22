#pragma once

#include <string>

struct Point
{
    double x = 0.0;
    double y = 0.0;
};

enum class PinType
{
    Input,
    Output
};

struct Pin
{
    std::string name;
    PinType type = PinType::Input;
    Point offset;
};

struct Symbol
{
    double width = 0.0;
    double height = 0.0;
    std::string shape;
};

enum class GateType
{
    NOT,
    AND,
    OR,
    XOR,
    NAND,
    NOR,
    XNOR
};

std::string toString(GateType type);
