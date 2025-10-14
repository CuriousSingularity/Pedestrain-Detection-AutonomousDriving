/***************************************************************************
 *============= Copyright by Darmstadt University of Applied Sciences =======
 ****************************************************************************
 * Filename        : ModernApplication.h
 * Author          : Bharath Ramachandraiah (stbhrama@stud.h-da.de)
 * Description     : Refactored application using modern design patterns
 *
 ****************************************************************************/

#ifndef MODERNAPPLICATION_H
#define MODERNAPPLICATION_H

#include "../../Common/inc/ConfigurationManager.h"
#include "../../Common/inc/ErrorHandler.h"
#include "../../Common/inc/ICommand.h"
#include "../../Common/inc/IDetectionAlgorithm.h"
#include "../../Common/inc/IObserver.h"
#include "../../Common/inc/IServiceFactory.h"
#include "../../Common/inc/Logger.h"
#include "../../Common/inc/ObservableSubject.h"
#include "../../Communication/inc/CommandQueue.h"
#include "../../OS/inc/CThread.h"

#include <atomic>
#include <memory>

/**
 * @brief Modern application class using design patterns and SOLID principles
 */
class ModernApplication : public IObserver {
  private:
    // Core components
    std::unique_ptr<IServiceFactory> m_serviceFactory;
    std::unique_ptr<IDetectionAlgorithm> m_detectionAlgorithm;
    std::unique_ptr<CommandQueue> m_commandQueue;
    std::shared_ptr<ObservableSubject> m_eventSubject;

    // Service threads
    std::unique_ptr<CThread> m_detectionThread;
    std::unique_ptr<CThread> m_cameraThread;
    std::unique_ptr<CThread> m_communicationThread;
    std::unique_ptr<CThread> m_commandProcessorThread;

    // Application state
    std::atomic<bool> m_isRunning;
    std::atomic<bool> m_shutdownRequested;
    PlatformType m_platformType;
    std::string m_configFile;

    // Statistics
    struct ApplicationStats {
        uint64_t totalFramesProcessed;
        uint64_t totalDetections;
        uint64_t commandsExecuted;
        uint64_t errorsHandled;
        double averageProcessingTime;
    } m_stats;

    mutable std::mutex m_statsMutex;

  public:
    /**
     * @brief Constructor
     * @param configFile Path to configuration file
     */
    explicit ModernApplication(const std::string& configFile = "config.ini");

    /**
     * @brief Destructor
     */
    virtual ~ModernApplication();

    /**
     * @brief Initialize the application
     * @return global::RC_t Return code indicating success or failure
     */
    global::RC_t initialize();

    /**
     * @brief Start the application
     * @return global::RC_t Return code indicating success or failure
     */
    global::RC_t start();

    /**
     * @brief Stop the application gracefully
     * @return global::RC_t Return code indicating success or failure
     */
    global::RC_t stop();

    /**
     * @brief Run the application (blocking call)
     * @return global::RC_t Return code indicating final result
     */
    global::RC_t run();

    /**
     * @brief Check if application is running
     * @return bool True if running
     */
    bool isRunning() const { return m_isRunning.load(); }

    /**
     * @brief Request shutdown
     */
    void requestShutdown() { m_shutdownRequested.store(true); }

    /**
     * @brief Get application statistics
     * @return ApplicationStats Current statistics
     */
    ApplicationStats getStatistics() const;

    /**
     * @brief Reset statistics
     */
    void resetStatistics();

    /**
     * @brief Observer interface implementation
     * @param eventData Shared pointer to event data
     */
    void onNotify(std::shared_ptr<EventData> eventData) override;

    /**
     * @brief Get observer identifier
     * @return std::string Observer name/identifier
     */
    std::string getObserverId() const override { return "ModernApplication"; }

    /**
     * @brief Get platform type
     * @return PlatformType Current platform
     */
    PlatformType getPlatformType() const { return m_platformType; }

    /**
     * @brief Get event subject for external observers
     * @return std::shared_ptr<ObservableSubject> Event subject
     */
    std::shared_ptr<ObservableSubject> getEventSubject() const { return m_eventSubject; }

  private:
    /**
     * @brief Load configuration from file
     * @return global::RC_t Return code indicating success or failure
     */
    global::RC_t loadConfiguration();

    /**
     * @brief Initialize logging system
     * @return global::RC_t Return code indicating success or failure
     */
    global::RC_t initializeLogging();

    /**
     * @brief Initialize error handling
     * @return global::RC_t Return code indicating success or failure
     */
    global::RC_t initializeErrorHandling();

    /**
     * @brief Create and configure service factory
     * @return global::RC_t Return code indicating success or failure
     */
    global::RC_t createServiceFactory();

    /**
     * @brief Create detection algorithm
     * @return global::RC_t Return code indicating success or failure
     */
    global::RC_t createDetectionAlgorithm();

    /**
     * @brief Start all service threads
     * @return global::RC_t Return code indicating success or failure
     */
    global::RC_t startServiceThreads();

    /**
     * @brief Stop all service threads
     * @return global::RC_t Return code indicating success or failure
     */
    global::RC_t stopServiceThreads();

    /**
     * @brief Handle detection results
     * @param detectionData Detection event data
     */
    void handleDetectionResults(std::shared_ptr<DetectionEventData> detectionData);

    /**
     * @brief Handle camera frame ready
     * @param frameData Camera frame event data
     */
    void handleCameraFrame(std::shared_ptr<CameraFrameEventData> frameData);

    /**
     * @brief Handle communication events
     * @param commData Communication event data
     */
    void handleCommunicationEvent(std::shared_ptr<CommunicationEventData> commData);

    /**
     * @brief Handle error events
     * @param errorData Error event data
     */
    void handleErrorEvent(std::shared_ptr<ErrorEventData> errorData);

    /**
     * @brief Update application statistics
     * @param frameProcessed True if frame was processed
     * @param detectionCount Number of detections
     * @param processingTime Processing time in milliseconds
     */
    void updateStatistics(bool frameProcessed, size_t detectionCount, double processingTime);

    /**
     * @brief Validate system prerequisites
     * @return global::RC_t Return code indicating validation result
     */
    global::RC_t validateSystemPrerequisites();

    /**
     * @brief Setup default configuration if none exists
     * @return global::RC_t Return code indicating success or failure
     */
    global::RC_t setupDefaultConfiguration();
};

#endif /* MODERNAPPLICATION_H */