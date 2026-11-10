#pragma once

namespace ECSEngine {

template <typename... Components>
class SpawnSystem : public System<Components...> {
public:
    const std::string& GetName() const override {
        static std::string name("SpawnSystem");
        return name;
    }

    tSystemReturn Run(EngineView<Components...>* engine) override
    {
        auto spawnComponent = engine->GetEntityManager().template GetComponentStorage<SpawnComponent>();
        for (int i = 0; i < spawnComponent.mStorage.size(); i++)
        {
            if (spawnComponent.mValid[i] != 0)
            {
                // SpawnComponent spawnStorage = spawnComponent.mStorage[i];
                // spawnStorage.
            }
        }

        return kContinue;
    }
};

}  // namespace ECSEngine
