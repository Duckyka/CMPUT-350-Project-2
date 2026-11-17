#pragma once

namespace ECSEngine {
/// @brief Updates the location of entities based on their movement velocity.
/// @tparam ...Components
template <typename... Components>
class MovementSystem : public System<Components...> {
public:
    const std::string& GetName() const override {
        static std::string name("MovementSystem");
        return name;
    }
    /// @brief Adds each valid entity's velocity to its current location.
    tSystemReturn Run(EngineView<Components...>* engine) override {  
        auto& entityManager = engine->GetEntityManager(); // get the entity manager
        auto& locations = entityManager.template GetComponentStorage<LocationComponent>(); //get location compoment
        auto& movements = entityManager.template GetComponentStorage<MovementComponent>(); //get movement component
        for (size_t i = 0; i < locations.mStorage.size(); i++) {
        if (i >= movements.mStorage.size() || i >= locations.mValid.size() || i >= movements.mValid.size()) { //index out of bounds
            continue; } //skip
        if (!locations.mValid[i] || !movements.mValid[i]) { //if location or movement is invalid.
            continue; } //skip
        locations.mStorage[i].loc.x += movements.mStorage[i].velocity.x;    //add x
        locations.mStorage[i].loc.y += movements.mStorage[i].velocity.y; }  //add y

    return kContinue;}
};

}  // namespace ECSEngine
