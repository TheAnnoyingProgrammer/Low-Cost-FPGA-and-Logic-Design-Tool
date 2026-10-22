#include "02_Definition.h"

Definition Definition::makeGate(GateType type, int requestedInputCount)
{
    Definition gate;
    gate.name = toString(type);
    gate.gateType = type;
    const int inputCount = validInputCount(type, requestedInputCount);
    gate.buildGatePins(inputCount);
    gate.buildGateSymbol(inputCount);
    return gate;
}

Definition Definition::makeCustom(const std::string& name)
{
    Definition custom;
    custom.name = name;
    return custom;
}

bool Definition::isBuiltIn() const
{
    return gateType.has_value();
}

int Definition::validInputCount(GateType type, int requestedInputCount)
{
    if (type == GateType::NOT) return 1;
    return requestedInputCount < 2 ? 2 : requestedInputCount;
}

void Definition::buildGatePins(int inputCount)
{
    for (int i = 0; i < inputCount; ++i)
        pins.push_back({ std::string(1, char('A' + i)), PinType::Input, {} });
    pins.push_back({ "Y", PinType::Output, {} });
}

void Definition::buildGateSymbol(int inputCount)
{
    symbol = { 40.0, 20.0 * inputCount, name };
}