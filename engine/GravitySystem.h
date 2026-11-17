#pragma once
#include <cmath>
#include "Keyboard.h"

namespace ECSEngine
{

/// @brief For each valid GravityComponent, applies downward gravity acceleration to the object with the corresponding gravityComponent if
///        said object is not already on the ground, determined by collisions. 
/// @tparam ...Components 
template <typename... Components>
class GravitySystem : public System<Components...> {
public:
	const std::string & GetName() const override
	{ static std::string name("GravitySystem"); return name; }
	
	tSystemReturn Run(EngineView<Components...> *engine) override
	{
		auto& entityManager = engine->GetEntityManager(); //get entity for components
		auto& gravStorage = entityManager.template GetComponentStorage<GravityComponent>(); //gravity
		auto& movements = entityManager.template GetComponentStorage<MovementComponent>();  //movement/velocity
		auto& collisions = entityManager.template GetComponentStorage<CollisionComponent>(); //touching wall/ground
		auto& inputs = entityManager.template GetComponentStorage<InputComponent>(); //player inputs

        for (size_t i = 0; i < gravStorage.mStorage.size(); i++) //loop through components
        {
            if (gravStorage.mValid[i] && i < movements.mValid.size() && movements.mValid[i]) //valid gavity n movement
            { 
				bool hasCollision = false;
				if (i < collisions.mValid.size()) { //check valid collision
					if (collisions.mValid[i]) {
						hasCollision = true; } }
				if (hasCollision && collisions.mStorage[i].onGround) continue;  //if on ground, skip

				auto& velocity = movements.mStorage[i].velocity; // get velocity accel
				float acceleration = std::abs(gravStorage.mStorage[i].acceleration.y);  //get gravity accel (positive)

				if (velocity.y >= 0.f) {acceleration *= 1.3f;} // if falling increase gravity
				if (i < inputs.mValid.size() && inputs.mValid[i] && inputs.mStorage[i].keydown[Key::Down]) { //if pressing down
					acceleration *= 1.5f; } //increase gravity
				velocity.y += acceleration; //add gravity to velocity
				if (hasCollision && (collisions.mStorage[i].onLeftWall || collisions.mStorage[i].onRightWall)) { //if entity is touching a wall  
					if (velocity.y > 1.1f) { //if falling fast
						velocity.y = 1.1f; //slow it down
					}
				}
            }
        }

		return kContinue;
	}
	
};


}



