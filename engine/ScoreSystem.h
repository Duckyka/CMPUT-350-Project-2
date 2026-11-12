#pragma once

namespace ECSEngine {

/// @brief For all valid ScoreComponents, takes the current score in data and sets each sprite to the correct digit respectively. 
/// @tparam ...Components 
template <typename... Components>
class ScoreSystem : public System<Components...> {
public:
    const std::string& GetName() const override {
        static std::string name("ScoreSystem");
        return name;
    }

    tSystemReturn Run(EngineView<Components...>* engine) override
    {
        auto componentStorage = engine->GetEntityManager().template GetComponentStorage<ScoreComponent>();
        for (int i = 0; i < componentStorage.mStorage.size(); i++)
        {
            if (componentStorage.mValid[i] != 0)
            {
                ScoreComponent scoreStorage = componentStorage.mStorage[i];
                int score = scoreStorage.score;
                int digit1 = ((int)(score/(pow(10,3-1)))%10);
                scoreStorage.digitID[2] = scoreStorage.digitTextureID[digit1];

                int digit2 = ((int)(score/(pow(10,2-1)))%10);
                scoreStorage.digitID[1] = scoreStorage.digitTextureID[digit2];

                int digit3 = ((int)(score/(pow(10,1-1)))%10);
                scoreStorage.digitID[0] = scoreStorage.digitTextureID[digit3];
            }
        }
        return kContinue;
    }
};

}  // namespace ECSEngine
