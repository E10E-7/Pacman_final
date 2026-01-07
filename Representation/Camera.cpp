//
// Camera.cpp
//

#include "Camera.h"
#include <algorithm>

namespace Render {

    Camera::Camera(unsigned int width, unsigned int height)
        : windowWidth(width), windowHeight(height) {}

    sf::Vector2f Camera::worldToScreen(float x, float y) const {
        // 1. Determine the scale factor (same as in worldToScreenSize)
        const auto scale = static_cast<float>(std::min(windowWidth, windowHeight));

        // 2. Calculate offsets to center the view in the window
        const float offsetX = (static_cast<float>(windowWidth) - scale) * 0.5f;
        const float offsetY = (static_cast<float>(windowHeight) - scale) * 0.5f;

        // 3. Apply transformation
        // Map [-1, 1] -> [0, 2] -> [0, 1] -> [0, scale] -> [offset, offset + scale]
        float screenX = ((x + 1.0f) * 0.5f * scale) + offsetX;
        float screenY = ((y + 1.0f) * 0.5f * scale) + offsetY;

        return {screenX, screenY};
    }

    float Camera::worldToScreenSize(const float size) const {
        // Use the smaller dimension to maintain aspect ratio
        const auto scale = static_cast<float>(std::min(windowWidth, windowHeight));
        // Divide by 2.0 because world space is [-1, 1], a range of 2.0
        return size * scale / 2.0f;
    }

    void Camera::setWindowSize(const unsigned int width, const unsigned int height) {
        windowWidth = width;
        windowHeight = height;
    }

} // namespace Render