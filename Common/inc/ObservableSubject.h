/***************************************************************************
 *============= Copyright by Darmstadt University of Applied Sciences =======
 ****************************************************************************
 * Filename        : ObservableSubject.h
 * Author          : Bharath Ramachandraiah (stbhrama@stud.h-da.de)
 * Description     : Observable subject implementation for Observer pattern
 *
 ****************************************************************************/

#ifndef OBSERVABLESUBJECT_H
#define OBSERVABLESUBJECT_H

#include "EventData.h"
#include "IObserver.h"

#include <map>
#include <memory>
#include <mutex>
#include <vector>

/**
 * @brief Concrete implementation of IObservable interface
 */
class ObservableSubject : public IObservable {
  private:
    mutable std::mutex m_observerMutex;
    std::map<EventType, std::vector<std::weak_ptr<IObserver>>> m_observers;

  public:
    /**
     * @brief Constructor
     */
    ObservableSubject();

    /**
     * @brief Destructor
     */
    virtual ~ObservableSubject();

    /**
     * @brief Add an observer to the notification list
     * @param observer Shared pointer to observer
     * @param eventType Type of events to observe
     * @return global::RC_t Return code indicating success or failure
     */
    global::RC_t addObserver(std::shared_ptr<IObserver> observer,
                             EventType eventType = EventType::DETECTION_RESULT) override;

    /**
     * @brief Remove an observer from the notification list
     * @param observer Shared pointer to observer to remove
     * @return global::RC_t Return code indicating success or failure
     */
    global::RC_t removeObserver(std::shared_ptr<IObserver> observer) override;

    /**
     * @brief Notify all registered observers of an event
     * @param eventData Shared pointer to event data
     */
    void notifyObservers(std::shared_ptr<EventData> eventData) override;

    /**
     * @brief Get number of observers for a specific event type
     * @param eventType Event type to count
     * @return size_t Number of active observers
     */
    size_t getObserverCount(EventType eventType) const;
};


#endif /* OBSERVABLESUBJECT_H */