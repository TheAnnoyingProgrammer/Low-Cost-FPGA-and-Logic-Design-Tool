#pragma once

#include <string>

struct Point
{
    double x;
    double y;
};

enum class PinType
{
    Input,
    Output
};

struct Pin
{
    std::string name;
    PinType type;
};

struct Symbol
{
    double width;
    double height;
    std::string shapeType;
};
