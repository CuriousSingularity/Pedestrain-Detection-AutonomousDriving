/***************************************************************************
 *============= Copyright by Darmstadt University of Applied Sciences =======
 ****************************************************************************
 * Filename        : ObservableSubject.cpp
 * Author          : Bharath Ramachandraiah (stbhrama@stud.h-da.de)
 * Description     : Observable subject implementation for Observer pattern
 *
 ****************************************************************************/

#include "../inc/ObservableSubject.h"

#include <algorithm>
#include <chrono>
#include <iostream>

using namespace std;

ObservableSubject::ObservableSubject() {
    // Initialize empty
}

ObservableSubject::~ObservableSubject() {
    // Clear all observers
    lock_guard<mutex> lock(m_observerMutex);
    m_observers.clear();
}

global::RC_t ObservableSubject::addObserver(shared_ptr<IObserver> observer, EventType eventType) {
    if (!observer) {
        return global::RC_ERROR_NULL;
    }

    lock_guard<mutex> lock(m_observerMutex);

    // Check if observer already exists for this event type
    auto& observersForType = m_observers[eventType];
    auto it = find_if(observersForType.begin(), observersForType.end(),
                      [&observer](const weak_ptr<IObserver>& weakObs) {
                          auto sharedObs = weakObs.lock();
                          return sharedObs && sharedObs.get() == observer.get();
                      });

    if (it != observersForType.end()) {
        return global::RC_ERROR_INVALID;  // Observer already registered
    }

    observersForType.push_back(observer);
    return global::RC_SUCCESS;
}

global::RC_t ObservableSubject::removeObserver(shared_ptr<IObserver> observer) {
    if (!observer) {
        return global::RC_ERROR_NULL;
    }

    lock_guard<mutex> lock(m_observerMutex);
    bool found = false;

    // Remove from all event types
    for (auto& eventTypePair : m_observers) {
        auto newEnd = remove_if(eventTypePair.second.begin(), eventTypePair.second.end(),
                                [&observer, &found](const weak_ptr<IObserver>& weakObs) {
                                    auto sharedObs = weakObs.lock();
                                    if (sharedObs && sharedObs.get() == observer.get()) {
                                        found = true;
                                        return true;
                                    }
                                    return false;
                                });
        eventTypePair.second.erase(newEnd, eventTypePair.second.end());
    }

    return found ? global::RC_SUCCESS : global::RC_ERROR_NOT_MATCH;
}

void ObservableSubject::notifyObservers(shared_ptr<EventData> eventData) {
    if (!eventData) {
        return;
    }

    // Set timestamp if not already set
    if (eventData->timestamp == 0) {
        auto now = chrono::system_clock::now();
        auto timestamp = chrono::duration_cast<chrono::milliseconds>(now.time_since_epoch());
        eventData->timestamp = timestamp.count();
    }

    vector<shared_ptr<IObserver>> validObservers;

    {
        lock_guard<mutex> lock(m_observerMutex);

        // Get observers for this event type
        auto it = m_observers.find(eventData->type);
        if (it != m_observers.end()) {
            for (auto& weakObs : it->second) {
                auto sharedObs = weakObs.lock();
                if (sharedObs) {
                    validObservers.push_back(sharedObs);
                }
            }

            // Clean up expired weak pointers
            auto newEnd =
                remove_if(it->second.begin(), it->second.end(),
                          [](const weak_ptr<IObserver>& weakObs) { return weakObs.expired(); });
            it->second.erase(newEnd, it->second.end());
        }
    }

    // Notify observers outside of lock to prevent deadlock
    for (auto& observer : validObservers) {
        try {
            observer->onNotify(eventData);
        } catch (const exception& e) {
            cerr << "ERROR: Observer notification failed: " << e.what() << endl;
        }
    }
}

size_t ObservableSubject::getObserverCount(EventType eventType) const {
    lock_guard<mutex> lock(m_observerMutex);

    auto it = m_observers.find(eventType);
    if (it != m_observers.end()) {
        // Count only valid (non-expired) observers
        return count_if(it->second.begin(), it->second.end(),
                        [](const weak_ptr<IObserver>& weakObs) { return !weakObs.expired(); });
    }

    return 0;
}