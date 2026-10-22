#include "00_Project.h"

ProjectClass::ProjectClass()
    : name("Untitled")
    , filePath("")
{
}

bool ProjectClass::save(const std::string& path)
{
    // TODO: serialize blocks + wires to file
    filePath = path;
    return true;
}

bool ProjectClass::load(const std::string& path)
{
    // TODO: parse file into blocks + wires
    filePath = path;
    return true;
}
