#include "01_JsonFile.h"
#include <fstream>

std::optional<Json> readJsonFile(const std::string& path)
{
    std::ifstream file(path);
    if (!file) return std::nullopt;

    Json json = Json::parse(file, nullptr, false);
    if (json.is_discarded()) return std::nullopt;
    return json;
}

bool writeJsonFile(const std::string& path, const Json& json)
{
    std::ofstream file(path);
    if (!file) return false;

    file << json.dump(4);
    return static_cast<bool>(file);
}
