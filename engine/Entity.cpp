#include "Entity.h"

namespace ECSEngine {

EntityID Entity::GetID() const
{
    return mID;
}

} // namespace ECSEngine