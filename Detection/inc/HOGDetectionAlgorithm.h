/***************************************************************************
 *============= Copyright by Darmstadt University of Applied Sciences =======
 ****************************************************************************
 * Filename        : HOGDetectionAlgorithm.h
 * Author          : Bharath Ramachandraiah (stbhrama@stud.h-da.de)
 * Description     : C++20 HOG-based pedestrian detection algorithm
 *
 ****************************************************************************/

#ifndef HOGDETECTIONALGORITHM_H
#define HOGDETECTIONALGORITHM_H

#include "../../Common/inc/Concepts.h"
#include "../../Common/inc/IDetectionAlgorithm.h"
#include "../../Common/inc/ModernConstants.h"
#include "../../Common/inc/RangesUtilities.h"

#include <chrono>
#include <ranges>
#include <span>

#include <opencv2/objdetect.hpp>
#include <opencv2/opencv.hpp>

/**
 * @brief C++20 HOG-based pedestrian detection algorithm using Strategy pattern
 */
class HOGDetectionAlgorithm : public IDetectionAlgorithm {
    // Note: Concept validation disabled for GCC 11 compatibility
  private:
    cv::HOGDescriptor m_hogDescriptor;
    IDetectionAlgorithm::DetectionConfig m_config;
    bool m_isInitialized;

    // C++20 HOG-specific parameters with designated initializers
    struct HOGConfig {
        cv::Size winSize{64, 128};
        cv::Size blockSize{16, 16};
        cv::Size blockStride{8, 8};
        cv::Size cellSize{8, 8};
        int nbins{9};
        int derivAperture{1};
        double winSigma{-1.0};
        cv::HOGDescriptor::HistogramNormType histogramNormType{cv::HOGDescriptor::L2Hys};
        double L2HysThreshold{0.2};
        bool gammaCorrection{false};
        int nlevels{cv::HOGDescriptor::DEFAULT_NLEVELS};
        bool signedGradient{false};

        // C++20 constexpr validation
        constexpr bool isValid() const noexcept {
            return winSize.width > 0 && winSize.height > 0 && blockSize.width > 0 &&
                   blockSize.height > 0 && nbins > 0 && L2HysThreshold >= 0.0;
        }
    } m_hogConfig;

    // C++20 timing information
    mutable std::chrono::milliseconds m_lastProcessingTime{0};
    mutable std::chrono::steady_clock::time_point m_lastProcessingStart;

    /**
     * @brief Initialize HOG descriptor with configuration
     * @return global::RC_t Return code indicating success or failure
     */
    global::RC_t initializeHOG();

    /**
     * @brief Apply Non-Maximum Suppression to detections
     * @param detections Input detections
     * @param confidences Detection confidences
     * @param filteredDetections Output filtered detections
     * @return global::RC_t Return code indicating success or failure
     */
    global::RC_t applyNMS(const std::vector<cv::Rect>& detections,
                          const std::vector<float>& confidences,
                          std::vector<cv::Rect>& filteredDetections,
                          std::vector<float>& filteredConfidences);

    /**
     * @brief Calculate angle of detection relative to camera center
     * @param detection Bounding box of detection
     * @param frameSize Size of input frame
     * @return float Angle in degrees (-31 to +31 for 62° FOV)
     */
    float calculateAngle(const cv::Rect& detection, const cv::Size& frameSize);

    /**
     * @brief Calculate angular span of detection
     * @param detection Bounding box of detection
     * @param frameSize Size of input frame
     * @return float Angular span in degrees
     */
    float calculateDeltaAngle(const cv::Rect& detection, const cv::Size& frameSize);

    /**
     * @brief Apply straight line filtering to reduce false positives
     * @param frame Input frame
     * @param detection Detection bounding box
     * @return bool True if detection passes filter (few straight lines)
     */
    bool applyStraightLineFilter(const cv::Mat& frame, const cv::Rect& detection);

    /**
     * @brief Apply area filtering to remove small detections
     * @param detection Detection bounding box
     * @return bool True if detection passes area threshold
     */
    bool applyAreaFilter(const cv::Rect& detection);

  public:
    /**
     * @brief Constructor
     */
    HOGDetectionAlgorithm();

    /**
     * @brief Destructor
     */
    virtual ~HOGDetectionAlgorithm() = default;

    /**
     * @brief Detect pedestrians using C++20 span interface
     * @param frame Input frame for detection
     * @param results Output span of detection results
     * @return global::RC_t Return code indicating success or failure
     */
    global::RC_t detect(const cv::Mat& frame,
                        std::span<IDetectionAlgorithm::DetectionResult> results) override;

    /**
     * @brief Detect pedestrians with vector output (legacy interface)
     * @param frame Input frame for detection
     * @param results Output vector of detection results
     * @return global::RC_t Return code indicating success or failure
     */
    global::RC_t detectLegacy(const cv::Mat& frame,
                              std::vector<IDetectionAlgorithm::DetectionResult>& results) override;

    /**
     * @brief Detect pedestrians with ranges support
     * @param frame Input frame for detection
     * @return std::vector<IDetectionAlgorithm::DetectionResult> Detection results
     */
    std::vector<IDetectionAlgorithm::DetectionResult> detectRange(const cv::Mat& frame) override;

    /**
     * @brief Configure the HOG detection algorithm
     * @param config Configuration parameters
     * @return global::RC_t Return code indicating success or failure
     */
    global::RC_t configure(const IDetectionAlgorithm::DetectionConfig& config) override;

    /**
     * @brief Get algorithm name
     * @return std::string Algorithm identifier
     */
    std::string getName() const override { return "HOG_Pedestrian_Detector_C++20"; }

    /**
     * @brief Get current configuration
     * @return IDetectionAlgorithm::DetectionConfig Current configuration
     */
    IDetectionAlgorithm::DetectionConfig getCurrentConfig() const override { return m_config; }

    /**
     * @brief Get processing time statistics
     * @return std::chrono::milliseconds Last processing time
     */
    std::chrono::milliseconds getLastProcessingTime() const override {
        return m_lastProcessingTime;
    }

    /**
     * @brief Set HOG-specific configuration
     * @param hogConfig HOG-specific parameters
     * @return global::RC_t Return code indicating success or failure
     */
    global::RC_t setHOGConfig(const HOGConfig& hogConfig);

    /**
     * @brief Get current HOG configuration
     * @return HOGConfig Current HOG parameters
     */
    const HOGConfig& getHOGConfig() const { return m_hogConfig; }
};

#endif /* HOGDETECTIONALGORITHM_H */