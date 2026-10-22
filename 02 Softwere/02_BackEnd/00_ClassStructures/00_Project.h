#pragma once

#include <string>
#include <vector>
#include <memory>
#include "01-1_LogicBlockInstance.h"

struct Wire
{
    std::string fromInstanceId;
    int fromPinIndex;
    std::string toInstanceId;
    int toPinIndex;
};

class ProjectClass
{
public:
    ProjectClass();

    std::string name;
    std::string filePath;

    std::vector<std::unique_ptr<LogicBlockInstance>> blocks;
    std::vector<Wire> wires;

    bool save(const std::string& path);
    bool load(const std::string& path);
};