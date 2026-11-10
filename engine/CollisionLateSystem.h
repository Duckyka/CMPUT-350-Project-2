#pragma once

namespace ECSEngine {

template <typename... Components>
class CollisionLateSystem : public System<Components...> {
public:
    const std::string& GetName() const override {
        static std::string name("CollisionLateSystem");
        return name;
    }

    tSystemReturn Run(EngineView<Components...>* engine) override
    {
        auto componentStorage = engine->GetEntityManager().template GetComponentStorage<CollisionComponent>();
        for (int i = 0; i < componentStorage.mStorage.size(); i++)
        {
            if (componentStorage.mValid[i] != 0)
            {
                componentStorage.mStorage[i].previousCollisionBox = componentStorage.mStorage[i].collisionBox;
            }
        }
        return kContinue;
    }
};

}  // namespace ECSEngine
