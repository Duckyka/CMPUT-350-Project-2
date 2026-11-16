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
        auto& entityManager = engine->GetEntityManager(); // get the entity manager
        auto& componentStorage = entityManager.template GetComponentStorage<ScoreComponent>(); //get scores
        auto& sprites = entityManager.template GetComponentStorage<SpriteComponent>(); //get sprites
        for (size_t i = 0; i < componentStorage.mStorage.size(); i++) //loop through scores
        {
            if (componentStorage.mValid[i] != 0) //check valid score
            {
                ScoreComponent& scoreStorage = componentStorage.mStorage[i];
                int score = scoreStorage.score;  //get current score

                int digit1 = ((int)(score/(pow(10,3-1)))%10); //get hundreds digit
                EntityID id = scoreStorage.digitID[2]; //get sprite id 
                if (id < sprites.mValid.size() && sprites.mValid[id]) { //check valid sprite
                    sprites.mStorage[id].textureID = scoreStorage.digitTextureID[digit1]; //update digit
                }

                int digit2 = ((int)(score/(pow(10,2-1)))%10);  //get tens digit
                id = scoreStorage.digitID[1]; //get sprite id
                if (id < sprites.mValid.size() && sprites.mValid[id]) { //check valid sprite
                    sprites.mStorage[id].textureID =scoreStorage.digitTextureID[digit2]; //update digit
                }
                
                int digit3 = ((int)(score/(pow(10,1-1)))%10);  //get ones digit
                id = scoreStorage.digitID[0];//get sprite id
                if (id < sprites.mValid.size() && sprites.mValid[id]) { //check valid sprite
                    sprites.mStorage[id].textureID = scoreStorage.digitTextureID[digit3]; //update digit
                }
            }
        }
        return kContinue;
    }
};

}  // namespace ECSEngine
