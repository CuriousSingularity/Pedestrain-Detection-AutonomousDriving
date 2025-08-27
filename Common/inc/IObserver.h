/***************************************************************************
 *============= Copyright by Darmstadt University of Applied Sciences =======
 ****************************************************************************
 * Filename        : IObserver.h
 * Author          : Bharath Ramachandraiah (stbhrama@stud.h-da.de)
 * Description     : Observer pattern interface for event notifications
 *
 ****************************************************************************/

#ifndef IOBSERVER_H
#define IOBSERVER_H

#include "EventData.h"
#include "global.h"

#include <memory>

/**
 * @brief Observer interface for receiving event notifications
 */
class IObserver {
  public:
    virtual ~IObserver() = default;

    /**
     * @brief Called when an observed event occurs
     * @param eventData Shared pointer to event data
     */
    virtual void onNotify(std::shared_ptr<EventData> eventData) = 0;

    /**
     * @brief Get observer identifier
     * @return std::string Observer name/identifier
     */
    virtual std::string getObserverId() const = 0;
};

/**
 * @brief Subject interface for observable objects
 */
class IObservable {
  public:
    virtual ~IObservable() = default;

    /**
     * @brief Add an observer to the notification list
     * @param observer Shared pointer to observer
     * @param eventType Type of events to observe (optional filter)
     * @return global::RC_t Return code indicating success or failure
     */
    virtual global::RC_t addObserver(std::shared_ptr<IObserver> observer,
                                     EventType eventType = EventType::DETECTION_RESULT) = 0;

    /**
     * @brief Remove an observer from the notification list
     * @param observer Shared pointer to observer to remove
     * @return global::RC_t Return code indicating success or failure
     */
    virtual global::RC_t removeObserver(std::shared_ptr<IObserver> observer) = 0;

    /**
     * @brief Notify all registered observers of an event
     * @param eventData Shared pointer to event data
     */
    virtual void notifyObservers(std::shared_ptr<EventData> eventData) = 0;
};

#endif /* IOBSERVER_H */