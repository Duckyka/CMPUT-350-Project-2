#include "SpriteManager.h"

namespace ECSEngine {

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

sf::Sprite& SpriteManager::GetSprite(SpriteID id) {
    return mSprites.at(id);
}

}  // namespace ECSEngine
