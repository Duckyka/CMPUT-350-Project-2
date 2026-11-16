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

    /// @brief Creates a new entity of type Component and finds the earliest available spot in the list for this entity to reside in. 
    /// @param name 
    /// @return Returns the index in the mEntities list, AKA an entityID for the newly created entity. 
    [[nodiscard]] EntityID CreateEntity(const std::string& name)
    {
        size_t pos = 0;

        // If there is no space in the vector tEntity, add to the end
        if (std::find(mEntityTracker.begin(), mEntityTracker.end(), 0) == this->mEntityTracker.end())
        {
            mEntityNames.push_back(name);
            //pos = std::distance(mEntityNames.begin(), std::find(mEntityNames.begin(), mEntityNames.end(), (name)));
            pos = mEntityNames.size() - 1;
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

    /// @brief Removes the given entity and sets its state to invalid for future checks. Assumes the entity exists. 
    /// @param entity 
    void RemoveEntity(EntityID entity)
    {
        assert(mEntities.at(entity).mID != kNullComponent);
        mEntityTracker.at(entity) = 0;
        mEntities.at(entity).mID = kNullComponent;
        mEntityNames.at(entity).clear();
        (InvalidateComponent<Components>(entity), ...); // Invalidate
    }

    /// @brief Given an entityID and a component of type T, adds this component to the entity. Assumes the entity does not already 
    ///        have this component type. 
    /// @tparam T 
    /// @param entity 
    /// @param component 
    template <typename T>
    void AddComponent(EntityID entity, T component)
    {
        assert(mEntityTracker.at(entity) != 0 && !HasComponent<T>(entity)); //Valid Entity
        auto& storage = std::get<ComponentStorage<T>>(mComponents);

        if (storage.mStorage.size() <= entity) {
            storage.mStorage.insert(storage.mStorage.end(), entity + 1 - storage.mStorage.size(), component);
            storage.mValid.resize(entity + 1, false);}
        storage.mStorage[entity] = component;
        storage.mValid[entity] = true;
    }

    /// @brief Given an entityID, removes the component associated with that entity of type T. Assumes the entity is valid and has said component. 
    /// @tparam T 
    /// @param entity 
    template <typename T>
    void RemoveComponent(EntityID entity)
    {
        assert(mEntityTracker.at(entity) != 0 && HasComponent<T>(entity)); //Valid Entity
        auto& storage = std::get<ComponentStorage<T>>(mComponents);
        storage.mValid[entity] = false;
    }

    /// @brief Returns true or false depending on if the entity at entityID has the component type T. Assumes the entity is valid. 
    /// @tparam T 
    /// @param entity 
    /// @return Returns true if the entity has a component of type T. Returns false if not. 
    template <typename T>
    bool HasComponent(EntityID entity) const
    {
        assert(mEntityTracker.at(entity) != 0);
        const auto& storage = std::get<ComponentStorage<T>>(mComponents);
        return entity < storage.mValid.size() && storage.mValid[entity];
    }

    /// @brief Returns a reference to the component of type T given an entityID. Assumes the entity has said component type. 
    /// @tparam T 
    /// @param entity 
    /// @return Returns a reference to the component of type T. 
    template <typename T>
    T& GetComponent(EntityID entity) {
        assert(HasComponent<T>(entity));
        return std::get<ComponentStorage<T>>(mComponents).mStorage[entity];
    }

    /// @brief Returns a reference to the componentStorage of component type T. 
    /// @tparam T 
    /// @return ComponentStorage of type T. 
    template <typename T>
    ComponentStorage<T>& GetComponentStorage() {
        return std::get<ComponentStorage<T>>(mComponents);
    }

private:
    std::tuple<ComponentStorage<Components>...> mComponents;
    std::vector<Entity> mEntities;
    std::vector<std::string>mEntityNames;
    std::vector<bool> mEntityTracker;
    template<typename T> void InvalidateComponent(EntityID entity) {
        auto& storage = std::get<ComponentStorage<T>>(mComponents);
        if (entity < storage.mValid.size()) {storage.mValid[entity] = false; }
    }

public:
    using tEntity = std::vector<Entity>;
	using const_iterator = tEntity::const_iterator;
	const_iterator cbegin() const { return mEntities.begin(); }
	const_iterator cend() const { return mEntities.end(); }
	const_iterator begin() { return mEntities.begin(); }
	const_iterator end() { return mEntities.end(); }
};

}  // namespace ECSEngine
