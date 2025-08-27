/***************************************************************************
 *============= Copyright by Darmstadt University of Applied Sciences =======
 ****************************************************************************
 * Filename        : Concepts.h
 * Author          : Bharath Ramachandraiah (stbhrama@stud.h-da.de)
 * Description     : C++20 concepts for type safety and constraints
 *
 ****************************************************************************/

#ifndef CONCEPTS_H
#define CONCEPTS_H

#include <concepts>
#include <type_traits>
#include <opencv2/opencv.hpp>
#include "global.h"

namespace pedestrian_detection::concepts {

/**
 * @brief Concept for detection algorithms
 */
template<typename T>
concept DetectionAlgorithm = requires(T t, const cv::Mat& frame) {
    typename T::DetectionResult;
    typename T::DetectionConfig;
    
    { t.detect(frame, std::declval<std::vector<typename T::DetectionResult>&>()) } -> std::same_as<global::RC_t>;
    { t.configure(std::declval<const typename T::DetectionConfig&>()) } -> std::same_as<global::RC_t>;
    { t.getName() } -> std::convertible_to<std::string>;
};

/**
 * @brief Concept for observable objects
 */
template<typename T>
concept Observable = requires(T t) {
    { t.addObserver(std::declval<std::shared_ptr<class IObserver>>()) } -> std::same_as<global::RC_t>;
    { t.removeObserver(std::declval<std::shared_ptr<class IObserver>>()) } -> std::same_as<global::RC_t>;
    { t.notifyObservers(std::declval<std::shared_ptr<struct EventData>>()) } -> std::same_as<void>;
};

/**
 * @brief Concept for observer objects
 */
template<typename T>
concept Observer = requires(T t) {
    { t.onNotify(std::declval<std::shared_ptr<struct EventData>>()) } -> std::same_as<void>;
    { t.getObserverId() } -> std::convertible_to<std::string>;
};

/**
 * @brief Concept for command objects
 */
template<typename T>
concept Command = requires(T t) {
    { t.execute() } -> std::same_as<global::RC_t>;
    { t.getType() } -> std::same_as<enum class CommandType>;
    { t.getPriority() } -> std::same_as<enum class CommandPriority>;
    { t.getDescription() } -> std::convertible_to<std::string>;
    { t.getId() } -> std::same_as<uint32_t>;
};

/**
 * @brief Concept for undoable commands
 */
template<typename T>
concept UndoableCommand = Command<T> && requires(T t) {
    { t.undo() } -> std::same_as<global::RC_t>;
    { t.canUndo() } -> std::same_as<bool>;
};

/**
 * @brief Concept for service factories
 */
template<typename T>
concept ServiceFactory = requires(T t) {
    { t.getPlatformName() } -> std::convertible_to<std::string>;
    { t.getPlatformType() } -> std::same_as<enum class PlatformType>;
};

/**
 * @brief Concept for thread-safe objects
 */
template<typename T>
concept ThreadSafe = requires(T t) {
    requires std::is_move_constructible_v<T>;
    requires std::is_move_assignable_v<T>;
};

/**
 * @brief Concept for configuration values
 */
template<typename T>
concept ConfigurationValue = requires(T t) {
    requires std::is_copy_constructible_v<T>;
    requires std::is_copy_assignable_v<T>;
    requires std::is_default_constructible_v<T>;
};

/**
 * @brief Concept for numeric types
 */
template<typename T>
concept Numeric = std::integral<T> || std::floating_point<T>;

/**
 * @brief Concept for container types that can be used with OpenCV
 */
template<typename T>
concept OpenCVCompatible = requires(T t) {
    requires std::is_same_v<T, cv::Mat> || 
             std::is_same_v<T, cv::UMat> ||
             std::is_same_v<T, std::vector<cv::Point>> ||
             std::is_same_v<T, std::vector<cv::Rect>>;
};

/**
 * @brief Concept for error recovery strategies
 */
template<typename T>
concept ErrorRecoveryStrategy = requires(T t) {
    { t.recover(std::declval<const struct ErrorInfo&>()) } -> std::same_as<global::RC_t>;
    { t.canHandle(std::declval<const struct ErrorInfo&>()) } -> std::same_as<bool>;
    { t.getName() } -> std::convertible_to<std::string>;
};

/**
 * @brief Concept for log destinations
 */
template<typename T>
concept LogDestination = requires(T t) {
    { t.write(std::declval<const struct LogEntry&>()) } -> std::same_as<global::RC_t>;
    { t.flush() } -> std::same_as<global::RC_t>;
    { t.isEnabled() } -> std::same_as<bool>;
    { t.setEnabled(std::declval<bool>()) } -> std::same_as<void>;
};

/**
 * @brief Concept for resource management (RAII)
 */
template<typename T>
concept RAIIResource = requires(T t) {
    requires std::is_destructible_v<T>;
    requires !std::is_copy_constructible_v<T> || std::is_move_constructible_v<T>;
};

/**
 * @brief Concept for platform-specific components
 */
template<typename T>
concept PlatformSpecific = requires(T t) {
    { t.getPlatformType() } -> std::same_as<enum class PlatformType>;
    requires std::is_default_constructible_v<T>;
};

/**
 * @brief Concept for serializable data
 */
template<typename T>
concept Serializable = requires(T t) {
    requires std::is_trivially_copyable_v<T> || requires {
        { t.serialize() } -> std::convertible_to<std::vector<uint8_t>>;
        { T::deserialize(std::declval<const std::vector<uint8_t>&>()) } -> std::same_as<T>;
    };
};

/**
 * @brief Concept for frame processors
 */
template<typename T>
concept FrameProcessor = requires(T t, const cv::Mat& frame) {
    { t.process(frame) } -> std::same_as<global::RC_t>;
    requires OpenCVCompatible<decltype(frame)>;
};

/**
 * @brief Concept for algorithms with configurable parameters
 */
template<typename T, typename ConfigType>
concept ConfigurableAlgorithm = requires(T t, const ConfigType& config) {
    { t.configure(config) } -> std::same_as<global::RC_t>;
    { t.getConfiguration() } -> std::same_as<ConfigType>;
    requires ConfigurationValue<ConfigType>;
};

/**
 * @brief Concept for timing-aware components
 */
template<typename T>
concept TimingAware = requires(T t) {
    { t.getLastProcessingTime() } -> std::convertible_to<std::chrono::milliseconds>;
    { t.getAverageProcessingTime() } -> std::convertible_to<std::chrono::milliseconds>;
};

/**
 * @brief Concept for statistics collectors
 */
template<typename T>
concept StatisticsCollector = requires(T t) {
    { t.getStatistics() } -> std::is_class_v<decltype(t.getStatistics())>;
    { t.resetStatistics() } -> std::same_as<void>;
};

} // namespace pedestrian_detection::concepts

#endif /* CONCEPTS_H */