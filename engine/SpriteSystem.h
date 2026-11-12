#pragma once

#include <bitset>
#include <unordered_map>

#include "CameraComponent.h"
#include "EntityManager.h"
#include "MathUtil.h"
#include "SpriteManager.h"
#include "SystemManager.h"
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>

namespace ECSEngine {

/// @brief For all valid SpriteComponents, gets the corresponding sprite texture based off of the spriteComponent and draws this to the window. 
/// @tparam ...Components 
template <typename... Components>
class SpriteSystem : public System<Components...> {
public:
    const std::string& GetName() const override {
        static std::string name("SpriteSystem");
        return name;
    }

    tSystemReturn Run(EngineView<Components...>* engine) override
    {
        auto spriteStorage = engine->GetEntityManager().template GetComponentStorage<SpriteComponent>();
        for (int i = 0; i < spriteStorage.mStorage.size(); i++)
        {
            if (spriteStorage.mValid[i] != 0)
            {
                SpriteComponent sprite = spriteStorage.mStorage[i];
                sf::Sprite spriteObject = engine->GetSpriteManager().GetSprite(sprite.textureID);
                engine->GetWindowManager().GetWindow()->draw(spriteObject);
            }
        }

        return kContinue;
    }
};

}  // namespace ECSEngine
