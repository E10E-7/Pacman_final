//
// Observer.h - Observer design pattern for Model-View separation
//

#ifndef PACMAN_RETRY_OBSERVER_H
#define PACMAN_RETRY_OBSERVER_H
#include <memory>
#include <vector>

/**
 * @file Observer.h
 * @brief Implementation of the Observer design pattern for Model-View decoupling
 *
 * This file provides the foundation for the Model-View-Controller architecture
 * by implementing the Observer pattern. It allows Views to automatically update
 * when Model state changes, without the Model having any knowledge of the View layer.
 *
 * Pattern Structure:
 * - Subject (Model): Entities that can be observed
 * - Observer (View): Objects that watch Subjects and react to changes
 * - Notification Flow: Subject::notify() → Observer::onNotify()
 *
 * Usage Example:
 * @code
 * // Model side (Logic layer)
 * class Pacman : public Subject {
 *     void move() {
 *         x += speed;
 *         notify(); // Trigger view updates
 *     }
 * };
 *
 * // View side (Representation layer)
 * class PacmanView : public Observer {
 *     void onNotify() override {
 *         draw(); // Redraw Pacman at new position
 *     }
 * };
 *
 * // Connection (in ConcreteFactory)
 * auto pacman = std::make_shared<Pacman>();
 * auto view = std::make_shared<PacmanView>(pacman);
 * pacman->attach(view); // View now observes Pacman
 * @endcode
 */

/**
 * @enum EventType
 * @brief Types of events that can trigger observer notifications
 *
 * While these event types are defined, the current implementation uses
 * a simpler approach where observers are notified generically and determine
 * what to do based on the subject's current state.
 *
 * Future Enhancement: Could be used for more granular event handling where
 * observers only react to specific event types rather than all notifications.
 *
 * @note Currently unused in favor of simpler generic notification
 */
enum class EventType {
    EntityMoved,        ///< Entity changed position
    EntityDied,         ///< Entity entered death state
    CoinCollected,      ///< Coin was picked up
    FruitCollected,     ///< Fruit was picked up
    GhostEaten,         ///< Ghost was eaten by Pacman
    DirectionChanged,   ///< Entity changed movement direction
    ModeChanged,        ///< Ghost changed AI mode (chasing/feared)
    AnimationUpdate     ///< Generic signal to update animation frame
};

/**
 * @class Observer
 * @brief Abstract base class for objects that observe Subjects
 *
 * Observer is the "View" side of the Observer pattern. Objects that inherit
 * from Observer can attach themselves to Subjects and will be notified when
 * the Subject's state changes.
 *
 * Design Principles:
 * - Pure virtual onNotify() ensures all observers implement response logic
 * - Virtual destructor prevents memory leaks with polymorphic deletion
 * - Default constructor allows simple initialization
 *
 * Typical Subclasses:
 * - EntityView (and its subclasses: PacmanView, GhostView, etc.)
 * - Score (observes game events for point calculation)
 *
 * Thread Safety:
 * - NOT thread-safe - assumes single-threaded game loop
 *
 * @see Subject For the observable side of the pattern
 */
class Observer {
public:
    /**
     * @brief Default constructor
     */
    Observer() = default;

    /**
     * @brief Virtual destructor for safe polymorphic deletion
     *
     * Ensures proper cleanup when deleting Observer* pointing to derived classes.
     */
    virtual ~Observer() = default;

    /**
     * @brief Called when the observed Subject's state changes
     *
     * This pure virtual method must be implemented by all concrete observers.
     * It defines how the observer responds to changes in the subject.
     *
     * Implementation Strategy:
     * - EntityView subclasses typically call draw() to re-render
     * - Score class performs point calculations based on event type
     * - Could check subject's state to determine specific response
     *
     * @note This method is called by Subject::notify()
     * @note May be called multiple times per frame if subject changes frequently
     */
    virtual void onNotify() = 0;
};

/**
 * @class Subject
 * @brief Abstract base class for objects that can be observed
 *
 * Subject is the "Model" side of the Observer pattern. Objects that inherit
 * from Subject can have Observers attached to them. When the Subject's state
 * changes, it calls notify() to inform all attached observers.
 *
 * Design Principles:
 * - Manages a list of weak_ptr to observers (not owning)
 * - Automatically removes destroyed observers
 * - Prevents duplicate observer registration
 * - Safe to attach/detach during notification (uses weak_ptr)
 *
 * Memory Management:
 * - Uses std::shared_ptr for observer storage in the vector
 * - Observers typically have longer lifetime than subjects (Views outlive Models)
 * - No circular reference issues due to proper ownership hierarchy
 *
 * Typical Subclasses:
 * - Entity (and all game entities: Pacman, Ghost, Coin, etc.)
 *
 * Usage Pattern:
 * @code
 * class MyEntity : public Subject {
 *     void changeState() {
 *         // Modify internal state
 *         x = newX;
 *         // Notify all observers
 *         notify();
 *     }
 * };
 * @endcode
 *
 * Thread Safety:
 * - NOT thread-safe - observers list can be corrupted by concurrent attach/detach
 * - Assumes single-threaded game loop where all updates happen sequentially
 *
 * @see Observer For the observer side of the pattern
 */
class Subject {
    std::vector<std::shared_ptr<Observer>> observers;  ///< List of attached observers

public:
    /**
     * @brief Virtual destructor for safe polymorphic deletion
     *
     * Automatically detaches all observers when Subject is destroyed.
     */
    virtual ~Subject() = default;

    /**
     * @brief Attaches an observer to receive notifications
     * @param observer Shared pointer to the observer to attach
     *
     * The observer will be added to the notification list if it's not already present.
     * Duplicate observers are automatically prevented using std::ranges::find.
     *
     * Memory Management:
     * - Takes a shared_ptr, so the Subject shares ownership with the caller
     * - Observer will not be destroyed as long as Subject exists
     * - Safe to call multiple times with same observer (no duplicates)
     *
     * Performance:
     * - O(n) search to check for duplicates (where n = number of observers)
     * - Typically n is very small (1-2 observers per entity)
     *
     * @note Thread-unsafe - do not call from multiple threads
     *
     * Example:
     * @code
     * auto entity = std::make_shared<Pacman>();
     * auto view = std::make_shared<PacmanView>();
     * entity->attach(view); // View will be notified of entity changes
     * @endcode
     */
    void attach(const std::shared_ptr<Observer>& observer);

    /**
     * @brief Detaches an observer from the notification list
     * @param observer Shared pointer to the observer to remove
     *
     * Removes the specified observer from the list. If the observer is not
     * in the list, this is a no-op (safe to call even if not attached).
     *
     * Uses std::erase which automatically handles:
     * - Removing all matching elements (though there should only be one)
     * - Shrinking the vector
     * - Calling the observer's destructor if this was the last reference
     *
     * Performance:
     * - O(n) search + O(n) removal (where n = number of observers)
     * - Acceptable since n is typically very small (1-2)
     *
     * @note Thread-unsafe - do not call from multiple threads
     * @note Safe to call during notify() loop (uses std::erase, not manual iteration)
     *
     * Example:
     * @code
     * entity->detach(view); // View will no longer receive notifications
     * @endcode
     */
    void detach(const std::shared_ptr<Observer>& observer);

    /**
     * @brief Notifies all attached observers of a state change
     *
     * Iterates through all observers and calls their onNotify() method.
     * Automatically skips null observers (defensive programming).
     *
     * Call this method whenever the Subject's state changes in a way that
     * observers need to know about. For example:
     * - Entity moved to new position
     * - Ghost changed state (chasing → feared)
     * - Animation frame needs to update
     *
     * Performance:
     * - O(n) where n = number of observers
     * - Each observer's onNotify() may trigger rendering or other work
     * - Called frequently (potentially every frame), so keep observer count low
     *
     * Safety:
     * - Null-checks observers before calling (defensive)
     * - Uses range-based for loop which is safe with std::erase during iteration
     * - Observers can safely detach themselves during notification
     *
     * @note Thread-unsafe - assumes single-threaded game loop
     * @note Marked const because it doesn't modify Subject's logical state
     *       (only notifies about existing state, doesn't change it)
     *
     * Example:
     * @code
     * class Pacman : public Subject {
     *     void update(float deltaTime) {
     *         x += speed * deltaTime;
     *         notify(); // Tell views to redraw at new position
     *     }
     * };
     * @endcode
     */
    void notify() const;
};

#endif //PACMAN_RETRY_OBSERVER_H