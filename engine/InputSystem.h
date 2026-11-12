#pragma once

#include "Keyboard.h"

namespace ECSEngine {

const float kMaxRunVelocity = 15;
const float kDashVelocity = 50;
const float kJumpVelocity = 25;
const float kGroundRunAcceleration = 5;
const float kAirRunAcceleration = 4;
const float kDashInterval = 25;
bool canJump = false;

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
        auto componentStorage = engine->GetEntityManager().template GetComponentStorage<InputComponent>();
        for (int i = 0; i < componentStorage.mStorage.size(); i++)
        {
            if (componentStorage.mValid[i] != 0)
            {
                std::bitset<sf::Keyboard::ScancodeCount> keydown = componentStorage.mStorage[i].keydown;
                std::bitset<sf::Keyboard::ScancodeCount> lastKeydown = componentStorage.mStorage[i].lastKeydown;
                EntityID entity = componentStorage.mStorage[i].entity;

                auto locationComponent = engine->GetEntityManager().template GetComponentStorage<MovementComponent>();
                MovementComponent mMovementComp = locationComponent.mStorage[entity];
                auto collisionComponent = engine->GetEntityManager().template GetComponentStorage<CollisionComponent>();
                CollisionComponent mCollisionComp = collisionComponent.mStorage[entity];
                if (keydown[Key::Left] == true && keydown[Key::Right] == false)
                {
                    if (mCollisionComp.onGround)
                    {
                        mMovementComp.velocity.x -= kGroundRunAcceleration;
                    }
                    else
                    {
                        mMovementComp.velocity.x -= kAirRunAcceleration;
                    }
                    if (mMovementComp.velocity.x < -kMaxRunVelocity) {mMovementComp.velocity.x = -kMaxRunVelocity;}
                }
                else if (keydown[Key::Right] == true && keydown[Key::Left] == false)
                {
                    if (mCollisionComp.onGround)
                    {
                        mMovementComp.velocity.x += kGroundRunAcceleration;
                    }
                    else
                    {
                        mMovementComp.velocity.x += kAirRunAcceleration;
                    }
                    if (mMovementComp.velocity.x > kMaxRunVelocity) {mMovementComp.velocity.x = kMaxRunVelocity;}
                }
                else if (keydown[Key::Right] == true && keydown[Key::Left] == true)
                {
                    if (mCollisionComp.onGround)
                    {
                        mMovementComp.velocity.x -= kGroundRunAcceleration;
                    }
                    else
                    {
                        mMovementComp.velocity.x -= kAirRunAcceleration;
                    }
                    if (mMovementComp.velocity.x < -kMaxRunVelocity) {mMovementComp.velocity.x = -kMaxRunVelocity;}
                }
                else if (keydown[Key::Right] == false && keydown[Key::Left] == false && mMovementComp.velocity.x != 0)
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

                if (canJump == false && mCollisionComp.onGround)
                {
                    canJump = true;
                }

                if (keydown[Key::Z] == true && mCollisionComp.onGround)
                {
                    mMovementComp.velocity.y += 2;
                }
                else if (keydown[Key::Z] == true && lastKeydown[Key::Z] == true && canJump)
                {
                    mMovementComp.velocity.y += 2;

                    if (mMovementComp.velocity.y > kJumpVelocity)
                    {
                        mMovementComp.velocity.y = kJumpVelocity;
                        canJump = false;
                    }
                }

                if (keydown[Key::Z] == true && (mCollisionComp.onLeftWall || mCollisionComp.onRightWall))
                {
                    mMovementComp.velocity.y = kJumpVelocity;
                }
                
                if (keydown[Key::C])
                {
                    if (keydown[Key::Left] == true && keydown[Key::Right] == false)
                    {
                        for (int i = 0; i < kDashInterval; i++)
                        {
                            mMovementComp.velocity.x = -kDashVelocity;
                        }
                        mMovementComp.velocity.x = -kMaxRunVelocity;
                    }
                    else if (keydown[Key::Left] == false && keydown[Key::Right] == true)
                    {
                        for (int i = 0; i < kDashInterval; i++)
                        {
                            mMovementComp.velocity.x = kDashVelocity;
                        }
                        mMovementComp.velocity.x = kMaxRunVelocity;
                    }
                    else
                    {
                        for (int i = 0; i < kDashInterval; i++)
                        {
                            mMovementComp.velocity.x = -kDashVelocity;
                        }
                        mMovementComp.velocity.x = 0;
                    }
                }
            }
        }
        return kContinue;
    }
};

}  // namespace ECSEngine
