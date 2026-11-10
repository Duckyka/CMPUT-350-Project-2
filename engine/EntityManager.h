#pragma once

#include <cassert>
#include <iostream>
#include <limits>
#include <type_traits>
#include <tuple>
#include <vector>
#include <algorithm>

#include "Component.h"
#include "Entity.h"

using EntityID = size_t;

namespace ECSEngine {

template <typename... Components>
class EntityManager {
public:
    static constexpr size_t kNullComponent = std::numeric_limits<size_t>::max();
    [[nodiscard]] EntityID CreateEntity(const std::string& name)
    {
        size_t pos = 0;

        // If there is no space in the vector tEntity, add to the end
        if (std::find(mEntityTracker.begin(), mEntityTracker.end(), 0) == this->mEntityTracker.end())
        {
            mEntityNames.push_back(name);
            pos = std::distance(mEntityNames.begin(), std::find(mEntityNames.begin(), mEntityNames.end(), (name)));
            mEntities.push_back(Entity(pos, name));
            mEntityTracker.push_back(1);
        }
        else    // Else there is room from a removed entity, reuse the first space available
        {
            pos = std::distance(mEntityTracker.begin(), std::find(mEntityTracker.begin(), mEntityTracker.end(), 0));
            mEntityNames.at(pos) = name;
            mEntities.at(pos) = Entity(pos, name);
            mEntityTracker.at(pos) = 1;
        }

        return pos;
    }

    void RemoveEntity(EntityID entity)
    {
        mEntityTracker.at(entity) = 0;
        mEntities.at(entity).mID = kNullComponent;
        mEntityNames.at(entity) = kNullComponent;
    }

    template <typename T>
    void AddComponent(EntityID entity, T component)
    {
        assert(mEntityTracker.at(entity) != 0 && !HasComponent<T>(entity)); //Valid Entity
        std::get<T>(mComponents)->mStorage[entity] = component;
    }

    template <typename T>
    void RemoveComponent(EntityID entity)
    {
        assert(mEntityTracker.at(entity) != 0 && HasComponent<T>(entity)); //Valid Entity
        //std::get<T>(mComponents)[entity]->mFreeList.push_back(entity);
        std::get<T>(mComponents)->mValid[entity] = 0;
        std::get<T>(mComponents)->mStorage[entity] = kNullComponent;
    }

    template <typename T>
    bool HasComponent(EntityID entity) const
    {
        assert(mEntityTracker.at(entity) != 0); //Valid Entity
        if (std::get<T>(mComponents)->mStorage[entity] != kNullComponent)
        {
            if (std::get<T>(mComponents)->mValid[entity] != 0)
            {
                return true;
            }
            else { return false; }
        }
        else
        {
            return false;
        }
    }

    template <typename T>
    T& GetComponent(EntityID entity) {
        // Re-write code
        assert(HasComponent<T>(entity));
        static T component = std::get<T>(mComponents)->mStorage[entity];
        return component;
    }

    template <typename T>
    ComponentStorage<T>& GetComponentStorage() {
        // Re-write code
        static ComponentStorage<T> componentStorage = std::get<ComponentStorage<T>>(mComponents);
        return componentStorage;
    }

private:
    std::tuple<ComponentStorage<Components>...> mComponents;
    std::vector<Entity> mEntities;
    std::vector<std::string>mEntityNames;
    std::vector<bool> mEntityTracker;

    using tEntity = std::vector<Entity>;
	using const_iterator = tEntity::const_iterator;
	const_iterator cbegin() const { return mEntities.begin(); }
	const_iterator cend() const { return mEntities.end(); }
	const_iterator begin() { return mEntities.begin(); }
	const_iterator end() { return mEntities.end(); }
};

}  // namespace ECSEngine
