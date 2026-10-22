#pragma once

#include <optional>
#include <string>
#include "02_BackEnd/00_ClassStructures/01_Library.h"

bool saveLibrary(const Library& library, const std::string& path);
std::optional<Library> loadLibrary(const std::string& path);
