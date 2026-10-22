#include "03_ProjectFile.h"
#include <algorithm>
#include <filesystem>
#include "01_JsonFile.h"
#include "02_LibraryFile.h"

namespace
{
    std::string resolveLibraryPath(const Project& project, const std::string& libraryPath)
    {
        const std::filesystem::path path(libraryPath);
        if (path.is_absolute() || project.filePath.empty()) return libraryPath;
        return (std::filesystem::path(project.filePath).parent_path() / path).string();
    }

    void rememberLibraryPath(Project& project, const std::string& libraryPath)
    {
        auto& paths = project.libraryPaths;
        if (std::find(paths.begin(), paths.end(), libraryPath) == paths.end())
            paths.push_back(libraryPath);
    }

    Json projectToJson(const Project& project)
    {
        return { {"name", project.name},
                 {"libraries", project.libraryPaths},
                 {"instances", toJsonArray(project.instances)},
                 {"wires", toJsonArray(project.wires)} };
    }

    void fillProjectFromJson(Project& project, const Json& json)
    {
        project.name = json.value("name", "Untitled");
        for (const std::string& libraryPath : json.value("libraries", std::vector<std::string>{}))
            importLibrary(project, libraryPath);
        project.instances = fromJsonArray<Instance>(json.value("instances", Json::array()), instanceFromJson);
        project.wires = fromJsonArray<Wire>(json.value("wires", Json::array()), wireFromJson);
    }
}

bool saveProject(Project& project, const std::string& path)
{
    if (!writeJsonFile(path, projectToJson(project))) return false;
    project.filePath = path;
    return true;
}

bool loadProject(Project& project, const std::string& path)
{
    std::optional<Json> json = readJsonFile(path);
    if (!json) return false;

    Project loaded;
    loaded.filePath = path;
    try
    {
        fillProjectFromJson(loaded, *json);
    }
    catch (const Json::exception&)
    {
        return false;
    }

    project = std::move(loaded);
    project.resolveDefinitions();
    return true;
}

bool importLibrary(Project& project, const std::string& libraryPath)
{
    rememberLibraryPath(project, libraryPath);

    std::optional<Library> library = loadLibrary(resolveLibraryPath(project, libraryPath));
    if (!library) return false;

    project.addLibrary(*library);
    project.resolveDefinitions();
    return true;
}
