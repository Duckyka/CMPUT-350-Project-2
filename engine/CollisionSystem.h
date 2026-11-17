#pragma once
#include <algorithm>
#include <utility>
#include "ECSEngine.h"

namespace ECSEngine {

/// @brief Handles collisions between entities with CollisionComponents.
///        Resolves overlaps, updates collision flags, and handles star collection.
template <typename... Components>
class CollisionSystem : public System<Components...> {
public:
    const std::string& GetName() const override {
        static std::string name("CollisionSystem");
        return name;
    }
    
    /// @brief Checks collisions between valid entities each frame.
    ///        Resolves collisions between moving and static objects,
    ///        updates ground and wall contact flags, and handles star collection.
    /// @param engine Pointer to the engine containing the entity components.
    /// @return kContinue after processing collisions.
    tSystemReturn Run(EngineView<Components...>* engine) override
    { 
        auto& entities = engine->GetEntityManager(); 
        auto& collisions = entities.template GetComponentStorage<CollisionComponent>();
        auto& locations = entities.template GetComponentStorage<LocationComponent>();
        auto& movements = entities.template GetComponentStorage<MovementComponent>();
        auto& inputs = entities.template GetComponentStorage<InputComponent>();
        auto& scores = entities.template GetComponentStorage<ScoreComponent>();
        const size_t count = collisions.mStorage.size();

        auto valid = [&](size_t id) { // check if the entity has collision and location components
            if (id >= collisions.mStorage.size() || id >= collisions.mValid.size()) {
                return false;}
            if (id >= locations.mStorage.size() || id >= locations.mValid.size()) {
                return false;}
            if (collisions.mValid[id] == 0) {return false;}
            if (locations.mValid[id] == 0) {return false;}
        return true;};

        auto moving = [&](size_t id) { // check if the entity can move
            if (id >= movements.mStorage.size() || id >= movements.mValid.size()) {
                return false;}
            if (movements.mValid[id] == 0) {
                return false;}
        return true;};

        auto player = [&](size_t id) { // check if the entity is the player
            if (id >= inputs.mStorage.size() || id >= inputs.mValid.size()) {return false;}
            if (inputs.mValid[id] == 0) {
                return false;}
        return true;};

        auto bounds = [&](size_t id) { // get the collision box's position in the world
            Rect box = collisions.mStorage[id].collisionBox;
            Point2D position = locations.mStorage[id].loc;
            box.topLeft.x = box.topLeft.x + position.x;
            box.topLeft.y = box.topLeft.y + position.y;
        return box;};

        std::vector<bool> wasGrounded(count, false); // save the old ground and wall states
        std::vector<bool> wasOnWall(count, false);

        for (size_t i = 0; i < count; i++) {
            if (valid(i) && player(i)) {
                if (collisions.mStorage[i].onGround) {wasGrounded[i] = true;}
                if (collisions.mStorage[i].onLeftWall ||collisions.mStorage[i].onRightWall) {
                    wasOnWall[i] = true;
                }
            }
        }

        for (size_t i = 0; i < count; ++i) { // reset collision states for this frame
            if (!valid(i)) continue;
            auto& c = collisions.mStorage[i];
            c.onGround = c.onCeiling = c.onLeftWall = c.onRightWall = 0;
        }

        for (size_t i = 0; i < count; ++i) { // check if any two entities are touching
            if (!valid(i)) continue;
            for (size_t j = i + 1; j < count; ++j) {
                if (!valid(i) || !valid(j)) continue;
                const Rect a = bounds(i), b = bounds(j);
                float aLeft = a.topLeft.x;    //each rectangle's edges
                float aRight = aLeft + a.width;
                float aTop = a.topLeft.y;
                float aBottom = aTop + a.height;
                float bLeft = b.topLeft.x;
                float bRight = bLeft + b.width;
                float bTop = b.topLeft.y;
                float bBottom = bTop + b.height;
                float ox = std::min(aRight, bRight) - std::max(aLeft, bLeft); // overlap
                float oy = std::min(aBottom, bBottom) - std::max(aTop, bTop);
            
                if (ox < 0) {continue;}
                if (oy < 0) {continue;}
                if (ox == 0 && oy == 0) {continue;}
    
                const bool staticA = collisions.mStorage[i].staticEntity; // skip if both objects are static
                const bool staticB = collisions.mStorage[j].staticEntity;
                if (staticA && staticB) continue;
                const bool playerA = player(i); // collect stars
                const bool playerB = player(j);

                if (!staticA && !staticB && playerA != playerB) { // collect the star and update the score
                    size_t star;
                    if (playerA) { 
                        star = j;}
                    else {star = i;}
                    for (size_t k = 0; k < scores.mStorage.size(); ++k) {
                        if (k < scores.mValid.size() && scores.mValid[k]) {
                            ++scores.mStorage[k].score;
                            break;
                        }
                    }
                    engine->GetSoundManager().GetSound("star");
                    entities.RemoveEntity(star);
                    continue;
                }

                if (!staticA && !staticB) { // handle collisions between two moving objects
                    const bool axisX = ox < oy;
                    float difference = 0;
                    float overlap = 0;
                    if (axisX) {
                        float centerA = a.topLeft.x + a.width / 2;
                        float centerB = b.topLeft.x + b.width / 2;
                        difference = centerA - centerB;
                        overlap = ox;
                    }
                    else {
                        float centerA = a.topLeft.y + a.height / 2;
                        float centerB = b.topLeft.y + b.height / 2;
                        difference = centerA - centerB;
                        overlap = oy;
                    }
                    float direction = 1.0f;
                    if (difference < 0) {direction = -1.0f;}
                    float shift = overlap / 2 * direction;                
                    if (axisX) {
                        locations.mStorage[i].loc.x += shift;
                        locations.mStorage[j].loc.x -= shift;
                        if (moving(i) && moving(j)) {
                            std::swap(movements.mStorage[i].velocity.x, movements.mStorage[j].velocity.x);}} 
                    else {
                        locations.mStorage[i].loc.y += shift;
                        locations.mStorage[j].loc.y -= shift;
                        if (moving(i) && moving(j)) {
                            std::swap(movements.mStorage[i].velocity.y, movements.mStorage[j].velocity.y);
                        }    
                    }
                continue;}

                size_t d;  //moving and static object
                size_t s;
                if (staticA) { 
                    s = i;
                    d = j;
                }
                else {
                    s = j;
                    d = i;
                }

                auto& collision = collisions.mStorage[d];

                const Rect dynamic = bounds(d);
                const Rect solid = bounds(s);
                const Rect previous = collision.previousCollisionBox;

                float oldTop = previous.topLeft.y;
                float oldBottom = oldTop + previous.height;
                float oldLeft = previous.topLeft.x;
                float oldRight = oldLeft + previous.width;

                float wallTop = solid.topLeft.y;
                float wallBottom = wallTop + solid.height;
                float wallLeft = solid.topLeft.x;
                float wallRight = wallLeft + solid.width;

                bool above = oldBottom <= wallTop; // check where the object was last frame
                bool below = oldTop >= wallBottom;
                bool left = oldRight <= wallLeft;
                bool right = oldLeft >= wallRight;

                bool vertical = false; 	// decide if the collision is vertical or horizontal
                bool fromSide = left || right;
                bool fromTopOrBottom = above || below;
                
                if (fromSide && !fromTopOrBottom) {
                    vertical = false;}
                else if (fromTopOrBottom && !fromSide) {
                    vertical = true;}
                else if (fromSide && fromTopOrBottom) {
                    if (ox > 0 && oy <= ox) {vertical = true;}
                    else {vertical = false;}}
                else {
                    if (ox > 0 && oy <= ox) {vertical = true;}
                }
                
                const bool star = !player(d);

                if (vertical) { // handle landing on floors or hitting ceilings
                    float playerMiddle = dynamic.topLeft.y + dynamic.height / 2;
                    float platformMiddle = solid.topLeft.y + solid.height / 2;

                    bool floor = false;

                    if (above) {
                        floor = true; 
                    }
                    else if (!below && playerMiddle < platformMiddle) {
                        floor = true;
                    }

                    if (floor) {
                        locations.mStorage[d].loc.y = solid.topLeft.y - collision.collisionBox.topLeft.y - collision.collisionBox.height;
                        collision.onGround = 1;
                        if (player(d) && !wasGrounded[d]) {
                            engine->GetSoundManager().GetSound("land");
                            wasGrounded[d]=true;
                        }
                    } 
                    else {
                        locations.mStorage[d].loc.y = solid.topLeft.y + solid.height - collision.collisionBox.topLeft.y;collision.onCeiling = 1;
                    }

                    if (moving(d)) {
                        if (star) {
                            movements.mStorage[d].velocity.y *= -0.8f;
                        }
                        else {
                            movements.mStorage[d].velocity.y = 0;
                        }
                    }
                } 
                else { // handle hitting walls from either side
                    float playerCenter = dynamic.topLeft.x + dynamic.width / 2;
                    float wallCenter = solid.topLeft.x + solid.width / 2;
                    bool hitLeft = false;

                    if (left) {
                        hitLeft = true;
                    }
                    else if (!right && playerCenter < wallCenter) {
                        hitLeft = true;
                    }

                    if (hitLeft) {
                        locations.mStorage[d].loc.x = solid.topLeft.x - collision.collisionBox.topLeft.x - collision.collisionBox.width;
                        collision.onRightWall = 1;
                    } else {
                        locations.mStorage[d].loc.x = solid.topLeft.x + solid.width - collision.collisionBox.topLeft.x;
                        collision.onLeftWall = 1;
                    }

                    if (player(d) && moving(d)) {
                        if (movements.mStorage[d].velocity.y > 0) {
                            if (wasOnWall[d] == false) {
                                engine->GetSoundManager().GetSound("wall");
                                wasOnWall[d] = true;
                            }
                        }
                    }

                    if (moving(d)) {
                        if (star) {
                            movements.mStorage[d].velocity.x *= -0.8f;
                        } else {
                            movements.mStorage[d].velocity.x = 0;
                        }
                    }
                }
            }
        }

        return kContinue;
    }
};

}  // namespace ECSEngine
