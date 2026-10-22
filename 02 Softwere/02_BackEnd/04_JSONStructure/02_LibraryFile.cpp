#include "02_LibraryFile.h"
#include "01_JsonFile.h"

namespace
{
    Json libraryToJson(const Library& library)
    {
        Json definitions = Json::array();
        for (const auto& [name, definition] : library.getDefinitions())
            definitions.push_back(toJson(definition));
        return { {"name", library.name}, {"definitions", definitions} };
    }

    Library libraryFromJson(const Json& json)
    {
        Library library(json.at("name"));
        for (const Json& definition : json.value("definitions", Json::array()))
            library.add(definitionFromJson(definition));
        return library;
    }
}

bool saveLibrary(const Library& library, const std::string& path)
{
    return writeJsonFile(path, libraryToJson(library));
}

std::optional<Library> loadLibrary(const std::string& path)
{
    std::optional<Json> json = readJsonFile(path);
    if (!json) return std::nullopt;

    try
    {
        return libraryFromJson(*json);
    }
    catch (const Json::exception&)
    {
        return std::nullopt;
    }
}
