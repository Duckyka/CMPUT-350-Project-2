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
        auto& entityManager = engine->GetEntityManager(); // get the entity manager
        auto& spriteStorage = entityManager.template GetComponentStorage<SpriteComponent>();
        auto& locations = entityManager.template  GetComponentStorage<LocationComponent>();
        auto& window = engine->GetWindowManager();
        
        for (size_t i = 0; i < spriteStorage.mStorage.size(); i++)
        {
            if (spriteStorage.mValid[i] != 0) {
                SpriteComponent& sprite = spriteStorage.mStorage[i];
                sf::Sprite spriteObject = engine->GetSpriteManager().GetSprite(sprite.textureID);
                Rect bounds = sprite.bounds;  //copy box
                if (i < locations.mValid.size() && locations.mValid[i]) {  //if location valid
                    bounds.topLeft.x += locations.mStorage[i].loc.x;    //add x coord
                    bounds.topLeft.y += locations.mStorage[i].loc.y;    //add y coord
                }
                if (sprite.worldSpace) { //if in world
                    bounds = window.WorldToWindow(bounds); //convert coords to world
                }
                const auto local = spriteObject.getLocalBounds(); //get sprite bounds
                if (local.size.x <= 0.f || local.size.y <= 0.f) continue; //if invalid, skip
                spriteObject.setPosition(sf::Vector2f(bounds.topLeft.x, bounds.topLeft.y)); //set pos of sprite
                spriteObject.setScale(sf::Vector2f(bounds.width / local.size.x, bounds.height / local.size.y)); //scale sprite
                window.GetWindow()->draw(spriteObject); //draw sprite
            } } 
    return kContinue; }
};

}  // namespace ECSEngine
