# 🚀 C++20 Usage Examples

This document demonstrates how to use the modernized C++20 features in the pedestrian detection system.

## 1. 🔒 Using Concepts for Type Safety

```cpp
#include "Common/inc/Concepts.h"
#include "Detection/inc/HOGDetectionAlgorithm.h"

// Function that only accepts detection algorithms
template<pedestrian_detection::concepts::DetectionAlgorithm T>
void processWithAlgorithm(T& algorithm, const cv::Mat& frame) {
    auto results = algorithm.detectRange(frame);
    // Process results...
}

// Usage
HOGDetectionAlgorithm hog;
processWithAlgorithm(hog, frame);  // ✓ Compiles
// processWithAlgorithm(someOtherClass, frame);  // ✗ Compile error
```

## 2. ♾️ Coroutines for Async Processing

```cpp
#include "Common/inc/AsyncDetection.h"

using namespace pedestrian_detection::async;

// Async frame processing
Task<std::vector<IDetectionAlgorithm::DetectionResult>> processFrameAsync() {
    cv::Mat frame = captureFrame();
    auto service = createAsyncDetectionService<HOGDetectionAlgorithm>();
    
    co_return co_await service->processFrameAsync(std::move(frame));
}

// Streaming detection with generator
void streamingDetection() {
    auto service = createAsyncDetectionService<HOGDetectionAlgorithm>();
    auto frameSource = []() { return captureFrame(); };
    
    for (auto results : service->processFrameStream(frameSource)) {
        // Process each detection result as it arrives
        std::println("Detected {} objects", results.size());
    }
}
```

## 3. 🔢 Ranges for Data Processing

```cpp
#include "Common/inc/RangesUtilities.h"

using namespace pedestrian_detection::ranges;
using namespace std::ranges;

void processDetections(std::span<const IDetectionAlgorithm::DetectionResult> detections) {
    // Filter high-confidence detections using ranges
    auto highConfidence = detections 
        | confidence_filter(0.8f)
        | views::take(10);  // Take top 10
    
    // Extract bounding boxes
    auto boxes = highConfidence | bounding_boxes;
    
    // Calculate average confidence
    auto avgConfidence = DetectionProcessor::calculateAverageConfidence(detections);
    
    // Group by angle ranges
    auto grouped = DetectionProcessor::groupByAngle(detections, 15.0f);
    
    // Transform to centers
    auto centers = detections | extractCenters;
}
```

## 4. ⚖️ Modern Constants and Compile-time Evaluation

```cpp
#include "Common/inc/ModernConstants.h"

using namespace pedestrian_detection::constants;

// Compile-time calculations
constexpr auto angleInRadians = degreesToRadians(45.0f);
constexpr auto optimalSize = calculateOptimalSize(640, 480, 1.2f);

// Constexpr validation
static_assert(AlgorithmConstants::isValidThreshold(0.5f));
static_assert(MathUtils::calculateAngle(320, 320, 640) == 0.0f);

// Runtime usage with compile-time constants
void setupDetection() {
    auto config = IDetectionAlgorithm::DetectionConfig{
        .hitThreshold = AlgorithmConstants::DEFAULT_HIT_THRESHOLD,
        .scaleFactor = AlgorithmConstants::DEFAULT_SCALE_FACTOR,
        .minNeighbors = AlgorithmConstants::DEFAULT_MIN_NEIGHBORS
    };
}
```

## 5. ✨ Modern Application with C++20 Features

```cpp
#include "App/inc/ModernCpp20Application.h"

using namespace pedestrian_detection::modern;

int main() {
    // Create application with designated initializers
    auto config = ModernConfiguration<IDetectionAlgorithm::DetectionConfig>{
        .detectionConfig = {
            .hitThreshold = 0.6f,
            .scaleFactor = 1.1f,
            .minNeighbors = 3
        },
        .enableLogging = true,
        .frameInterval = std::chrono::milliseconds{33},
        .maxDetectionResults = 20
    };
    
    // Build application using builder pattern
    auto app = ApplicationBuilder<HOGDetectionAlgorithm>{}
        .withDetectionConfig(config.detectionConfig)
        .withFrameInterval(std::chrono::milliseconds{50})
        .withMaxDetections(15)
        .withLogging(true)
        .build();
    
    // Initialize and start
    if (auto error = app->initialize()) {
        std::println("Failed to initialize: {}", 
                    constants::getErrorDescription(*error));
        return -1;
    }
    
    // Start async processing
    auto startTask = app->startAsync();
    
    // Get formatted statistics
    std::println("{}", app->getFormattedStatistics());
    
    return 0;
}
```

## 6. 📝 Using std::format for Logging

```cpp
#include <format>
#include <print>  // C++23 preview

void logDetectionResults(const std::vector<IDetectionAlgorithm::DetectionResult>& results) {
    // Modern formatted output
    std::println("Detection Results:");
    
    for (const auto& [index, result] : std::views::enumerate(results)) {
        std::println("  [{}] Confidence: {:.2f}, Angle: {:.1f}°, Box: {}x{}", 
                    index, 
                    result.confidence, 
                    result.angle,
                    result.boundingBox.width,
                    result.boundingBox.height);
    }
    
    // Summary with formatting
    auto summary = std::format(
        "Summary: {} detections, avg confidence: {:.2f}",
        results.size(),
        std::ranges::fold_left(results | ranges::confidences, 0.0f, std::plus{}) / results.size()
    );
    
    std::println("{}", summary);
}
```

## 7. 🏗️ Structured Bindings and Modern Syntax

```cpp
void modernSyntaxExample() {
    // Structured bindings
    auto detectionResults = detectPedestrians();
    auto [count, avgConfidence, maxAngle] = analyzeResults(detectionResults);
    
    // Range-based processing with structured bindings
    for (const auto& [confidence, angle, box] : 
         detectionResults | views::transform([](const auto& det) {
             return std::tuple{det.confidence, det.angle, det.boundingBox};
         })) {
        
        if (confidence > 0.8f) {
            processHighConfidenceDetection(angle, box);
        }
    }
    
    // Modern error handling
    if (auto result = processFrame(); result.has_value()) {
        auto& [detections, processingTime] = *result;
        logResults(detections, processingTime);
    }
}
```

## 8. 🎨 Concepts-Constrained Templates

```cpp
template<concepts::DetectionAlgorithm Algorithm, 
         concepts::ConfigurationValue Config>
requires concepts::ConfigurableAlgorithm<Algorithm, Config>
class OptimizedProcessor {
public:
    OptimizedProcessor(Algorithm algo, Config config) 
        : m_algorithm{std::move(algo)}, m_config{config} {
        
        // Compile-time validation
        static_assert(requires { config.isValid(); });
        
        m_algorithm.configure(m_config);
    }
    
    auto processFrame(const cv::Mat& frame) 
        -> std::expected<std::vector<typename Algorithm::DetectionResult>, global::RC_t> {
        
        if (!frame.empty()) {
            return m_algorithm.detectRange(frame);
        }
        return std::unexpected(global::RC_ERROR_BAD_PARAM);
    }
    
private:
    Algorithm m_algorithm;
    Config m_config;
};
```

## 9. ⏱️ Performance Monitoring with Chrono

```cpp
#include <chrono>

class PerformanceMonitor {
public:
    template<std::invocable F>
    auto timeOperation(F&& operation) {
        using namespace std::chrono;
        
        auto start = high_resolution_clock::now();
        auto result = std::invoke(std::forward<F>(operation));
        auto end = high_resolution_clock::now();
        
        auto duration = duration_cast<microseconds>(end - start);
        
        m_measurements.push_back(duration);
        
        return std::pair{std::move(result), duration};
    }
    
    auto getStatistics() const {
        using namespace std::ranges;
        
        if (m_measurements.empty()) return std::nullopt;
        
        auto [min, max] = minmax_element(m_measurements);
        auto avg = fold_left(m_measurements, std::chrono::microseconds{0}, std::plus{}) / m_measurements.size();
        
        return std::optional{std::tuple{*min, *max, avg}};
    }
    
private:
    std::vector<std::chrono::microseconds> m_measurements;
};
```

## 10. 🚨 Modern Error Handling

```cpp
#include <expected>  // C++23 feature
#include <optional>

// Using std::expected for error handling
std::expected<cv::Mat, global::RC_t> loadFrame(const std::string& path) {
    cv::Mat frame = cv::imread(path);
    if (frame.empty()) {
        return std::unexpected(global::RC_ERROR_OPEN);
    }
    return frame;
}

// Modern error propagation
auto processImagePipeline(const std::string& imagePath) 
    -> std::expected<std::vector<IDetectionAlgorithm::DetectionResult>, global::RC_t> {
    
    // Chain operations with error propagation
    auto frame = loadFrame(imagePath);
    if (!frame) return std::unexpected(frame.error());
    
    auto algorithm = createDetectionAlgorithm();
    if (!algorithm) return std::unexpected(global::RC_ERROR_MEMORY);
    
    return algorithm->detectRange(*frame);
}
```

These examples showcase the modern C++20 features integrated into the pedestrian detection system, providing better type safety, performance, and maintainability.