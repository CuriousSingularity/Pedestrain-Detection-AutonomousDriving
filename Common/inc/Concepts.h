/***************************************************************************
 *============= Copyright by Darmstadt University of Applied Sciences =======
 ****************************************************************************
 * Filename        : Concepts.h
 * Author          : Bharath Ramachandraiah (stbhrama@stud.h-da.de)
 * Description     : Simplified C++20 concepts for GCC 11 compatibility
 *
 ****************************************************************************/

#ifndef CONCEPTS_H
#define CONCEPTS_H

#include "global.h"

#include <concepts>
#include <type_traits>

#include <opencv2/opencv.hpp>

namespace pedestrian_detection::concepts {

// Platform type enum for concepts
enum class PlatformType { NVIDIA_PLATFORM = 1, RSP_PLATFORM = 2, PC_PLATFORM = 3 };

/**
 * @brief Simplified concept for detection algorithms
 */
template <typename T>
concept DetectionAlgorithm = requires(T t, const cv::Mat& frame) {
    t.getName();
    t.getCurrentConfig();
    t.getLastProcessingTime();
};

/**
 * @brief Simplified concept for observable objects
 */
template <typename T>
concept Observable = requires(T t) { t.notifyObservers(); };

/**
 * @brief Simplified concept for observer objects
 */
template <typename T>
concept Observer = requires(T t) { t.update(); };

/**
 * @brief Simplified concept for configuration values
 */
template <typename T>
concept ConfigurationValue = std::is_default_constructible_v<T> && std::is_copy_constructible_v<T>;

/**
 * @brief Simplified concept for command objects
 */
template <typename T>
concept Command = requires(T t) {
    t.execute();
    t.undo();
};

/**
 * @brief Simplified concept for platform-specific components
 */
template <typename T>
concept PlatformSpecific = requires(T t) {
    t.getPlatformType();
    requires std::is_default_constructible_v<T>;
};

/**
 * @brief Simplified concept for statistics collectors
 */
template <typename T>
concept StatisticsCollector = requires(T t) {
    t.getStatistics();
    t.resetStatistics();
};

}  // namespace pedestrian_detection::concepts

#endif /* CONCEPTS_H */