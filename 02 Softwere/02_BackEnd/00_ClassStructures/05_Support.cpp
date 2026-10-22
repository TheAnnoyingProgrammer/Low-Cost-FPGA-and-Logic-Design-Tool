#include "05_Support.h"

std::string toString(GateType type)
{
    switch (type)
    {
    case GateType::NOT:  return "NOT";
    case GateType::AND:  return "AND";
    case GateType::OR:   return "OR";
    case GateType::XOR:  return "XOR";
    case GateType::NAND: return "NAND";
    case GateType::NOR:  return "NOR";
    case GateType::XNOR: return "XNOR";
    }
    return "UNKNOWN";
}
