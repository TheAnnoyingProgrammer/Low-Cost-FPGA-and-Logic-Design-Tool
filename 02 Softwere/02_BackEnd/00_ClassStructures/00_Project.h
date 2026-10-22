#pragma once

#include <map>
#include <string>
#include <vector>
#include "01_Library.h"
#include "03_Instance.h"
#include "04_Wire.h"

class Project
{
public:
    Project();

    std::string name;
    std::string filePath;

    std::vector<std::string> libraryPaths;
    std::map<std::string, Library> libraries;
    std::vector<Instance> instances;
    std::vector<Wire> wires;

    void addLibrary(const Library& library);
    const Definition* findDefinition(const std::string& definitionId) const;
    Instance* place(const std::string& definitionId, Point position);
    void addWire(Point start, Point end);
    void resolveDefinitions();

private:
    void resolve(std::vector<Instance>& targets) const;
    std::string nextInstanceName(const std::string& definitionName) const;
};