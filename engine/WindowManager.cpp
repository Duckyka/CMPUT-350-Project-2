#include <cstdint>
#include "WindowManager.h"

namespace ECSEngine {

/// @brief Constructor for a windowManager. Takes in a width, height, and title for a RenderWindow. 
/// @param width 
/// @param height 
/// @param title 
WindowManager::WindowManager(unsigned int width, unsigned int height, const std::string& title)
    : mWindow(std::make_shared<sf::RenderWindow>(sf::VideoMode({width, height}), title)) {}

/// @brief Gets the RenderWindow stored in mWindow. 
/// @return Returns a shared pointer to a renderWindow. 
std::shared_ptr<sf::RenderWindow> WindowManager::GetWindow() const { return mWindow; }

/// @brief Gets the window bounds of the stored mWindow. 
/// @return Returns a Rect made up of mWindow (x, y) size. 
Rect WindowManager::GetWindowBounds() const { return Rect(mWindow->getSize().x, mWindow->getSize().y); }

/// @brief Given a Rect of width and height, draws a rectangle to the window with a fill colour made up of the given RGBA value. 
/// @param r 
/// @param red 
/// @param green 
/// @param blue 
/// @param alpha 
void WindowManager::DebugDrawRect(const Rect& r, int red, int green, int blue, int alpha)
{
    sf::RectangleShape rect;
    rect.setSize({r.width, r.height});
    rect.setFillColor(sf::Color{static_cast<std::uint8_t>(red), static_cast<std::uint8_t>(green), static_cast<std::uint8_t>(blue), static_cast<std::uint8_t>(alpha)});  //compile error
    rect.setOutlineColor(sf::Color{1, 1, 1, 0});
    rect.setPosition({r.topLeft.x, r.topLeft.y});
    mWindow->draw(rect);
}

/// @brief Given a Rect of width and height, draws the frame of a rectangle to the window with frame colour made up of the given RGBA value. 
/// @param r 
/// @param red 
/// @param green 
/// @param blue 
/// @param alpha 
void WindowManager::DebugFrameRect(const Rect& r, int red, int green, int blue, int alpha)
{
    sf::RectangleShape rect;
    rect.setSize({r.width, r.height});
    rect.setFillColor(sf::Color{1, 1, 1, 0});
    rect.setOutlineColor(sf::Color{static_cast<std::uint8_t>(red), static_cast<std::uint8_t>(green), static_cast<std::uint8_t>(blue), static_cast<std::uint8_t>(alpha)});  //compile error
    rect.setPosition({r.topLeft.x, r.topLeft.y});
    mWindow->draw(rect);
}

/// @brief Converts window coordinates to world coordinates, given a Rect. 
/// @param rect 
/// @return Returns Rect in world coordinates. 
Rect WindowManager::WindowToWorld(const Rect& rect) const
{
    return Rect{{mWorldCenter.x - ((mWindow->getSize().x * 0.5f) * mWorldUnitsPerPixel) + (rect.topLeft.x * mWorldUnitsPerPixel),
                 mWorldCenter.y - ((mWindow->getSize().y * 0.5f) * mWorldUnitsPerPixel) + (rect.topLeft.y * mWorldUnitsPerPixel)}, 
                 rect.width * mWorldUnitsPerPixel, rect.height * mWorldUnitsPerPixel};
}

/// @brief Converts window coordinates to world coordinates, given a point2D. 
/// @param pt 
/// @return Returns Point2D in world coordinates. 
Point2D WindowManager::WindowToWorld(const Point2D& pt) const
{
    return {mWorldCenter.x - ((mWindow->getSize().x * 0.5f) * mWorldUnitsPerPixel) + (pt.x * mWorldUnitsPerPixel), 
            mWorldCenter.y - ((mWindow->getSize().y * 0.5f) * mWorldUnitsPerPixel) + (pt.y * mWorldUnitsPerPixel)};
}

/// @brief Converts world coordinates to window coordinates, given a Rect. 
/// @param rect 
/// @return Returns Rect in window coordinates. 
Rect WindowManager::WorldToWindow(const Rect& rect) const
{
    return Rect{{(rect.topLeft.x - (mWorldCenter.x - ((mWindow->getSize().x * 0.5f) * mWorldUnitsPerPixel))) / mWorldUnitsPerPixel,
                  (rect.topLeft.y - (mWorldCenter.y - ((mWindow->getSize().y * 0.5f) * mWorldUnitsPerPixel))) / mWorldUnitsPerPixel}, 
                  rect.width / mWorldUnitsPerPixel, rect.height / mWorldUnitsPerPixel};
}

/// @brief Converts world coordinates to window coordinates, given a Point2D. 
/// @param pt 
/// @return Returns Point2D in window coordinates. 
Point2D WindowManager::WorldToWindow(const Point2D& pt) const
{
    return {(pt.x - (mWorldCenter.x - ((mWindow->getSize().x * 0.5f) * mWorldUnitsPerPixel))) / mWorldUnitsPerPixel, 
            (pt.y - (mWorldCenter.y - ((mWindow->getSize().y * 0.5f) * mWorldUnitsPerPixel))) / mWorldUnitsPerPixel};
}

/// @brief Sets the center of the camera in world coordinates given a world point and a screen point (in window coordinates). 
/// @param worldPt 
/// @param screenPt 
void WindowManager::SetCamera(const Point2D& worldPt, const Point2D& screenPt)
{
    mWorldCenter = {worldPt.x - screenPt.x * mWorldUnitsPerPixel + mWindow->getSize().x * 0.5f * mWorldUnitsPerPixel,
                    worldPt.y - screenPt.y * mWorldUnitsPerPixel + mWindow->getSize().y * 0.5f * mWorldUnitsPerPixel};
}

/// @brief Gets the center of the camera in world coordinates, stored in mWorldCenter. 
/// @return Returns a point2D in world coordinates to the center of the camera. 
Point2D WindowManager::GetWorldCenter() const { return mWorldCenter; }

/// @brief Returns the scale of the world, between world to window ratios. 
/// @return Returns a float for the scale of the world. 
float WindowManager::GetWorldScale() const { return mWorldUnitsPerPixel; }

/// @brief Sets the center of the camera in world coordinates given a world point. 
/// @param worldCenter 
void WindowManager::SetWorldCenter(const Point2D& worldCenter) { mWorldCenter = worldCenter; }

/// @brief Sets the scale of the world given a scale to set in mWorldUnitsPerPixel. 
/// @param worldUnitsPerPixel 
void WindowManager::SetWorldScale(float worldUnitsPerPixel) { mWorldUnitsPerPixel = worldUnitsPerPixel; }

/// @brief Sets the center of the camera in world coordinates given Rect in world coordinates. 
/// @param worldCameraRect 
void WindowManager::SetWorldRect(const Rect& worldCameraRect)
{
    Point2D cameraCenter = worldCameraRect.topLeft;
    cameraCenter.x += worldCameraRect.width * 0.5f;
    cameraCenter.y += worldCameraRect.height * 0.5f;
    SetWorldCenter(cameraCenter);
}

/// @brief Given a float x, converts this value, given in window units, to world units. 
/// @param x 
/// @return Returns a float value in world units. 
float WindowManager::WindowToWorld(float x) const { return x * mWorldUnitsPerPixel; }

/// @brief Given a float x, converts this calue, given in world units, to window units. 
/// @param x 
/// @return Returns a float value in window units. 
float WindowManager::WorldToWindow(float x) const { return x / mWorldUnitsPerPixel; }

}  // namespace ECSEngine
