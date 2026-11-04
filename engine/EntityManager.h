#pragma once

#include <cassert>
#include <iostream>
#include <limits>
#include <type_traits>
#include <tuple>
#include <vector>

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
            mEntities.push_back(name);
            pos = std::distance(mEntities.begin(), std::find(mEntities.begin(), mEntities.end(), (name)));
            mEntityTracker.push_back(1);
        }
        else    // Else there is room from a removed entity, reuse the first space available
        {
            pos = std::distance(mEntityTracker.begin(), std::find(mEntityTracker.begin(), mEntityTracker.end(), 0));
            mEntities.at(pos) = name;
            mEntityTracker.at(pos) = 1;
        }

        return pos;
    }

    void RemoveEntity(EntityID entity)
    {
        mEntityTracker.at(entity) = 0;
        mEntities.at(entity) = kNullComponent;
    }

    template <typename T>
    void AddComponent(EntityID entity, T component)
    {

    }

    template <typename T>
    void RemoveComponent(EntityID entity)
    {

    }

    template <typename T>
    bool HasComponent(EntityID entity) const
    {
        if (mEntityTracker.at(entity) != 0)
        {
            //if (mComponents)
            return true;
        }
        else
        {
            return false;
        }
    }

    template <typename T>
    T& GetComponent(EntityID entity) {
        // Re-write code
        static T dummy;
        return dummy;
    }

    template <typename T>
    ComponentStorage<T>& GetComponentStorage() {
        // Re-write code
        static ComponentStorage<T> dummy;
        return dummy;
    }

private:
    std::tuple<std::vector<ComponentTypes>...> mComponents;
    std::vector<std::string> mEntities;
    std::vector<bool> mEntityTracker;

    using tEntity = std::vector<Entity>;
	using const_iterator = tEntity::const_iterator;
	const_iterator cbegin() const { return mEntities.begin(); }
	const_iterator cend() const { return mEntities.end(); }
	const_iterator begin() { return mEntities.begin(); }
	const_iterator end() { return mEntities.end(); }
};

}  // namespace ECSEngine
