#pragma once

#include <string>
#include "01-S_LogicBlockSupport.h"
#include "01-0_LogicBlockDefinition.h"

class LogicBlockInstance
{
public:
    LogicBlockInstance(LogicBlockDefinition* definition);

    std::string id;
    std::string instanceName;          // "AND_1", "U3"
    LogicBlockDefinition* definition;  // points to shared type info, not owned here
    Point position;
};
