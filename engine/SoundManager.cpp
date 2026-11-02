#include "SoundManager.h"

namespace ECSEngine {

void SoundManager::RegisterSound(const std::string& soundPath, const std::string& soundName)
{
    auto seen_string = mSounds.find(soundName);
    if (seen_string != mSounds.end())
    {
        //Do nothing, sound is already registered
    }
    else
    {
        sf::SoundBuffer soundBuffer(soundPath);
        mSounds.insert({soundName, soundBuffer});
    }
}

sf::Sound& SoundManager::GetSound(const std::string& soundName) {
    static sf::SoundBuffer soundBuffer = mSounds[soundName];
    static sf::Sound sound(soundBuffer);
    sound.play();
    return sound;
}

}  // namespace ECSEngine
