#pragma once

#include <bitset>
#include <unordered_map>
#include <chrono>
#include "MathUtil.h"
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Window/Keyboard.hpp>
#include "Keyboard.h"

namespace ECSEngine {

struct InputComponent {
    InputComponent(EntityID whichEntity): entity(whichEntity), lastDash() {}
    EntityID entity;
    std::chrono::steady_clock::time_point lastDash;
    std::bitset<Key::ScanCount> keydown;
    std::bitset<Key::ScanCount> lastKeydown;
};

}  // namespace ECSEngine
