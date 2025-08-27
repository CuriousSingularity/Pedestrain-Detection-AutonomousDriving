/***************************************************************************
 *============= Copyright by Darmstadt University of Applied Sciences =======
 ****************************************************************************
 * Filename        : IDetectionAlgorithm.h
 * Author          : Bharath Ramachandraiah (stbhrama@stud.h-da.de)
 * Description     : Interface for detection algorithms using C++20 concepts
 *
 ****************************************************************************/

#ifndef IDETECTIONALGORITHM_H
#define IDETECTIONALGORITHM_H

#include <opencv2/opencv.hpp>
#include <vector>
#include <chrono>
#include "global.h"
#include "Cpp20Compatibility.h"

#if CONCEPTS_AVAILABLE
    #include "Concepts.h"
#endif

/**
 * @brief Interface for detection algorithms using Strategy pattern
 */
class IDetectionAlgorithm {
public:
    struct DetectionResult {
        cv::Rect boundingBox;
        float confidence;
        float angle;
        float deltaAngle;
    };

    struct DetectionConfig {
        float hitThreshold{0.5f};
        float scaleFactor{1.1f};
        int minNeighbors{3};
        cv::Size minSize{30, 30};
        cv::Size maxSize{};
        
        // C++20 designated initializers support
        constexpr DetectionConfig() = default;
        constexpr DetectionConfig(float threshold, float scale, int neighbors) 
            : hitThreshold{threshold}, scaleFactor{scale}, minNeighbors{neighbors} {}
    };

    virtual ~IDetectionAlgorithm() = default;

    /**
     * @brief Detect objects in the given frame using span interface
     * @param frame Input frame for detection (OpenCV compatible)
     * @param results Output span of detection results
     * @return RC_t Return code indicating success or failure
     */
    virtual global::RC_t detect(const cv::Mat& frame, pedestrian_detection::compat::span<DetectionResult> results) = 0;
    
    /**
     * @brief Detect objects with vector output (legacy interface)
     * @param frame Input frame for detection
     * @param results Output vector of detection results
     * @return RC_t Return code indicating success or failure
     */
    virtual global::RC_t detectLegacy(const cv::Mat& frame, std::vector<DetectionResult>& results) = 0;
    
    /**
     * @brief Detect objects with modern return type
     * @param frame Input frame for detection
     * @return std::vector<DetectionResult> Detection results
     */
    virtual std::vector<DetectionResult> detectRange(const cv::Mat& frame) = 0;

    /**
     * @brief Configure the detection algorithm
     * @param config Configuration parameters
     * @return RC_t Return code indicating success or failure
     */
#if CONCEPTS_AVAILABLE
    virtual global::RC_t configure(const DetectionConfig& config) 
        requires pedestrian_detection::concepts::ConfigurationValue<DetectionConfig> = 0;
#else
    virtual global::RC_t configure(const DetectionConfig& config) = 0;
#endif

    /**
     * @brief Get algorithm name using std::format
     * @return std::string Algorithm identifier
     */
    virtual std::string getName() const = 0;
    
    /**
     * @brief Get formatted algorithm information
     * @return std::string Formatted algorithm details
     */
    virtual std::string getInfo() const {
#if FORMAT_AVAILABLE
        return std::format("Algorithm: {}, Config: threshold={:.2f}, scale={:.2f}", 
                          getName(), getCurrentConfig().hitThreshold, getCurrentConfig().scaleFactor);
#else
        return pedestrian_detection::compat::format("Algorithm: {}, Config: threshold={:.2f}, scale={:.2f}", 
                          getName(), getCurrentConfig().hitThreshold, getCurrentConfig().scaleFactor);
#endif
    }
    
    /**
     * @brief Get current configuration
     * @return DetectionConfig Current configuration
     */
    virtual DetectionConfig getCurrentConfig() const = 0;
    
    /**
     * @brief Get processing time statistics
     * @return std::chrono::milliseconds Last processing time
     */
    virtual std::chrono::milliseconds getLastProcessingTime() const = 0;
};

#endif /* IDETECTIONALGORITHM_H */