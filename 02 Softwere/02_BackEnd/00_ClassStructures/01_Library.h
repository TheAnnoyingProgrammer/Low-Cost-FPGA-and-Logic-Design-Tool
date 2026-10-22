#pragma once

#include <map>
#include <string>
#include "02_Definition.h"

class Library
{
public:
    static Library makePrimitive();

    explicit Library(const std::string& name);

    std::string name;

    void add(const Definition& definition);
    const Definition* find(const std::string& definitionName) const;
    const std::map<std::string, Definition>& getDefinitions() const;
    std::map<std::string, Definition>& getDefinitions();

private:
    std::map<std::string, Definition> definitions;
};