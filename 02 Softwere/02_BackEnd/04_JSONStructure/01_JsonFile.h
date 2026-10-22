#pragma once

#include <optional>
#include <string>
#include "00_JsonConvert.h"

std::optional<Json> readJsonFile(const std::string& path);
bool writeJsonFile(const std::string& path, const Json& json);
