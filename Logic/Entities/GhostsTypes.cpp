// Logic/Entities/GhostsTypes.cpp
#include "GhostsTypes.h"
#include <limits>
#include <cmath>

#include "Pacman.h"

namespace Logic {

    // ---------------- HELPER FUNCTIE ----------------
    // Omdat Pink, Blue en Orange allemaal Manhattan distance gebruiken,
    // maken we een kleine helper om dubbele code te voorkomen.
    char getBestManhattanDirection(float startX, float startY, float targetX, float targetY, const std::vector<char>& valid) {
        float minDistance = std::numeric_limits<float>::max();
        std::vector<char> bestMoves;

        for (char move : valid) {
            float nextX = startX;
            float nextY = startY;

            // Simuleer 1 stap
            switch(move) {
                case 'U': nextY -= 1.0f; break;
                case 'D': nextY += 1.0f; break;
                case 'L': nextX -= 1.0f; break;
                case 'R': nextX += 1.0f; break;
            }

            // Manhattan distance naar Target
            float dist = std::abs(nextX - targetX) + std::abs(nextY - targetY);

            if (dist < minDistance - 0.001f) {
                minDistance = dist;
                bestMoves.clear();
                bestMoves.push_back(move);
            } else if (std::abs(dist - minDistance) < 0.001f) {
                bestMoves.push_back(move);
            }
        }

        if (bestMoves.empty()) return valid[0];
        // Ties broken at random
        return bestMoves[Random::getInstance().nextInt(0, bestMoves.size() - 1)];
    }

    // ---------------- RED GHOST (Locked / Random) ----------------
    char RedGhost::decideDirection(const std::vector<char> &valid, std::shared_ptr<Pacman> pacman) {
        if (valid.empty()) return ' ';

        // Requirement: "If a ghost reaches a corner or intersection it will reconsider"
        // Een intersection is meestal als er meer dan 2 opties zijn, OF als we niet rechtdoor kunnen.

        bool isIntersection = (valid.size() >= 2);

        // Check of we rechtdoor kunnen (current direction zit in valid)
        bool canGoStraight = false;
        for(char c : valid) if(c == direction) canGoStraight = true;

        if (!canGoStraight) isIntersection = true; // We botsen, dus we moeten kiezen

        if (isIntersection) {
            // Requirement: "probability p = 0.5, the ghost will lock to a random direction"
            if (Random::getInstance().nextBool(0.5f)) {
                return valid[Random::getInstance().nextInt(0, valid.size() - 1)];
            } else {
                // De andere 50%: Probeer rechtdoor te blijven gaan ("Locked")
                if (canGoStraight) return direction;
                // Als we niet rechtdoor kunnen, moeten we alsnog een nieuwe kiezen
                return valid[Random::getInstance().nextInt(0, valid.size() - 1)];
            }
        }

        // Geen intersection? Blijf in de locked direction (als die nog geldig is)
        if (canGoStraight) return direction;
        return valid[0]; // Bocht (zou door intersection logic opgevangen moeten worden)
    }

    // ---------------- PINK GHOST (Target: In Front of Pacman) ----------------
    char PinkGhost::decideDirection(const std::vector<char> &valid, std::shared_ptr<Pacman> pacman) {
        if (valid.empty()) return ' ';

        // 1. Bepaal positie VOOR Pacman
        float targetX = pacman->getX();
        float targetY = pacman->getY();

        char pacDir = pacman->getDirection();
        float offset = 1.0f; // 1 tile vooruit

        switch(pacDir) {
            case 'U': targetY -= offset; break;
            case 'D': targetY += offset; break;
            case 'L': targetX -= offset; break;
            case 'R': targetX += offset; break;
            default: break; // Stilstand
        }

        // 2. Gebruik Manhattan logica naar dit target
        return getBestManhattanDirection(getX(), getY(), targetX, targetY, valid);
    }

    // ---------------- BLUE GHOST (Kopie van Pink) ----------------
    char BlueGhost::decideDirection(const std::vector<char> &valid, std::shared_ptr<Pacman> pacman) {
        // Exact dezelfde logica als Pink
        if (valid.empty()) return ' ';

        float targetX = pacman->getX();
        float targetY = pacman->getY();
        char pacDir = pacman->getDirection(); // <--- ZORG DAT DEZE BESTAAT
        float offset = 1.0f;

        switch(pacDir) {
            case 'U': targetY -= offset; break;
            case 'D': targetY += offset; break;
            case 'L': targetX -= offset; break;
            case 'R': targetX += offset; break;
            default: break;
        }

        return getBestManhattanDirection(getX(), getY(), targetX, targetY, valid);
    }

    // ---------------- ORANGE GHOST (Target: Direct Pacman) ----------------
    char OrangeGhost::decideDirection(const std::vector<char> &valid, std::shared_ptr<Pacman> pacman) {
        if (valid.empty()) return ' ';

        // Requirement: "Minimize Manhattan distance to its location" (Direct chase)
        // Dit was de 'oude' Pink logica
        return getBestManhattanDirection(getX(), getY(), pacman->getX(), pacman->getY(), valid);
    }

}