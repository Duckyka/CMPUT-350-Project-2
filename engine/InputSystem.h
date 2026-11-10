#pragma once

#include "Keyboard.h"

namespace ECSEngine {

const float kMaxRunVelocity = 15;
const float kDashVelocity = 50;
const float kJumpVelocity = 25;
const float kGroundRunAcceleration = 5;
const float kAirRunAcceleration = 4;
const float kDashInterval = 25;

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

                auto locationComponent = engine->GetEntityManager().template GetComponentStorage<LocationComponent>();
                MovementComponent mMovementComp = locationComponent.mStorage[entity];
                // if
                // mMovementComp.velocity

            }
        }
        return kContinue;
    }
};

}  // namespace ECSEngine
