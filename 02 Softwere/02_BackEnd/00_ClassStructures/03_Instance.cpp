#include "03_Instance.h"

Instance::Instance(const std::string& name, const std::string& definitionId, const Definition* definition, Point position)
    : name(name)
    , definitionId(definitionId)
    , definition(definition)
    , position(position)
{
}
