//
// Camera.h - Coordinate transformation (Representation)
//

#ifndef PACMAN_CAMERA_H
#define PACMAN_CAMERA_H

#include <SFML/Graphics.hpp>

namespace Render {

    class Camera {
    private:
        unsigned int windowWidth;
        unsigned int windowHeight;

    public:
        Camera(unsigned int width, unsigned int height);

        // Convert normalized coordinates [-1, 1] to pixel coordinates
        [[nodiscard]] sf::Vector2f worldToScreen(float x, float y) const;

        // Convert normalized size to pixel size
        [[nodiscard]] float worldToScreenSize(float size) const;

        // Update window dimensions
        void setWindowSize(unsigned int width, unsigned int height);

        [[nodiscard]] unsigned int getWindowWidth() const { return windowWidth; }
        [[nodiscard]] unsigned int getWindowHeight() const { return windowHeight; }
    };

} // namespace Render

#endif //PACMAN_CAMERA_H