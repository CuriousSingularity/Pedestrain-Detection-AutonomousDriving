/***************************************************************************
 *============= Copyright by Darmstadt University of Applied Sciences =======
 ****************************************************************************
 * Filename        : LegacyHOGAlgorithm.h
 * Author          : Bharath Ramachandraiah (stbhrama@stud.h-da.de)
 * Description     : Legacy HOG algorithm for C++14/17 compatibility
 *
 ****************************************************************************/

#ifndef LEGACYHOGGALGORITHM_H
#define LEGACYHOGGALGORITHM_H

#include "../../Common/inc/IDetectionAlgorithm.h"
#include "../../Common/inc/Cpp20Compatibility.h"
#include <opencv2/opencv.hpp>
#include <opencv2/objdetect.hpp>
#include <chrono>

/**
 * @brief Legacy HOG-based pedestrian detection algorithm for older C++ standards
 */
class LegacyHOGAlgorithm : public IDetectionAlgorithm {
private:
    cv::HOGDescriptor m_hogDescriptor;
    DetectionConfig m_config;
    bool m_isInitialized;
    mutable std::chrono::milliseconds m_lastProcessingTime;

    // Constants for detection
    static constexpr float CAMERA_FOV_DEGREES = 62.0f;
    static constexpr float HALF_FOV = CAMERA_FOV_DEGREES / 2.0f;
    static constexpr int MIN_DETECTION_AREA = 1000;
    static constexpr int MAX_STRAIGHT_LINES = 6;

public:
    /**
     * @brief Constructor
     */
    LegacyHOGAlgorithm() : m_isInitialized(false), m_lastProcessingTime(0) {
        // Set default configuration
        m_config.hitThreshold = 0.5f;
        m_config.scaleFactor = 1.1f;
        m_config.minNeighbors = 3;
        m_config.minSize = cv::Size(30, 30);
        
        initializeHOG();
    }

    /**
     * @brief Destructor
     */
    virtual ~LegacyHOGAlgorithm() = default;

    /**
     * @brief Detect objects using span interface
     * @param frame Input frame for detection
     * @param results Output span of detection results
     * @return global::RC_t Return code indicating success or failure
     */
    global::RC_t detect(const cv::Mat& frame, pedestrian_detection::compat::span<DetectionResult> results) override {
        std::vector<DetectionResult> tempResults;
        auto rc = detectLegacy(frame, tempResults);
        
        if (rc == global::RC_SUCCESS && !tempResults.empty()) {
            size_t copyCount = std::min(tempResults.size(), results.size());
            std::copy(tempResults.begin(), tempResults.begin() + copyCount, results.begin());
        }
        
        return rc;
    }
    
    /**
     * @brief Detect objects with vector output (legacy interface)
     * @param frame Input frame for detection
     * @param results Output vector of detection results
     * @return global::RC_t Return code indicating success or failure
     */
    global::RC_t detectLegacy(const cv::Mat& frame, std::vector<DetectionResult>& results) override {
        if (!m_isInitialized) {
            return global::RC_ERROR_INVALID_STATE;
        }
        
        if (frame.empty()) {
            return global::RC_ERROR_BAD_PARAM;
        }
        
        auto start = std::chrono::steady_clock::now();
        results.clear();
        
        try {
            std::vector<cv::Rect> detections;
            std::vector<double> foundWeights;
            
            // Perform HOG detection
            m_hogDescriptor.detectMultiScale(
                frame,
                detections,
                foundWeights,
                m_config.hitThreshold,
                cv::Size(8, 8),  // winStride
                cv::Size(32, 32), // padding
                m_config.scaleFactor,
                2  // finalThreshold
            );
            
            // Apply filtering and create results
            for (size_t i = 0; i < detections.size(); ++i) {
                const cv::Rect& detection = detections[i];
                
                // Apply area filter
                if (detection.width * detection.height < MIN_DETECTION_AREA) {
                    continue;
                }
                
                // Apply straight line filter
                if (!applyStraightLineFilter(frame, detection)) {
                    continue;
                }
                
                // Create detection result
                DetectionResult result;
                result.boundingBox = detection;
                result.confidence = static_cast<float>(foundWeights[i]);
                result.angle = calculateAngle(detection, frame.size());
                result.deltaAngle = calculateDeltaAngle(detection, frame.size());
                
                results.push_back(result);
            }
            
            auto end = std::chrono::steady_clock::now();
            m_lastProcessingTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
            
            return global::RC_SUCCESS;
        }
        catch (const std::exception& e) {
            return global::RC_ERROR;
        }
    }

    /**
     * @brief Detect objects with modern return type
     * @param frame Input frame for detection
     * @return std::vector<DetectionResult> Detection results
     */
    std::vector<DetectionResult> detectRange(const cv::Mat& frame) override {
        std::vector<DetectionResult> results;
        detectLegacy(frame, results);
        return results;
    }

    /**
     * @brief Configure the detection algorithm
     * @param config Configuration parameters
     * @return global::RC_t Return code indicating success or failure
     */
    global::RC_t configure(const DetectionConfig& config) override {
        m_config = config;
        return global::RC_SUCCESS;
    }

    /**
     * @brief Get algorithm name
     * @return std::string Algorithm identifier
     */
    std::string getName() const override {
#if HAS_CPP20
        return "HOG_Pedestrian_Detector_C++20";
#elif HAS_CPP17
        return "HOG_Pedestrian_Detector_C++17";
#else
        return "HOG_Pedestrian_Detector_C++14";
#endif
    }
    
    /**
     * @brief Get current configuration
     * @return DetectionConfig Current configuration
     */
    DetectionConfig getCurrentConfig() const override {
        return m_config;
    }
    
    /**
     * @brief Get processing time statistics
     * @return std::chrono::milliseconds Last processing time
     */
    std::chrono::milliseconds getLastProcessingTime() const override {
        return m_lastProcessingTime;
    }

private:
    /**
     * @brief Initialize HOG descriptor
     * @return global::RC_t Return code indicating success or failure
     */
    global::RC_t initializeHOG() {
        try {
            m_hogDescriptor.setSVMDetector(cv::HOGDescriptor::getDefaultPeopleDetector());
            m_isInitialized = true;
            return global::RC_SUCCESS;
        }
        catch (const std::exception& e) {
            return global::RC_ERROR;
        }
    }

    /**
     * @brief Calculate angle of detection relative to camera center
     * @param detection Bounding box of detection
     * @param frameSize Size of input frame
     * @return float Angle in degrees (-31 to +31 for 62° FOV)
     */
    float calculateAngle(const cv::Rect& detection, const cv::Size& frameSize) {
        float centerX = detection.x + detection.width / 2.0f;
        float frameCenterX = frameSize.width / 2.0f;
        
        float pixelOffset = centerX - frameCenterX;
        float pixelsPerDegree = frameSize.width / CAMERA_FOV_DEGREES;
        float angle = pixelOffset / pixelsPerDegree;
        
        // Clamp to FOV range
        angle = std::max(-HALF_FOV, std::min(HALF_FOV, angle));
        
        return angle;
    }

    /**
     * @brief Calculate angular span of detection
     * @param detection Bounding box of detection
     * @param frameSize Size of input frame
     * @return float Angular span in degrees
     */
    float calculateDeltaAngle(const cv::Rect& detection, const cv::Size& frameSize) {
        float pixelsPerDegree = frameSize.width / CAMERA_FOV_DEGREES;
        return detection.width / pixelsPerDegree;
    }

    /**
     * @brief Apply straight line filtering to reduce false positives
     * @param frame Input frame
     * @param detection Detection bounding box
     * @return bool True if detection passes filter (few straight lines)
     */
    bool applyStraightLineFilter(const cv::Mat& frame, const cv::Rect& detection) {
        try {
            cv::Mat roi = frame(detection);
            cv::Mat gray, edges;
            
            cv::cvtColor(roi, gray, cv::COLOR_BGR2GRAY);
            cv::Canny(gray, edges, 50, 150);
            
            std::vector<cv::Vec4i> lines;
            cv::HoughLinesP(edges, lines, 1, CV_PI/180, 50, 30, 10);
            
            return lines.size() <= MAX_STRAIGHT_LINES;
        }
        catch (...) {
            return true; // If filtering fails, accept the detection
        }
    }
};

#endif /* LEGACYHOGGALGORITHM_H */