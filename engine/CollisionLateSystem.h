#pragma once

namespace ECSEngine {

template <typename... Components>
class CollisionLateSystem : public System<Components...> {
public:
    const std::string& GetName() const override {
        static std::string name("CollisionLateSystem");
        return name;
    }

    /// @brief For each valid collision component, moves the data from collisionBox into previousCollisionBox in preparation for
    ///        the next collision check during this current frame. 
    /// @param engine 
    /// @return tSystemReturn state
    tSystemReturn Run(EngineView<Components...>* engine) override
    {
        auto& componentStorage = engine->GetEntityManager().template GetComponentStorage<CollisionComponent>();
        auto& locations = engine->GetEntityManager().template GetComponentStorage<LocationComponent>();
        //copy the collision box and adds the entity's position to it.
        for (size_t i = 0; i < componentStorage.mStorage.size(); i++) //loops through all collision components
        {
            if (componentStorage.mValid[i] != 0 && i < locations.mValid.size() && locations.mValid[i] != 0) { //if both exist, update collision box
                componentStorage.mStorage[i].previousCollisionBox = componentStorage.mStorage[i].collisionBox; 
                componentStorage.mStorage[i].previousCollisionBox.topLeft.x += locations.mStorage[i].loc.x; 
                componentStorage.mStorage[i].previousCollisionBox.topLeft.y += locations.mStorage[i].loc.y; } 
        }
        return kContinue; }
};

}  // namespace ECSEngine
