#include "SpriteManager.h"

namespace ECSEngine {

/// @brief Given a texturePath and a sourceRect, checks if the texturePath is already registered. If so, adds the new sprite based on the
///        provided sourceRect. If not seen, registers the new texture to mTextures and then adds the new sprite based on the provied
///        sourceRect. Stores these sprites in mSprites to be used later. 
/// @param texturePath 
/// @param sourceRect 
/// @return Returns the spriteID, AKA the index in the vector mSprites of the newly created sprite. 
SpriteID SpriteManager::RegisterTexture(const std::string& texturePath, const Rect& sourceRect)
{
    auto seen_string = mTextures.find(texturePath);
    if (seen_string != mTextures.end())
    {
        sf::Sprite sprite(seen_string->second);
        sprite.setTextureRect(sf::IntRect({sourceRect.topLeft.x, sourceRect.topLeft.y}, {sourceRect.width, sourceRect.height}));
        mSprites.push_back(sprite);
        return mSprites.size() - 1;
    }
    else
    {
        sf::Texture texture(texturePath);
        mTextures.insert({texturePath, texture});

        sf::Sprite sprite(seen_string->second);
        sprite.setTextureRect(sf::IntRect({sourceRect.topLeft.x, sourceRect.topLeft.y}, {sourceRect.width, sourceRect.height}));
        mSprites.push_back(sprite);
        return mSprites.size() - 1;
    }
}

/// @brief Returns a reference to the given spriteID from the list mSprites. 
/// @param id 
/// @return Returns a sprite reference. 
sf::Sprite& SpriteManager::GetSprite(SpriteID id) {
    return mSprites.at(id);
}

}  // namespace ECSEngine
