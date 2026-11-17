#pragma once

#include "Keyboard.h"

namespace ECSEngine {

const float kMaxRunVelocity = 15;
const float kDashVelocity = 50;
const float kJumpVelocity = 25;
const float kGroundRunAcceleration = 5;
const float kAirRunAcceleration = 4;

/// @brief For each valid InputComponent, gets a reference of the current and last keyDown bitmaps. 
///        If left or right arrows are pressed, moves along the horizontal axis accordingly. 
///        If z is pressed and the player is on the ground, jumps accordingly. Variable jump height is determined by the duration
///        the user holds the z key. 
///        If z is pressed while along a wall, the player jumps up the wall. 
///        If c is pressed, the player dashes in the direction the player is moving. 
///                     If both arrow keys are held down at the same time, the player will move left. 
/// @tparam ...Components 
template <typename... Components>
class InputSystem : public System<Components...> {
public:
    const std::string& GetName() const override {
        static std::string name("InputSystem");
        return name;
    }

    tSystemReturn Run(EngineView<Components...>* engine) override
    {
        auto& componentStorage = engine->GetEntityManager().template GetComponentStorage<InputComponent>();
        for (size_t i = 0; i < componentStorage.mStorage.size(); i++)
        {
            if (componentStorage.mValid[i] != 0)
            {
                const auto& keydown = componentStorage.mStorage[i].keydown;
                const auto& lastKeydown = componentStorage.mStorage[i].lastKeydown;
                EntityID entity = componentStorage.mStorage[i].entity;
                auto& movements = engine->GetEntityManager().template GetComponentStorage<MovementComponent>();
                auto& collisions = engine->GetEntityManager().template GetComponentStorage<CollisionComponent>();

                if (entity >= movements.mValid.size() || !movements.mValid[entity] || entity >= collisions.mValid.size() || !collisions.mValid[entity])  continue;

                auto& mMovementComp = movements.mStorage[entity];
                const auto& mCollisionComp = collisions.mStorage[entity];    

                const auto& dashTime = componentStorage.mStorage[i].lastDash;  
                bool dashing = false;
                if (dashTime != std::chrono::steady_clock::time_point{}) {
                    auto timeSinceDash = std::chrono::steady_clock::now() - dashTime;
                    if (timeSinceDash < std::chrono::milliseconds(150)) {
                        dashing = true;
                    }
                }       

                if (!dashing && (keydown[Key::Left] == true && keydown[Key::Right] == false))
                {
                    if (mCollisionComp.onGround)
                    {
                        mMovementComp.velocity.x -= kGroundRunAcceleration;
                    }
                    else
                    {
                        mMovementComp.velocity.x -= kAirRunAcceleration;
                    }
                    if (!dashing && mMovementComp.velocity.x < -kMaxRunVelocity) {mMovementComp.velocity.x = -kMaxRunVelocity;}
                }
                else if (!dashing &&(keydown[Key::Right] == true && keydown[Key::Left] == false))
                {
                    if (mCollisionComp.onGround)
                    {
                        mMovementComp.velocity.x += kGroundRunAcceleration;
                    }
                    else
                    {
                        mMovementComp.velocity.x += kAirRunAcceleration;
                    }
                    if (!dashing && mMovementComp.velocity.x > kMaxRunVelocity) {mMovementComp.velocity.x = kMaxRunVelocity;}
                }
                else if (!dashing && (keydown[Key::Right] == true && keydown[Key::Left] == true))
                {
                    if (mCollisionComp.onGround)
                    {
                        mMovementComp.velocity.x -= kGroundRunAcceleration;
                    }
                    else
                    {
                        mMovementComp.velocity.x -= kAirRunAcceleration;
                    }
                    if (!dashing && mMovementComp.velocity.x < -kMaxRunVelocity) {mMovementComp.velocity.x = -kMaxRunVelocity;}
                }
                else if (!dashing &&(keydown[Key::Right] == false && keydown[Key::Left] == false && mMovementComp.velocity.x != 0))
                {
                    if (mMovementComp.velocity.x > 0)
                    {
                        if (mCollisionComp.onGround)
                        {
                            mMovementComp.velocity.x -= kGroundRunAcceleration;
                        }
                        else
                        {
                            mMovementComp.velocity.x -= kAirRunAcceleration;
                        }
                        if (mMovementComp.velocity.x < 0) {mMovementComp.velocity.x = 0;}
                    }
                    else
                    {
                        if (mCollisionComp.onGround)
                        {
                            mMovementComp.velocity.x += kGroundRunAcceleration;
                        }
                        else
                        {
                            mMovementComp.velocity.x += kAirRunAcceleration;
                        }
                        if (mMovementComp.velocity.x > 0) {mMovementComp.velocity.x = 0;}
                    }
                }

                if (keydown[Key::Z] && !lastKeydown[Key::Z]) { 
                    if (mCollisionComp.onGround) {
                        mMovementComp.velocity.y = -kJumpVelocity;
                        engine->GetSoundManager().GetSound("jump");
                    }
                    else if (mMovementComp.velocity.y > 0.f && (mCollisionComp.onLeftWall || mCollisionComp.onRightWall)) {
                        mMovementComp.velocity.y = -kJumpVelocity;
                        if (mCollisionComp.onLeftWall) {
                            mMovementComp.velocity.x = kMaxRunVelocity;}
                        else {
                            mMovementComp.velocity.x = -kMaxRunVelocity;}
                        engine->GetSoundManager().GetSound("jump");
                    }
                }

                if (!keydown[Key::Z] && lastKeydown[Key::Z] && mMovementComp.velocity.y < 0.f) {
                    mMovementComp.velocity.y = 0.f;
                }
        
                if (keydown[Key::C] && !lastKeydown[Key::C]) {
                    auto& lastDash = componentStorage.mStorage[i].lastDash;
                    auto now = std::chrono::steady_clock::now();
                    bool canDash = lastDash == std::chrono::steady_clock::time_point{};
                    if (!canDash) {
                        canDash = now - lastDash >= std::chrono::seconds(1);}
                    if (canDash) {
                        float direction = 0;
                        if (keydown[Key::Left]) {
                            direction = -1;}
                        else if (keydown[Key::Right]) {
                            direction = 1;}
                        else if (mMovementComp.velocity.x < 0) {
                            direction = -1;}
                        else if (mMovementComp.velocity.x > 0) {
                            direction = 1;}
                        if (direction != 0) {
                            mMovementComp.velocity.x = direction * kDashVelocity;
                            lastDash = now;
                        }
                    }
                }

            }
        }
        return kContinue;
    }
};

}  // namespace ECSEngine
