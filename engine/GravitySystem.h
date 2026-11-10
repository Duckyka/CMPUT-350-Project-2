#pragma once

namespace ECSEngine
{

template <typename... Components>
class GravitySystem : public System<Components...> {
public:
	const std::string & GetName() const override
	{ static std::string name("GravitySystem"); return name; }
	
	tSystemReturn Run(EngineView<Components...> *engine) override
	{
		auto gravStorage = engine->GetEntityManager().template GetComponentStorage<GravityComponent>();
        for (int i = 0; i < gravStorage.mStorage.size(); i++)
        {
            if (gravStorage.mValid[i] != 0)
            {
                GravityComponent gravity = gravStorage.mStorage[i];
				auto collisionComponent = engine->GetEntityManager().template GetComponentStorage<CollisionComponent>();
                if (collisionComponent.mStorage[i].onGround == 1)
				{
					continue;
				}
				else
				{
					auto locationComponent = engine->GetEntityManager().template GetComponentStorage<MovementComponent>();
					MovementComponent locStorage = locationComponent.mStorage[i];
					locStorage.velocity.y -= gravity.acceleration.y;
				}
            }
        }

		return kContinue;
	}
	
};


}



