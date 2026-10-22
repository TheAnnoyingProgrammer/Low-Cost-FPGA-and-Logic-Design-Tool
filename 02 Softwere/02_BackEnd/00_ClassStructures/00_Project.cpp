#include "00_Project.h"

namespace
{
const char idSeparator = ':';

std::string libraryPart(const std::string& definitionId)
{
    return definitionId.substr(0, definitionId.find(idSeparator));
}

std::string namePart(const std::string& definitionId)
{
    const auto separator = definitionId.find(idSeparator);
    return separator == std::string::npos ? "" : definitionId.substr(separator + 1);
}
}

Project::Project()
    : name("Untitled")
{
    addLibrary(Library::makePrimitive());
}

void Project::addLibrary(const Library& library)
{
    libraries.insert_or_assign(library.name, library);
}

const Definition* Project::findDefinition(const std::string& definitionId) const
{
    auto library = libraries.find(libraryPart(definitionId));
    return library == libraries.end() ? nullptr : library->second.find(namePart(definitionId));
}

Instance* Project::place(const std::string& definitionId, Point position)
{
    const Definition* definition = findDefinition(definitionId);
    if (!definition) return nullptr;
    instances.emplace_back(nextInstanceName(definition->name), definitionId, definition, position);
    return &instances.back();
}

void Project::addWire(Point start, Point end)
{
    wires.push_back({ start, end });
}

std::string Project::nextInstanceName(const std::string& definitionName) const
{
    int count = 0;
    for (const Instance& instance : instances)
        if (instance.definition && instance.definition->name == definitionName) ++count;
    return definitionName + "_" + std::to_string(count + 1);
}

void Project::resolveDefinitions()
{
    resolve(instances);
    for (auto& [libraryName, library] : libraries)
        for (auto& [definitionName, definition] : library.getDefinitions())
            resolve(definition.instances);
}

void Project::resolve(std::vector<Instance>& targets) const
{
    for (Instance& instance : targets)
        instance.definition = findDefinition(instance.definitionId);
}