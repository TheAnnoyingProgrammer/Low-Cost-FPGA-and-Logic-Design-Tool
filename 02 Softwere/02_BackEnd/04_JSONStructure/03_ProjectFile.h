#pragma once

#include <string>
#include "02_BackEnd/00_ClassStructures/00_Project.h"

bool saveProject(Project& project, const std::string& path);
bool loadProject(Project& project, const std::string& path);
bool importLibrary(Project& project, const std::string& libraryPath);
