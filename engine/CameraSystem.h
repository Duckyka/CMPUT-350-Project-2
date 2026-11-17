#pragma once
#include <cmath>
#include <algorithm>
#include "CameraComponent.h"
#include "CollisionComponent.h"
#include <unordered_map>

#include "LocationComponent.h"
#include "MathUtil.h"
#include "SpriteComponent.h"
#include "SystemManager.h"
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>

namespace ECSEngine {

struct ContactInfo {         //record player sides touching
    bool left = false;
    bool right = false;
    bool ground = false;
    bool ceiling = false;
};

template <typename... Components>
class CameraSystem : public System<Components...> {
public:
    const std::string& GetName() const override {
        static std::string name = "CameraSystem";
        return name;
    }

tSystemReturn Run(EngineView<Components...>* engine) override {
    auto& entities = engine->GetEntityManager();
    auto& window = engine->GetWindowManager();
    auto& cameras = entities.template GetComponentStorage<CameraComponent>();
    auto& followers = entities.template GetComponentStorage<CameraFollower>();
    auto& locations = entities.template GetComponentStorage<LocationComponent>();
    auto& collisions = entities.template GetComponentStorage<CollisionComponent>();
    auto& shakes = entities.template GetComponentStorage<CameraShake>();

    const float width = static_cast<float>(window.GetWindow()->getSize().x); // get window size
    const float height = static_cast<float>(window.GetWindow()->getSize().y);

    for (size_t i = 0; i < cameras.mStorage.size(); ++i) { // iterate all camera

        if (i >= cameras.mValid.size()) {continue;} //Validations
        if (cameras.mValid[i] == 0) {continue;}
        if (i >= followers.mStorage.size() || i >= followers.mValid.size()) {continue;}
        if (followers.mValid[i] == 0) {continue;}
        auto& camera = cameras.mStorage[i];
        const EntityID player = followers.mStorage[i].entityID;

        if (player >= locations.mStorage.size()) {continue;} //if the player location valid
        if (player >= locations.mValid.size()) {continue;}
        if (locations.mValid[player] == 0) {continue;}

        if (camera.worldUnitsPerPixel <= 0.f) {camera.worldUnitsPerPixel = 1.f;} // camera scale
        window.SetWorldScale(camera.worldUnitsPerPixel);
        const float scale = camera.worldUnitsPerPixel;
        camera.hardExclusion = {{width * 0.10f, 0.f}, width * 0.80f, height}; //away from the outer 10%
        camera.softExclusion = {{width * 0.30f, 0.f}, width * 0.40f, height};  //within the middle 40%

        float playerX = locations.mStorage[player].loc.x; //player location
        if (player < collisions.mStorage.size() && player < collisions.mValid.size()) { ////
            if (collisions.mValid[player] != 0) { 
                const Rect& box = collisions.mStorage[player].collisionBox;
                playerX = playerX + box.topLeft.x + box.width / 2.0f;
            }
        }

        float distanceX = playerX - camera.position.x;//player-camera
        float screenDistance = distanceX / scale;  //distance to pixels
        float screenX = width / 2.0f + screenDistance; //player pos on screen
        const float hardLeft = camera.hardExclusion.topLeft.x;     //limits of camera zones
        const float hardRight = hardLeft + camera.hardExclusion.width;
        const float softLeft = camera.softExclusion.topLeft.x;
        const float softRight = softLeft + camera.softExclusion.width;
        float speed = camera.speed; //camera movement speed
        if (speed <= 0.0f) {speed = 0.15f;} //default speed if zero or negative
        if (speed > 1.0f) {speed = 1.0f;} // max speed limit

        if (screenX < hardLeft) { //// move camera when too far left
            float difference = screenX - softLeft;
            camera.position.x += difference * scale;}
        else if (screenX > hardRight) { //too far right
            float difference = screenX - softRight;
            camera.position.x += difference * scale;
        }
        else if (screenX < softLeft) { //slow left
            float difference = screenX - softLeft;
            float factor = speed;
            if (std::abs(difference) < 0.5f) {factor = 1.0f;} // stop moving if very close
            camera.position.x += difference * scale * factor; // move camera
        } else if (screenX > softRight) {  //slow right
            float difference = screenX - softRight; 
            float factor = speed;
            if (std::abs(difference) < 0.5f) {factor = 1.0f;}
            camera.position.x += difference * scale * factor;
        }

        Point2D center = camera.position; // copy position before shaking
        if (i < shakes.mStorage.size() && i < shakes.mValid.size()) { //if this camera has a shake component
            if (shakes.mValid[i] != 0) {
                auto& shake = shakes.mStorage[i]; // get camera shake component
                ContactInfo contacts;
                if (player < collisions.mStorage.size() && player < collisions.mValid.size()) { //if the player has a collision component
                    if (collisions.mValid[player] != 0) {   //collision flags
                        auto& collision = collisions.mStorage[player];
                        contacts.left = collision.onLeftWall;
                        contacts.right = collision.onRightWall;
                        contacts.ground = collision.onGround;
                        contacts.ceiling = collision.onCeiling;
                    }
                }
                ContactInfo previous = previousContacts[player];  //collision last frame
                bool newLeft = contacts.left && !previous.left;  //update player touching new surface
                bool newRight = contacts.right && !previous.right;
                bool newGround = contacts.ground && !previous.ground;
                bool newCeiling = contacts.ceiling && !previous.ceiling;
                previousContacts[player] = contacts;   // save current collisions for the next frame

                if (newLeft || newRight || newGround || newCeiling) { //shake
                    shake.offset.x = 0; //reset dir
                    shake.offset.y = 0;
                    if (newLeft) {shake.offset.x = -3.0f * scale;} //shake left
                    if (newRight) {shake.offset.x = 3.0f * scale;}
                    if (newGround) {shake.offset.y = 3.0f * scale;}
                    if (newCeiling) {shake.offset.y = -3.0f * scale;}
                    shake.frameLimit = 6; }
                if (shake.frameLimit > 0) { //continue shake
                    float direction = 1.0f;
                    if (shake.frameLimit % 2 != 0) {direction = -1.0f;}
                    center.x += direction * shake.offset.x;
                    center.y += direction * shake.offset.y;
                    shake.frameLimit--;
                }
            }
        }
    window.SetWorldCenter(center);} //update camera pos 
return kContinue;} 

private:
    std::unordered_map<EntityID, ContactInfo> previousContacts; // Store the previous collisions for each player
};

}  // namespace ECSEngine
