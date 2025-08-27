/***************************************************************************
 *============= Copyright by Darmstadt University of Applied Sciences =======
 ****************************************************************************
 * Filename        : RangesUtilities.h
 * Author          : Bharath Ramachandraiah (stbhrama@stud.h-da.de)
 * Description     : C++20 ranges and views for data processing
 *
 ****************************************************************************/

#ifndef RANGESUTILITIES_H
#define RANGESUTILITIES_H

#include <ranges>
#include <algorithm>
#include <numeric>
#include <functional>
#include <concepts>
#include <span>
#include <vector>
#include <array>
#include <opencv2/opencv.hpp>
#include "IDetectionAlgorithm.h"
#include "Concepts.h"

namespace pedestrian_detection::ranges {

namespace views = std::views;
namespace rng = std::ranges;

/**
 * @brief Custom view for filtering detection results by confidence
 */
struct ConfidenceFilterView : views::view_interface<ConfidenceFilterView> {
    std::span<const IDetectionAlgorithm::DetectionResult> detections;
    float minConfidence;

    ConfidenceFilterView(std::span<const IDetectionAlgorithm::DetectionResult> dets, float minConf)
        : detections(dets), minConfidence(minConf) {}

    auto begin() const {
        return detections 
            | views::filter([minConf = minConfidence](const auto& det) {
                return det.confidence >= minConf;
            })
            | rng::begin;
    }

    auto end() const {
        return detections 
            | views::filter([minConf = minConfidence](const auto& det) {
                return det.confidence >= minConf;
            })
            | rng::end;
    }
};

/**
 * @brief Custom view for extracting bounding boxes from detections
 */
struct BoundingBoxView : views::view_interface<BoundingBoxView> {
    std::span<const IDetectionAlgorithm::DetectionResult> detections;

    explicit BoundingBoxView(std::span<const IDetectionAlgorithm::DetectionResult> dets)
        : detections(dets) {}

    auto begin() const {
        return detections 
            | views::transform([](const auto& det) { return det.boundingBox; })
            | rng::begin;
    }

    auto end() const {
        return detections 
            | views::transform([](const auto& det) { return det.boundingBox; })
            | rng::end;
    }
};

/**
 * @brief Utility functions using C++20 ranges
 */
class DetectionProcessor {
public:
    /**
     * @brief Filter detections by confidence threshold using ranges
     * @param detections Input detection results
     * @param threshold Minimum confidence threshold
     * @return auto Filtered detections view
     */
    static auto filterByConfidence(std::span<const IDetectionAlgorithm::DetectionResult> detections, 
                                  float threshold) {
        return detections 
            | views::filter([threshold](const auto& det) {
                return det.confidence >= threshold;
            });
    }

    /**
     * @brief Get top N detections by confidence
     * @param detections Input detection results
     * @param n Number of top detections to return
     * @return auto Top N detections view
     */
    static auto getTopN(std::span<const IDetectionAlgorithm::DetectionResult> detections, size_t n) {
        return detections 
            | views::take(n)
            | views::transform([](const auto& det) { return det; });
    }

    /**
     * @brief Calculate average confidence using ranges
     * @param detections Input detection results
     * @return float Average confidence
     */
    static float calculateAverageConfidence(std::span<const IDetectionAlgorithm::DetectionResult> detections) {
        if (detections.empty()) return 0.0f;

        auto confidences = detections 
            | views::transform([](const auto& det) { return det.confidence; });
        
        return std::reduce(confidences.begin(), confidences.end(), 0.0f) / detections.size();
    }

    /**
     * @brief Group detections by angle ranges
     * @param detections Input detection results
     * @param angleStep Angle range step size
     * @return std::vector<std::vector<IDetectionAlgorithm::DetectionResult>> Grouped detections
     */
    static auto groupByAngle(std::span<const IDetectionAlgorithm::DetectionResult> detections, 
                            float angleStep = 10.0f) {
        std::map<int, std::vector<IDetectionAlgorithm::DetectionResult>> groups;
        
        for (const auto& det : detections) {
            int angleGroup = static_cast<int>(det.angle / angleStep);
            groups[angleGroup].push_back(det);
        }
        
        return groups | views::values;
    }

    /**
     * @brief Extract bounding box centers using ranges
     * @param detections Input detection results
     * @return auto Centers view
     */
    static auto extractCenters(std::span<const IDetectionAlgorithm::DetectionResult> detections) {
        return detections 
            | views::transform([](const auto& det) {
                return cv::Point2f(
                    det.boundingBox.x + det.boundingBox.width / 2.0f,
                    det.boundingBox.y + det.boundingBox.height / 2.0f
                );
            });
    }

    /**
     * @brief Filter detections within a specific area using ranges
     * @param detections Input detection results
     * @param area Region of interest
     * @return auto Filtered detections view
     */
    static auto filterByArea(std::span<const IDetectionAlgorithm::DetectionResult> detections, 
                            const cv::Rect& area) {
        return detections 
            | views::filter([area](const auto& det) {
                return (area & det.boundingBox).area() > 0;
            });
    }

    /**
     * @brief Transform detections to different coordinate system
     * @param detections Input detection results
     * @param transform Transformation function
     * @return auto Transformed detections view
     */
    template<std::invocable<const IDetectionAlgorithm::DetectionResult&> Transform>
    static auto transformDetections(std::span<const IDetectionAlgorithm::DetectionResult> detections,
                                   Transform transform) {
        return detections | views::transform(transform);
    }

    /**
     * @brief Calculate detection statistics using ranges algorithms
     * @param detections Input detection results
     * @return DetectionStats Statistics structure
     */
    struct DetectionStats {
        size_t count;
        float minConfidence;
        float maxConfidence;
        float averageConfidence;
        float minAngle;
        float maxAngle;
        cv::Size averageSize;
    };

    static DetectionStats calculateStatistics(std::span<const IDetectionAlgorithm::DetectionResult> detections) {
        if (detections.empty()) {
            return DetectionStats{};
        }

        auto confidences = detections | views::transform(&IDetectionAlgorithm::DetectionResult::confidence);
        auto angles = detections | views::transform(&IDetectionAlgorithm::DetectionResult::angle);
        auto sizes = detections | views::transform([](const auto& det) { 
            return cv::Size(det.boundingBox.width, det.boundingBox.height); 
        });

        auto [minConf, maxConf] = rng::minmax_element(confidences);
        auto [minAngle, maxAngle] = rng::minmax_element(angles);
        
        float avgConfidence = std::reduce(confidences.begin(), confidences.end(), 0.0f) / detections.size();
        
        // Calculate average size
        int totalWidth = std::reduce(sizes.begin(), sizes.end(), 0,
            [](int sum, const cv::Size& size) { return sum + size.width; });
        int totalHeight = std::reduce(sizes.begin(), sizes.end(), 0,
            [](int sum, const cv::Size& size) { return sum + size.height; });

        return DetectionStats{
            .count = detections.size(),
            .minConfidence = *minConf,
            .maxConfidence = *maxConf,
            .averageConfidence = avgConfidence,
            .minAngle = *minAngle,
            .maxAngle = *maxAngle,
            .averageSize = cv::Size(totalWidth / detections.size(), totalHeight / detections.size())
        };
    }

    /**
     * @brief Batch process multiple detection result sets using ranges
     * @param detectionSets Multiple sets of detection results
     * @param processor Processing function
     * @return auto Processed results view
     */
    template<std::invocable<std::span<const IDetectionAlgorithm::DetectionResult>> Processor>
    static auto batchProcess(std::span<const std::vector<IDetectionAlgorithm::DetectionResult>> detectionSets,
                            Processor processor) {
        return detectionSets 
            | views::transform([processor](const auto& detSet) {
                return processor(std::span<const IDetectionAlgorithm::DetectionResult>(detSet));
            });
    }

    /**
     * @brief Sliding window processing over detection history
     * @param detectionHistory Historical detection results
     * @param windowSize Size of sliding window
     * @return auto Sliding window view
     */
    static auto slidingWindow(std::span<const std::vector<IDetectionAlgorithm::DetectionResult>> detectionHistory,
                             size_t windowSize) {
        return detectionHistory 
            | views::slide(windowSize)
            | views::transform([](auto window) {
                std::vector<IDetectionAlgorithm::DetectionResult> combined;
                for (const auto& detSet : window) {
                    combined.insert(combined.end(), detSet.begin(), detSet.end());
                }
                return combined;
            });
    }

    /**
     * @brief Parallel processing using ranges (when available)
     * @param detections Input detection results
     * @param processor Processing function
     * @return auto Processed results
     */
    template<std::invocable<const IDetectionAlgorithm::DetectionResult&> Processor>
    static auto parallelProcess(std::span<const IDetectionAlgorithm::DetectionResult> detections,
                               Processor processor) {
        // Note: std::execution::par is not yet widely supported in ranges
        // This is a placeholder for future parallel ranges support
        return detections | views::transform(processor);
    }
};

/**
 * @brief Range adaptor for creating confidence filter view
 */
constexpr auto confidence_filter = [](float threshold) {
    return views::filter([threshold](const auto& det) {
        return det.confidence >= threshold;
    });
};

/**
 * @brief Range adaptor for creating bounding box view
 */
constexpr auto bounding_boxes = views::transform([](const auto& det) {
    return det.boundingBox;
});

/**
 * @brief Range adaptor for creating angle view
 */
constexpr auto angles = views::transform([](const auto& det) {
    return det.angle;
});

/**
 * @brief Range adaptor for creating confidence view
 */
constexpr auto confidences = views::transform([](const auto& det) {
    return det.confidence;
});

} // namespace pedestrian_detection::ranges

#endif /* RANGESUTILITIES_H */