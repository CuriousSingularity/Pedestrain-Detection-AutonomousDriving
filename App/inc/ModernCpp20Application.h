/***************************************************************************
 *============= Copyright by Darmstadt University of Applied Sciences =======
 ****************************************************************************
 * Filename        : ModernCpp20Application.h
 * Author          : Bharath Ramachandraiah (stbhrama@stud.h-da.de)
 * Description     : C++20 modern application with all latest features
 *
 ****************************************************************************/

#ifndef MODERNCPP20APPLICATION_H
#define MODERNCPP20APPLICATION_H

#include "../../Common/inc/IServiceFactory.h"
#include "../../Common/inc/IDetectionAlgorithm.h"
#include "../../Common/inc/IObserver.h"
#include "../../Common/inc/ObservableSubject.h"
#include "../../Common/inc/AsyncDetection.h"
#include "../../Common/inc/RangesUtilities.h"
#include "../../Common/inc/ModernConstants.h"
#include "../../Common/inc/Concepts.h"
#include "../../Communication/inc/CommandQueue.h"
#include <memory>
#include <atomic>
#include <span>
#include <ranges>
#include <format>
#include <coroutine>
#include <chrono>
#include <string_view>
#include <numbers>

namespace pedestrian_detection::modern {

/**
 * @brief C++20 Application Statistics with designated initializers
 */
struct ApplicationStatistics {
    uint64_t totalFramesProcessed{0};
    uint64_t totalDetections{0};
    uint64_t commandsExecuted{0};
    uint64_t errorsHandled{0};
    std::chrono::milliseconds totalProcessingTime{0};
    std::chrono::steady_clock::time_point startTime{std::chrono::steady_clock::now()};
    
    // C++20 constexpr methods
    constexpr double getAverageProcessingTime() const noexcept {
        return totalFramesProcessed > 0 ? 
               static_cast<double>(totalProcessingTime.count()) / totalFramesProcessed : 0.0;
    }
    
    constexpr std::chrono::milliseconds getUptime() const noexcept {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now() - startTime);
    }
    
    // C++20 formatted output
    std::string toString() const {
        return std::format(
            "Frames: {}, Detections: {}, Commands: {}, Errors: {}, "
            "Avg Processing: {:.2f}ms, Uptime: {}ms",
            totalFramesProcessed, totalDetections, commandsExecuted, 
            errorsHandled, getAverageProcessingTime(), getUptime().count()
        );
    }
};

/**
 * @brief C++20 Configuration with concepts validation
 */
template<pedestrian_detection::concepts::ConfigurationValue T>
struct ModernConfiguration {
    T detectionConfig{};
    std::string_view platformName{};
    bool enableLogging{true};
    std::chrono::milliseconds frameInterval{33}; // ~30 FPS
    size_t maxDetectionResults{constants::MAX_DETECTION_RESULTS};
    
    // C++20 spaceship operator for comparison
    auto operator<=>(const ModernConfiguration&) const = default;
    
    // Constexpr validation
    constexpr bool isValid() const noexcept requires requires(T t) { t.isValid(); } {
        return frameInterval.count() > 0 && 
               maxDetectionResults > 0 &&
               detectionConfig.isValid();
    }
};

/**
 * @brief C++20 Modern Application using all latest features
 */
class ModernCpp20Application : public IObserver {
    // C++20 concepts for template parameters
    template<concepts::DetectionAlgorithm Algorithm>
    friend class ApplicationBuilder;

private:
    // Core components with smart pointers
    std::unique_ptr<IServiceFactory> m_serviceFactory;
    std::shared_ptr<IDetectionAlgorithm> m_detectionAlgorithm;
    std::unique_ptr<async::AsyncDetectionService> m_asyncDetectionService;
    std::unique_ptr<CommandQueue> m_commandQueue;
    std::shared_ptr<ObservableSubject> m_eventSubject;
    
    // C++20 atomic and configuration
    std::atomic<bool> m_isRunning{false};
    std::atomic<bool> m_shutdownRequested{false};
    ModernConfiguration<IDetectionAlgorithm::DetectionConfig> m_config;
    
    // Statistics with atomic operations
    mutable std::atomic<ApplicationStatistics> m_statistics{};
    
    // C++20 ranges for frame processing
    std::vector<cv::Mat> m_frameBuffer;
    static constexpr size_t FRAME_BUFFER_SIZE = 10;

public:
    /**
     * @brief Constructor with designated initializers support
     */
    explicit ModernCpp20Application(const ModernConfiguration<IDetectionAlgorithm::DetectionConfig>& config = {})
        : m_config{config} {
        initializeServices();
    }

    /**
     * @brief Destructor with automatic cleanup
     */
    ~ModernCpp20Application() {
        if (m_isRunning.load()) {
            stop().value_or(global::RC_ERROR);
        }
    }

    // C++20 move semantics
    ModernCpp20Application(const ModernCpp20Application&) = delete;
    ModernCpp20Application& operator=(const ModernCpp20Application&) = delete;
    ModernCpp20Application(ModernCpp20Application&&) = default;
    ModernCpp20Application& operator=(ModernCpp20Application&&) = default;

    /**
     * @brief Initialize application with concepts validation
     * @return std::optional<global::RC_t> Result or nullopt on success
     */
    [[nodiscard]] std::optional<global::RC_t> initialize() 
        requires concepts::ThreadSafe<ModernCpp20Application> {
        
        if (!m_config.isValid()) {
            return global::RC_ERROR_BAD_PARAM;
        }
        
        // C++20 coroutine initialization
        return initializeAsync().get();
    }

    /**
     * @brief Start application using coroutines
     * @return async::Task<global::RC_t> Async result
     */
    async::Task<global::RC_t> startAsync() {
        if (m_isRunning.exchange(true)) {
            co_return global::RC_ERROR_INVALID_STATE;
        }
        
        // Start async detection service
        if (m_asyncDetectionService) {
            auto frameSource = [this]() -> cv::Mat {
                return getNextFrame();
            };
            
            // Start frame processing coroutine
            auto processingGenerator = m_asyncDetectionService->processFrameStream(
                frameSource, m_config.frameInterval);
                
            // Process detection results
            for (auto results : processingGenerator) {
                co_await processDetectionResults(std::move(results));
            }
        }
        
        co_return global::RC_SUCCESS;
    }

    /**
     * @brief Stop application gracefully
     * @return std::optional<global::RC_t> Error code if any
     */
    [[nodiscard]] std::optional<global::RC_t> stop() noexcept {
        if (!m_isRunning.exchange(false)) {
            return std::nullopt; // Already stopped
        }
        
        m_shutdownRequested.store(true);
        
        if (m_asyncDetectionService) {
            m_asyncDetectionService->stop();
        }
        
        return std::nullopt;
    }

    /**
     * @brief Process frames using C++20 ranges
     * @param frames Span of input frames
     * @return Generator of detection results
     */
    async::Generator<std::vector<IDetectionAlgorithm::DetectionResult>> 
    processFramesRange(std::span<const cv::Mat> frames) {
        using namespace std::ranges;
        using namespace pedestrian_detection::ranges;
        
        for (const auto& frame : frames | views::take(m_config.maxDetectionResults)) {
            if (m_shutdownRequested.load()) {
                co_return;
            }
            
            auto results = co_await async::FrameAwaitable{frame, m_detectionAlgorithm};
            
            // Use ranges to filter and process results
            auto filteredResults = results 
                | confidence_filter(0.5f)
                | views::take(static_cast<std::ptrdiff_t>(m_config.maxDetectionResults))
                | ranges::to<std::vector>();
            
            updateStatistics(true, filteredResults.size());
            co_yield filteredResults;
        }
    }

    /**
     * @brief Get statistics with C++20 formatting
     * @return std::string Formatted statistics
     */
    [[nodiscard]] std::string getFormattedStatistics() const {
        const auto stats = m_statistics.load();
        return std::format(
            "Application Statistics:\n"
            "  Frames Processed: {:L}\n"
            "  Total Detections: {:L}\n"
            "  Commands Executed: {:L}\n"
            "  Errors Handled: {:L}\n"
            "  Average Processing Time: {:.2f}ms\n"
            "  Uptime: {}",
            stats.totalFramesProcessed,
            stats.totalDetections,
            stats.commandsExecuted,
            stats.errorsHandled,
            stats.getAverageProcessingTime(),
            stats.getUptime()
        );
    }

    /**
     * @brief Observer pattern implementation
     * @param eventData Event data
     */
    void onNotify(std::shared_ptr<EventData> eventData) override {
        if (!eventData) return;
        
        // C++20 pattern matching simulation
        switch (eventData->type) {
            case EventType::DETECTION_RESULT:
                if (auto detectionEvent = std::dynamic_pointer_cast<DetectionEventData>(eventData)) {
                    handleDetectionEvent(*detectionEvent);
                }
                break;
            case EventType::ERROR_OCCURRED:
                if (auto errorEvent = std::dynamic_pointer_cast<ErrorEventData>(eventData)) {
                    handleErrorEvent(*errorEvent);
                }
                break;
            default:
                // Handle other events
                break;
        }
    }

    /**
     * @brief Get observer ID
     * @return std::string Observer identifier
     */
    std::string getObserverId() const override {
        return std::format("ModernCpp20Application_{}", 
                          static_cast<void*>(const_cast<ModernCpp20Application*>(this)));
    }

    /**
     * @brief Check if application is running
     * @return bool Running status
     */
    [[nodiscard]] bool isRunning() const noexcept {
        return m_isRunning.load();
    }

    /**
     * @brief Request shutdown
     */
    void requestShutdown() noexcept {
        m_shutdownRequested.store(true);
    }

    /**
     * @brief Get configuration
     * @return const auto& Configuration reference
     */
    [[nodiscard]] const auto& getConfiguration() const noexcept {
        return m_config;
    }

private:
    /**
     * @brief Initialize services
     */
    void initializeServices() {
        // Create service factory
        m_serviceFactory = ServiceFactoryManager::createFactoryForCurrentPlatform();
        
        // Create detection algorithm
        m_detectionAlgorithm = m_serviceFactory->createDetectionAlgorithm();
        
        // Create async detection service
        m_asyncDetectionService = std::make_unique<async::AsyncDetectionService>(m_detectionAlgorithm);
        
        // Create event subject
        m_eventSubject = std::make_shared<ObservableSubject>();
        m_eventSubject->addObserver(shared_from_this(), EventType::DETECTION_RESULT);
    }

    /**
     * @brief Async initialization coroutine
     * @return async::Task<global::RC_t> Initialization result
     */
    async::Task<global::RC_t> initializeAsync() {
        // Configure detection algorithm
        if (auto result = m_detectionAlgorithm->configure(m_config.detectionConfig); 
            result != global::RC_SUCCESS) {
            co_return result;
        }
        
        // Initialize frame buffer
        m_frameBuffer.reserve(FRAME_BUFFER_SIZE);
        
        co_return global::RC_SUCCESS;
    }

    /**
     * @brief Process detection results coroutine
     * @param results Detection results
     * @return async::Task<void> Processing task
     */
    async::Task<void> processDetectionResults(std::vector<IDetectionAlgorithm::DetectionResult> results) {
        // Create detection event
        auto detectionEvent = std::make_shared<DetectionEventData>(
            results, m_detectionAlgorithm->getName(), 
            static_cast<float>(m_detectionAlgorithm->getLastProcessingTime().count())
        );
        
        // Notify observers
        m_eventSubject->notifyObservers(detectionEvent);
        
        co_return;
    }

    /**
     * @brief Get next frame (placeholder implementation)
     * @return cv::Mat Next frame
     */
    cv::Mat getNextFrame() {
        // Placeholder - in real implementation, this would get frame from camera
        return cv::Mat::zeros(640, 480, CV_8UC3);
    }

    /**
     * @brief Update statistics atomically
     * @param frameProcessed Whether frame was processed
     * @param detectionCount Number of detections
     */
    void updateStatistics(bool frameProcessed, size_t detectionCount) noexcept {
        auto currentStats = m_statistics.load();
        
        if (frameProcessed) {
            currentStats.totalFramesProcessed++;
            currentStats.totalDetections += detectionCount;
        }
        
        m_statistics.store(currentStats);
    }

    /**
     * @brief Handle detection events
     * @param event Detection event data
     */
    void handleDetectionEvent(const DetectionEventData& event) {
        // Process detection results using ranges
        using namespace pedestrian_detection::ranges;
        
        auto highConfidenceDetections = event.detections 
            | confidence_filter(0.7f)
            | std::ranges::to<std::vector>();
        
        // Log high confidence detections
        if (!highConfidenceDetections.empty()) {
            auto stats = DetectionProcessor::calculateStatistics(
                std::span<const IDetectionAlgorithm::DetectionResult>(highConfidenceDetections)
            );
            
            // Use std::format for logging
            auto logMessage = std::format(
                "High confidence detections: {}, avg confidence: {:.2f}",
                stats.count, stats.averageConfidence
            );
            
            // Log message (placeholder)
        }
    }

    /**
     * @brief Handle error events
     * @param event Error event data
     */
    void handleErrorEvent(const ErrorEventData& event) {
        auto currentStats = m_statistics.load();
        currentStats.errorsHandled++;
        m_statistics.store(currentStats);
        
        // Handle critical errors
        if (event.severity == ErrorEventData::Severity::CRITICAL) {
            requestShutdown();
        }
    }
};

/**
 * @brief Builder pattern for application creation
 */
template<concepts::DetectionAlgorithm Algorithm>
class ApplicationBuilder {
private:
    ModernConfiguration<IDetectionAlgorithm::DetectionConfig> m_config{};

public:
    ApplicationBuilder& withDetectionConfig(const IDetectionAlgorithm::DetectionConfig& config) {
        m_config.detectionConfig = config;
        return *this;
    }
    
    ApplicationBuilder& withFrameInterval(std::chrono::milliseconds interval) {
        m_config.frameInterval = interval;
        return *this;
    }
    
    ApplicationBuilder& withMaxDetections(size_t maxDetections) {
        m_config.maxDetectionResults = maxDetections;
        return *this;
    }
    
    ApplicationBuilder& withLogging(bool enable) {
        m_config.enableLogging = enable;
        return *this;
    }
    
    [[nodiscard]] std::unique_ptr<ModernCpp20Application> build() {
        return std::make_unique<ModernCpp20Application>(m_config);
    }
};

} // namespace pedestrian_detection::modern

#endif /* MODERNCPP20APPLICATION_H */