/***************************************************************************
 *============= Copyright by Darmstadt University of Applied Sciences =======
 ****************************************************************************
 * Filename        : HOGDetectionAlgorithm.cpp
 * Author          : Bharath Ramachandraiah (stbhrama@stud.h-da.de)
 * Description     : HOG-based pedestrian detection algorithm implementation
 *
 ****************************************************************************/

#include "../inc/HOGDetectionAlgorithm.h"
#include "../../Common/inc/ConfigurationManager.h"
#include <iostream>
#include <algorithm>

using namespace cv;
using namespace std;

// Camera FOV constant (62 degrees total)
static const float CAMERA_FOV_DEGREES = 62.0f;
static const float HALF_FOV = CAMERA_FOV_DEGREES / 2.0f;

// Filtering thresholds
static const int MIN_DETECTION_AREA = 1000;  // Minimum area for valid detection
static const int MAX_STRAIGHT_LINES = 6;     // Maximum straight lines for valid detection

HOGDetectionAlgorithm::HOGDetectionAlgorithm() : m_isInitialized(false) {
    // Set default configuration
    m_config.hitThreshold = 0.5f;
    m_config.scaleFactor = 1.1f;
    m_config.minNeighbors = 3;
    m_config.minSize = Size(30, 30);
    
    // Initialize HOG descriptor
    initializeHOG();
}

global::RC_t HOGDetectionAlgorithm::initializeHOG() {
    try {
        // Create HOG descriptor with default people detector
        m_hogDescriptor.setSVMDetector(HOGDescriptor::getDefaultPeopleDetector());
        
        // Load configuration from ConfigurationManager if available
        auto& configMgr = ConfigurationManager::getInstance();
        
        m_config.hitThreshold = configMgr.getValue(ConfigCategory::DETECTION, "hitThreshold", ConfigValue(0.5f)).floatValue;
        m_config.scaleFactor = configMgr.getValue(ConfigCategory::DETECTION, "scaleFactor", ConfigValue(1.1f)).floatValue;
        m_config.minNeighbors = configMgr.getValue(ConfigCategory::DETECTION, "minNeighbors", ConfigValue(3)).intValue;
        
        m_isInitialized = true;
        return global::RC_SUCCESS;
    }
    catch (const exception& e) {
        cerr << "ERROR: Failed to initialize HOG descriptor: " << e.what() << endl;
        return global::RC_ERROR;
    }
}

global::RC_t HOGDetectionAlgorithm::detect(const Mat& frame, vector<DetectionResult>& results) {
    if (!m_isInitialized) {
        return global::RC_ERROR_INVALID_STATE;
    }
    
    if (frame.empty()) {
        return global::RC_ERROR_BAD_PARAM;
    }
    
    results.clear();
    
    try {
        vector<Rect> detections;
        vector<double> foundWeights;
        
        // Perform HOG detection
        m_hogDescriptor.detectMultiScale(
            frame,
            detections,
            foundWeights,
            m_config.hitThreshold,
            Size(8, 8),  // winStride
            Size(32, 32), // padding
            m_config.scaleFactor,
            2,  // finalThreshold
            false  // useMeanshiftGrouping
        );
        
        // Convert weights to confidences
        vector<float> confidences;
        for (double weight : foundWeights) {
            confidences.push_back(static_cast<float>(weight));
        }
        
        // Apply Non-Maximum Suppression
        vector<Rect> nmsDetections;
        vector<float> nmsConfidences;
        if (applyNMS(detections, confidences, nmsDetections, nmsConfidences) != global::RC_SUCCESS) {
            return global::RC_ERROR;
        }
        
        // Apply additional filtering and create results
        for (size_t i = 0; i < nmsDetections.size(); ++i) {
            const Rect& detection = nmsDetections[i];
            
            // Apply straight line filter
            if (!applyStraightLineFilter(frame, detection)) {
                continue;
            }
            
            // Apply area filter
            if (!applyAreaFilter(detection)) {
                continue;
            }
            
            // Create detection result
            DetectionResult result;
            result.boundingBox = detection;
            result.confidence = nmsConfidences[i];
            result.angle = calculateAngle(detection, frame.size());
            result.deltaAngle = calculateDeltaAngle(detection, frame.size());
            
            results.push_back(result);
        }
        
        return global::RC_SUCCESS;
    }
    catch (const exception& e) {
        cerr << "ERROR: Detection failed: " << e.what() << endl;
        return global::RC_ERROR;
    }
}

global::RC_t HOGDetectionAlgorithm::configure(const DetectionConfig& config) {
    m_config = config;
    
    // Save to configuration manager
    auto& configMgr = ConfigurationManager::getInstance();
    configMgr.setValue(ConfigCategory::DETECTION, "hitThreshold", ConfigValue(config.hitThreshold));
    configMgr.setValue(ConfigCategory::DETECTION, "scaleFactor", ConfigValue(config.scaleFactor));
    configMgr.setValue(ConfigCategory::DETECTION, "minNeighbors", ConfigValue(config.minNeighbors));
    
    return global::RC_SUCCESS;
}

global::RC_t HOGDetectionAlgorithm::setHOGConfig(const HOGConfig& hogConfig) {
    m_hogConfig = hogConfig;
    return initializeHOG();
}

global::RC_t HOGDetectionAlgorithm::applyNMS(const vector<Rect>& detections, 
                                           const vector<float>& confidences,
                                           vector<Rect>& filteredDetections,
                                           vector<float>& filteredConfidences) {
    if (detections.size() != confidences.size()) {
        return global::RC_ERROR_BAD_PARAM;
    }
    
    vector<int> indices;
    dnn::NMSBoxes(detections, confidences, m_config.hitThreshold, 0.3f, indices);
    
    filteredDetections.clear();
    filteredConfidences.clear();
    
    for (int idx : indices) {
        filteredDetections.push_back(detections[idx]);
        filteredConfidences.push_back(confidences[idx]);
    }
    
    return global::RC_SUCCESS;
}

float HOGDetectionAlgorithm::calculateAngle(const Rect& detection, const Size& frameSize) {
    // Calculate center of detection
    float centerX = detection.x + detection.width / 2.0f;
    float frameCenterX = frameSize.width / 2.0f;
    
    // Calculate angle relative to frame center
    float pixelOffset = centerX - frameCenterX;
    float pixelsPerDegree = frameSize.width / CAMERA_FOV_DEGREES;
    float angle = pixelOffset / pixelsPerDegree;
    
    // Clamp to FOV range
    angle = max(-HALF_FOV, min(HALF_FOV, angle));
    
    return angle;
}

float HOGDetectionAlgorithm::calculateDeltaAngle(const Rect& detection, const Size& frameSize) {
    float pixelsPerDegree = frameSize.width / CAMERA_FOV_DEGREES;
    float deltaAngle = detection.width / pixelsPerDegree;
    
    return deltaAngle;
}

bool HOGDetectionAlgorithm::applyStraightLineFilter(const Mat& frame, const Rect& detection) {
    // Extract detection region
    Mat roi = frame(detection);
    Mat gray, edges;
    
    // Convert to grayscale and apply Canny edge detection
    cvtColor(roi, gray, COLOR_BGR2GRAY);
    Canny(gray, edges, 50, 150);
    
    // Detect lines using Hough transform
    vector<Vec4i> lines;
    HoughLinesP(edges, lines, 1, CV_PI/180, 50, 30, 10);
    
    // Filter based on number of straight lines
    return lines.size() <= MAX_STRAIGHT_LINES;
}

bool HOGDetectionAlgorithm::applyAreaFilter(const Rect& detection) {
    int area = detection.width * detection.height;
    return area >= MIN_DETECTION_AREA;
}