#include "SoundManager.h"

namespace ECSEngine {

/// @brief Given the filepath of a sound and a string for ther sound's name, creates a new soundBuffer and stores it in the list mSounds.  
///        Created sounds are stored in mSounds to be played at any time. 
///        If a soundname is already in mSounds and is requested to be registered again, nothing happens. 
/// @param soundPath 
/// @param soundName 
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

/// @brief Given a soundName as a string, finds the respective soundBuffer in mSounds and plays the extracted sound. 
/// @param soundName 
/// @return Returns the sound after it is played. 
sf::Sound& SoundManager::GetSound(const std::string& soundName) {
    static sf::SoundBuffer soundBuffer = mSounds[soundName];
    static sf::Sound sound(soundBuffer);
    sound.play();
    return sound;
}

}  // namespace ECSEngine
