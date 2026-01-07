// Logic/Entities/GhostsTypes.h
#ifndef PACMAN_RETRY_GHOSTTYPES_H
#define PACMAN_RETRY_GHOSTTYPES_H

#include "Ghost.h"

namespace Logic {

    // --- GHOST 1: RED (Locked / Random) ---
    class RedGhost : public Ghost {
    public:
        // Start direct (0s), spriteId = 0 (Red)
        RedGhost(float x, float y, float w, float h) : Ghost(x, y, w, h, 0.0f, 0) {}
        char decideDirection(const std::vector<char> &valid, std::shared_ptr<Pacman> pacman) override;
    };

    // --- GHOST 2: PINK (In Front of Pacman) ---
    class PinkGhost : public Ghost {
    public:
        // Start direct (0s), spriteId = 1 (Pink)
        PinkGhost(float x, float y, float w, float h) : Ghost(x, y, w, h, 0.0f, 1) {}
        char decideDirection(const std::vector<char> &valid, std::shared_ptr<Pacman> pacman) override;
    };

    // --- GHOST 3: BLUE (In Front of Pacman - Delayed) ---
    class BlueGhost : public Ghost {
    public:
        // Start na 5 seconden, spriteId = 2 (Blue)
        BlueGhost(float x, float y, float w, float h) : Ghost(x, y, w, h, 5.0f, 2) {}
        char decideDirection(const std::vector<char> &valid, std::shared_ptr<Pacman> pacman) override;
    };

    // --- GHOST 4: ORANGE (Direct Chase - Delayed) ---
    class OrangeGhost : public Ghost {
    public:
        // Start na 10 seconden, spriteId = 3 (Orange)
        OrangeGhost(float x, float y, float w, float h) : Ghost(x, y, w, h, 10.0f, 3) {}
        char decideDirection(const std::vector<char> &valid, std::shared_ptr<Pacman> pacman) override;
    };

}
#endif // PACMAN_RETRY_GHOSTTYPES_H