#pragma once

#include <optional>
#include <string>
#include <vector>
#include "03_Instance.h"
#include "04_Wire.h"
#include "05_Support.h"

class Definition
{
public:
    static Definition makeGate(GateType type, int requestedInputCount = 2);
    static Definition makeCustom(const std::string& name);

    std::string name;
    std::vector<Pin> pins;
    Symbol symbol;

    std::optional<GateType> gateType;
    std::vector<Instance> instances;
    std::vector<Wire> wires;

    bool isBuiltIn() const;

private:
    static int validInputCount(GateType type, int requestedInputCount);
    void buildGatePins(int inputCount);
    void buildGateSymbol(int inputCount);
};