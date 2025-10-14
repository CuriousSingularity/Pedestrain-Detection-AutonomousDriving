/***************************************************************************
 *============= Copyright by Darmstadt University of Applied Sciences =======
 ****************************************************************************
 * Filename        : EventData.h
 * Author          : Bharath Ramachandraiah (stbhrama@stud.h-da.de)
 * Description     : Event data structures for Observer pattern
 *
 ****************************************************************************/

#ifndef EVENTDATA_H
#define EVENTDATA_H

#include "global.h"

#include <string>
#include <vector>

#include <opencv2/opencv.hpp>

/**
 * @brief Event types for the observer pattern
 */
enum class EventType {
    DETECTION_RESULT,
    CAMERA_FRAME_READY,
    COMMUNICATION_STATUS,
    ERROR_OCCURRED,
    SYSTEM_STATUS
};

/**
 * @brief Base event data structure
 */
struct EventData {
    EventType type;
    int threadId;
    uint64_t timestamp;

    virtual ~EventData() = default;
};

/**
 * @brief Simple detection result for avoiding circular dependencies
 */
struct SimpleDetectionResult {
    cv::Rect boundingBox;
    float confidence;
    float angle;
    float deltaAngle;
};

/**
 * @brief Detection result event data
 */
struct DetectionEventData : public EventData {
    std::vector<SimpleDetectionResult> detections;
    std::string algorithmName;
    float processingTimeMs;

    DetectionEventData(const std::vector<SimpleDetectionResult>& results,
                       const std::string& algName, float processTime)
        : detections(results), algorithmName(algName), processingTimeMs(processTime) {
        type = EventType::DETECTION_RESULT;
    }
};

/**
 * @brief Camera frame ready event data
 */
struct CameraFrameEventData : public EventData {
    cv::Mat frame;
    uint32_t frameNumber;
    bool isValid;

    CameraFrameEventData(const cv::Mat& frameData, uint32_t frameNum, bool valid)
        : frame(frameData), frameNumber(frameNum), isValid(valid) {
        type = EventType::CAMERA_FRAME_READY;
    }
};

/**
 * @brief Communication status event data
 */
struct CommunicationEventData : public EventData {
    enum Status { CONNECTED, DISCONNECTED, ERROR, TRANSMITTING, RECEIVING };
    Status status;
    std::string message;

    CommunicationEventData(Status commStatus, const std::string& msg)
        : status(commStatus), message(msg) {
        type = EventType::COMMUNICATION_STATUS;
    }
};

/**
 * @brief Error event data
 */
struct ErrorEventData : public EventData {
    enum Severity { INFO, WARNING, ERROR, CRITICAL };
    Severity severity;
    std::string errorMessage;
    std::string component;
    global::RC_t errorCode;

    ErrorEventData(Severity sev, const std::string& msg, const std::string& comp, global::RC_t code)
        : severity(sev), errorMessage(msg), component(comp), errorCode(code) {
        type = EventType::ERROR_OCCURRED;
    }
};

#endif /* EVENTDATA_H */