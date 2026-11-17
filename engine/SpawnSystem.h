#pragma once

#include <cmath>
#include "SpawnComponent.h"
#include "LocationComponent.h"
#include "CollisionComponent.h"
#include "SpriteComponent.h"
#include <cstdlib>

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
        auto& entityManager = engine->GetEntityManager(); // get the entity manager
        auto& spawnComponent = entityManager.template GetComponentStorage<SpawnComponent>(); //get spawn component
        const size_t count = spawnComponent.mStorage.size(); // get index length/size
        for (size_t i = 0; i < count; i++)
        {
            if (spawnComponent.mValid[i] != 0) //if valid
            {
                auto& current = spawnComponent.mStorage[i]; // get component to spawn entities 
                auto& locations = entityManager.template GetComponentStorage<LocationComponent>();  // Get location components.
                SpawnComponent spawnStorage = current; // copy before resize 
                const EntityID source = spawnStorage.entityID;   //get spawner's entity ID.
                auto& spriteManager = engine->GetSpriteManager(); // get the sprite manager
                const auto bounds = spriteManager.GetSprite(spawnStorage.spawnTextureID).getLocalBounds(); //star's sprite bounds
                const float width = bounds.size.x;
                const float height = bounds.size.y;
                
                if (current.totalSpawns <= 0) continue; // if there are no stars left to spawn, Skip
                if (current.timeToNextSpawn > 0) {current.timeToNextSpawn--;} //if positive timer, reduce timer
                if (current.timeToNextSpawn > 0) {continue;}  // if timer positive, skip
                if (source >= locations.mStorage.size() || source >= locations.mValid.size() || !locations.mValid[source]) {continue;}  //if valid spawn location
                const Point2D position = locations.mStorage[source].loc; // Get the position where the star will spawn.
                --current.totalSpawns;  // reduce remaining spawns by one
                current.timeToNextSpawn = spawnStorage.timeBetweenSpawns; // reset countdown for next star
                if (current.timeToNextSpawn < 0) {current.timeToNextSpawn = 0;} // negative spawn timer invalid

                int degrees = rand() % 360; // random direction
                float direction = degrees * 3.14159f / 180.0f; // degrees to radians
                const float speed = 5.f; // Set the star's movement speed.
                const EntityID star = entityManager.CreateEntity(spawnStorage.spawnType);  //create star entity
                entityManager.template AddComponent<LocationComponent>(star, LocationComponent{position}); // starting position
                float moveX = cos(direction) * speed; //horizontal movement
                float moveY = sin(direction) * speed; //vertical movement
                MovementComponent movement{{moveX, moveY}}; // create movement
                entityManager.template AddComponent<MovementComponent>(star, movement); // add movement to the star
                CollisionComponent Collision{}; // create collision for the star
                Collision.staticEntity = false; // star moves
                Collision.doesDamage = false;  // does not do damage
                Collision.collisionBox.topLeft.x = 0;  // set collision box size
                Collision.collisionBox.topLeft.y = 0;
                Collision.collisionBox.width = width;
                Collision.collisionBox.height = height;
                Collision.previousCollisionBox = {position, width, height}; // init position of collision box started
                entityManager.template AddComponent<CollisionComponent>(star, Collision);  // add the collision component.
                SpriteComponent sprite{};  // create sprite component
                sprite.textureID = spawnStorage.spawnTextureID; // set texture
                sprite.bounds = {{0.f, 0.f}, width, height};  // set sprite size
                sprite.worldSpace = true;  // draw the sprite in world coordinates
                entityManager.template AddComponent<SpriteComponent>(star, sprite);  // add sprite component
            }
        }
    return kContinue; } //engine
};

}  // namespace ECSEngine
