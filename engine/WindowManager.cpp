#include "WindowManager.h"

namespace ECSEngine {

WindowManager::WindowManager(unsigned int width, unsigned int height, const std::string& title)
    : mWindow(std::make_shared<sf::RenderWindow>(sf::VideoMode({width, height}), title)) {}

std::shared_ptr<sf::RenderWindow> WindowManager::GetWindow() const { return mWindow; }

Rect WindowManager::GetWindowBounds() const { return Rect(mWindow->getSize().x, mWindow->getSize().y); }



// NOT DONE YET -------------------------------------------------------------------------------------- NOT DONE YET
// NOT DONE YET -------------------------------------------------------------------------------------- NOT DONE YET
void WindowManager::DebugDrawRect(const Rect& r, int red, int green, int blue, int alpha)
{
    sf::RectangleShape rect;
    rect.setSize({r.width, r.height});
    rect.setFillColor(sf::Color{red, green, blue, alpha});
    rect.setOutlineColor(sf::Color{1.0, 1.0, 1.0, 0.0});
    rect.setPosition({r.topLeft.x, r.topLeft.y});
    mWindow->draw(rect);
}

void WindowManager::DebugFrameRect(const Rect& r, int red, int green, int blue, int alpha)
{
    sf::RectangleShape rect;
    rect.setSize({r.width, r.height});
    rect.setFillColor(sf::Color{1.0, 1.0, 1.0, 0.0});
    rect.setOutlineColor(sf::Color{red, green, blue, alpha});
    rect.setPosition({r.topLeft.x, r.topLeft.y});
    mWindow->draw(rect);
}
// NOT DONE YET -------------------------------------------------------------------------------------- NOT DONE YET
// NOT DONE YET -------------------------------------------------------------------------------------- NOT DONE YET




Rect WindowManager::WindowToWorld(const Rect& rect) const
{
    return Rect((mWorldCenter.x - ((mWindow->getSize().x * 0.5f) * mWorldUnitsPerPixel) + (rect.topLeft.x * mWorldUnitsPerPixel),
                 mWorldCenter.y - ((mWindow->getSize().y * 0.5f) * mWorldUnitsPerPixel) + (rect.topLeft.y * mWorldUnitsPerPixel)), 
                 (rect.width * mWorldUnitsPerPixel, rect.height * mWorldUnitsPerPixel));
}

Point2D WindowManager::WindowToWorld(const Point2D& pt) const
{
    return (mWorldCenter.x - ((mWindow->getSize().x * 0.5f) * mWorldUnitsPerPixel) + (pt.x * mWorldUnitsPerPixel), 
            mWorldCenter.y - ((mWindow->getSize().y * 0.5f) * mWorldUnitsPerPixel) + (pt.y * mWorldUnitsPerPixel));
}

Rect WindowManager::WorldToWindow(const Rect& rect) const
{
    return Rect((((rect.topLeft.x - (mWorldCenter.x - ((mWindow->getSize().x * 0.5f) * mWorldUnitsPerPixel))) / mWorldUnitsPerPixel),
                  (rect.topLeft.y - (mWorldCenter.y - ((mWindow->getSize().y * 0.5f) * mWorldUnitsPerPixel))) / mWorldUnitsPerPixel), 
                  (rect.width / mWorldUnitsPerPixel, rect.height / mWorldUnitsPerPixel));
}

Point2D WindowManager::WorldToWindow(const Point2D& pt) const
{
    return ((pt.x - (mWorldCenter.x - ((mWindow->getSize().x * 0.5f) * mWorldUnitsPerPixel))) / mWorldUnitsPerPixel, 
            (pt.y - (mWorldCenter.y - ((mWindow->getSize().y * 0.5f) * mWorldUnitsPerPixel))) / mWorldUnitsPerPixel);
}

void WindowManager::SetCamera(const Point2D& worldPt, const Point2D& screenPt)
{
    mWorldCenter = (worldPt.x - screenPt.x * mWorldUnitsPerPixel + mWindow->getSize().x * 0.5f * mWorldUnitsPerPixel,
                    worldPt.y - screenPt.y * mWorldUnitsPerPixel + mWindow->getSize().y * 0.5f * mWorldUnitsPerPixel);
}

Point2D WindowManager::GetWorldCenter() const { return mWorldCenter; }

float WindowManager::GetWorldScale() const { return 1; }

void WindowManager::SetWorldCenter(const Point2D& worldCenter) { mWorldCenter = worldCenter; }

void WindowManager::SetWorldScale(float worldUnitsPerPixel) { mWorldUnitsPerPixel = worldUnitsPerPixel; }

void WindowManager::SetWorldRect(const Rect& worldCameraRect)
{
    Point2D cameraCenter = worldCameraRect.topLeft;
    cameraCenter.x += worldCameraRect.width * 0.5f;
    cameraCenter.y += worldCameraRect.height * 0.5f;
    SetWorldCenter(cameraCenter);
}

float WindowManager::WindowToWorld(float x) const { return x * mWorldUnitsPerPixel; }

float WindowManager::WorldToWindow(float x) const { return x / mWorldUnitsPerPixel; }

}  // namespace ECSEngine
