//
// Created by etienne on 12/4/25.
//

#include "../Logic/Observer.h"

#include <algorithm>

void Subject::attach(const std::shared_ptr<Observer>& observer) {
    // Check if observer is already attached to avoid duplicates
    if (auto it = std::ranges::find(observers, observer); it == observers.end()) {
        observers.push_back(observer);
    }
}

void Subject::detach(const std::shared_ptr<Observer>& observer) {
    // Remove the observer from the vector
    std::erase(observers, observer);
}

void Subject::notify() const {
    // Notify all attached observers
    for (auto& observer : observers) {
        if (observer) {
            observer->onNotify();
        }
    }
}
