/***************************************************************************
 *============= Copyright by Darmstadt University of Applied Sciences =======
 ****************************************************************************
 * Filename        : ErrorHandler.h
 * Author          : Bharath Ramachandraiah (stbhrama@stud.h-da.de)
 * Description     : Centralized error handling and recovery system
 *
 ****************************************************************************/

#ifndef ERRORHANDLER_H
#define ERRORHANDLER_H

#include "IObserver.h"
#include "Logger.h"
#include "global.h"

#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <string>

/**
 * @brief Error categories for classification
 */
enum class ErrorCategory {
    SYSTEM,
    HARDWARE,
    COMMUNICATION,
    DETECTION,
    CONFIGURATION,
    MEMORY,
    THREAD,
    UNKNOWN
};

/**
 * @brief Error severity levels
 */
enum class ErrorSeverity { INFO, WARNING, ERROR, CRITICAL, FATAL };

/**
 * @brief Error recovery actions
 */
enum class RecoveryAction {
    NONE,
    RETRY,
    RESTART_COMPONENT,
    RESTART_THREAD,
    SHUTDOWN_GRACEFUL,
    SHUTDOWN_IMMEDIATE
};

/**
 * @brief Error information structure
 */
struct ErrorInfo {
    global::RC_t errorCode;
    ErrorCategory category;
    ErrorSeverity severity;
    std::string component;
    std::string description;
    std::string function;
    int line;
    uint64_t timestamp;
    global::thread_id_t threadId;
    RecoveryAction suggestedAction;
    std::map<std::string, std::string> additionalData;
};

/**
 * @brief Error recovery strategy interface
 */
class IErrorRecoveryStrategy {
  public:
    virtual ~IErrorRecoveryStrategy() = default;

    /**
     * @brief Attempt to recover from an error
     * @param errorInfo Error information
     * @return global::RC_t Recovery result
     */
    virtual global::RC_t recover(const ErrorInfo& errorInfo) = 0;

    /**
     * @brief Check if strategy can handle this error
     * @param errorInfo Error information
     * @return bool True if strategy can handle error
     */
    virtual bool canHandle(const ErrorInfo& errorInfo) const = 0;

    /**
     * @brief Get strategy name
     * @return std::string Strategy identifier
     */
    virtual std::string getName() const = 0;
};

/**
 * @brief Centralized error handler using Observer pattern
 */
class ErrorHandler : public IObserver {
  private:
    std::map<ErrorCategory, std::vector<std::shared_ptr<IErrorRecoveryStrategy>>>
        m_recoveryStrategies;
    std::map<global::RC_t, ErrorInfo> m_errorDefinitions;
    mutable std::mutex m_errorMutex;
    std::vector<ErrorInfo> m_errorHistory;
    size_t m_maxHistorySize;
    bool m_enabled;

    ErrorHandler();

  public:
    /**
     * @brief Get singleton instance
     * @return ErrorHandler& Reference to singleton instance
     */
    static ErrorHandler& getInstance();

    /**
     * @brief Handle an error
     * @param errorCode Error code
     * @param component Component name
     * @param description Error description
     * @param function Function name (optional)
     * @param line Line number (optional)
     * @param additionalData Additional context data (optional)
     * @return global::RC_t Recovery result
     */
    global::RC_t handleError(global::RC_t errorCode, const std::string& component,
                             const std::string& description, const std::string& function = "",
                             int line = 0,
                             const std::map<std::string, std::string>& additionalData = {});

    /**
     * @brief Register error definition
     * @param errorCode Error code
     * @param category Error category
     * @param severity Error severity
     * @param defaultDescription Default description
     * @param suggestedAction Suggested recovery action
     * @return global::RC_t Return code indicating success or failure
     */
    global::RC_t registerError(global::RC_t errorCode, ErrorCategory category,
                               ErrorSeverity severity, const std::string& defaultDescription,
                               RecoveryAction suggestedAction = RecoveryAction::NONE);

    /**
     * @brief Add recovery strategy
     * @param category Error category
     * @param strategy Recovery strategy
     * @return global::RC_t Return code indicating success or failure
     */
    global::RC_t addRecoveryStrategy(ErrorCategory category,
                                     std::shared_ptr<IErrorRecoveryStrategy> strategy);

    /**
     * @brief Get error history
     * @param maxCount Maximum number of entries (0 = all)
     * @return std::vector<ErrorInfo> Error history
     */
    std::vector<ErrorInfo> getErrorHistory(size_t maxCount = 0) const;

    /**
     * @brief Clear error history
     */
    void clearHistory();

    /**
     * @brief Set maximum history size
     * @param maxSize Maximum history entries
     */
    void setMaxHistorySize(size_t maxSize) { m_maxHistorySize = maxSize; }

    /**
     * @brief Enable/disable error handling
     * @param enabled True to enable error handling
     */
    void setEnabled(bool enabled) { m_enabled = enabled; }

    /**
     * @brief Check if error handling is enabled
     * @return bool True if enabled
     */
    bool isEnabled() const { return m_enabled; }

    /**
     * @brief Observer interface implementation
     * @param eventData Shared pointer to event data
     */
    void onNotify(std::shared_ptr<EventData> eventData) override;

    /**
     * @brief Get observer identifier
     * @return std::string Observer name/identifier
     */
    std::string getObserverId() const override { return "ErrorHandler"; }

    // Deleted copy constructor and assignment operator for singleton
    ErrorHandler(const ErrorHandler&) = delete;
    ErrorHandler& operator=(const ErrorHandler&) = delete;

  private:
    ErrorInfo createErrorInfo(global::RC_t errorCode, const std::string& component,
                              const std::string& description, const std::string& function, int line,
                              const std::map<std::string, std::string>& additionalData);

    global::RC_t attemptRecovery(const ErrorInfo& errorInfo);
    void addToHistory(const ErrorInfo& errorInfo);
    uint64_t getCurrentTimestamp() const;
};

// Convenience macros for error handling
#define HANDLE_ERROR(code, component, description)                                                 \
    ErrorHandler::getInstance().handleError(code, component, description, __FUNCTION__, __LINE__)

#define HANDLE_ERROR_WITH_DATA(code, component, description, data)                                 \
    ErrorHandler::getInstance().handleError(code, component, description, __FUNCTION__, __LINE__,  \
                                            data)

#define REGISTER_ERROR(code, category, severity, description)                                      \
    ErrorHandler::getInstance().registerError(code, category, severity, description)

#define REGISTER_ERROR_WITH_ACTION(code, category, severity, description, action)                  \
    ErrorHandler::getInstance().registerError(code, category, severity, description, action)

/**
 * @brief Common recovery strategies
 */

/**
 * @brief Retry operation recovery strategy
 */
class RetryRecoveryStrategy : public IErrorRecoveryStrategy {
  private:
    int m_maxRetries;
    std::function<global::RC_t()> m_retryFunction;

  public:
    RetryRecoveryStrategy(int maxRetries, std::function<global::RC_t()> retryFunc);
    global::RC_t recover(const ErrorInfo& errorInfo) override;
    bool canHandle(const ErrorInfo& errorInfo) const override;
    std::string getName() const override { return "RetryRecovery"; }
};

/**
 * @brief Component restart recovery strategy
 */
class ComponentRestartStrategy : public IErrorRecoveryStrategy {
  public:
    global::RC_t recover(const ErrorInfo& errorInfo) override;
    bool canHandle(const ErrorInfo& errorInfo) const override;
    std::string getName() const override { return "ComponentRestart"; }
};

#endif /* ERRORHANDLER_H */