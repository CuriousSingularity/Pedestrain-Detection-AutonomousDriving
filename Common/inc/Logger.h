/***************************************************************************
 *============= Copyright by Darmstadt University of Applied Sciences =======
 ****************************************************************************
 * Filename        : Logger.h
 * Author          : Bharath Ramachandraiah (stbhrama@stud.h-da.de)
 * Description     : Singleton logger with multiple output destinations
 *
 ****************************************************************************/

#ifndef LOGGER_H
#define LOGGER_H

#include "global.h"

#include <fstream>
#include <memory>
#include <mutex>
#include <sstream>
#include <string>
#include <vector>

/**
 * @brief Log levels
 */
enum class LogLevel { TRACE = 0, DEBUG = 1, INFO = 2, WARNING = 3, ERROR = 4, CRITICAL = 5 };

/**
 * @brief Log output destinations
 */
enum class LogOutput { CONSOLE, FILE, SYSLOG };

/**
 * @brief Log entry structure
 */
struct LogEntry {
    LogLevel level;
    std::string timestamp;
    std::string component;
    std::string message;
    global::thread_id_t threadId;
    std::string function;
    int line;
};

/**
 * @brief Abstract log destination interface
 */
class ILogDestination {
  public:
    virtual ~ILogDestination() = default;
    virtual global::RC_t write(const LogEntry& entry) = 0;
    virtual global::RC_t flush() = 0;
    virtual bool isEnabled() const = 0;
    virtual void setEnabled(bool enabled) = 0;
};

/**
 * @brief Console log destination
 */
class ConsoleLogDestination : public ILogDestination {
  private:
    bool m_enabled;
    bool m_useColors;

  public:
    explicit ConsoleLogDestination(bool useColors = true);
    global::RC_t write(const LogEntry& entry) override;
    global::RC_t flush() override;
    bool isEnabled() const override { return m_enabled; }
    void setEnabled(bool enabled) override { m_enabled = enabled; }

  private:
    std::string getColorCode(LogLevel level) const;
    std::string getResetCode() const;
};

/**
 * @brief File log destination
 */
class FileLogDestination : public ILogDestination {
  private:
    std::ofstream m_file;
    std::string m_filename;
    bool m_enabled;
    size_t m_maxFileSize;
    int m_maxBackupFiles;

  public:
    explicit FileLogDestination(const std::string& filename,
                                size_t maxFileSize = 10 * 1024 * 1024,  // 10MB
                                int maxBackupFiles = 5);
    ~FileLogDestination();

    global::RC_t write(const LogEntry& entry) override;
    global::RC_t flush() override;
    bool isEnabled() const override { return m_enabled; }
    void setEnabled(bool enabled) override { m_enabled = enabled; }

  private:
    global::RC_t rotateFile();
    size_t getFileSize() const;
};

/**
 * @brief Singleton logger class
 */
class Logger {
  private:
    std::vector<std::unique_ptr<ILogDestination>> m_destinations;
    LogLevel m_minLevel;
    mutable std::mutex m_logMutex;
    bool m_enabled;

    Logger();

  public:
    /**
     * @brief Get singleton instance
     * @return Logger& Reference to singleton instance
     */
    static Logger& getInstance();

    /**
     * @brief Add log destination
     * @param destination Unique pointer to log destination
     * @return global::RC_t Return code indicating success or failure
     */
    global::RC_t addDestination(std::unique_ptr<ILogDestination> destination);

    /**
     * @brief Set minimum log level
     * @param level Minimum level to log
     */
    void setMinLevel(LogLevel level) { m_minLevel = level; }

    /**
     * @brief Get minimum log level
     * @return LogLevel Current minimum level
     */
    LogLevel getMinLevel() const { return m_minLevel; }

    /**
     * @brief Enable/disable logging
     * @param enabled True to enable logging
     */
    void setEnabled(bool enabled) { m_enabled = enabled; }

    /**
     * @brief Check if logging is enabled
     * @return bool True if logging is enabled
     */
    bool isEnabled() const { return m_enabled; }

    /**
     * @brief Log a message
     * @param level Log level
     * @param component Component name
     * @param message Log message
     * @param function Function name (optional)
     * @param line Line number (optional)
     */
    void log(LogLevel level, const std::string& component, const std::string& message,
             const std::string& function = "", int line = 0);

    /**
     * @brief Flush all log destinations
     * @return global::RC_t Return code indicating success or failure
     */
    global::RC_t flush();

    /**
     * @brief Clear all destinations
     */
    void clearDestinations();

    // Deleted copy constructor and assignment operator for singleton
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

  private:
    std::string getCurrentTimestamp() const;
    std::string levelToString(LogLevel level) const;
    global::thread_id_t getCurrentThreadId() const;
};

// Convenience macros for logging
#define LOG_TRACE(component, message)                                                              \
    Logger::getInstance().log(LogLevel::TRACE, component, message, __FUNCTION__, __LINE__)
#define LOG_DEBUG(component, message)                                                              \
    Logger::getInstance().log(LogLevel::DEBUG, component, message, __FUNCTION__, __LINE__)
#define LOG_INFO(component, message)                                                               \
    Logger::getInstance().log(LogLevel::INFO, component, message, __FUNCTION__, __LINE__)
#define LOG_WARNING(component, message)                                                            \
    Logger::getInstance().log(LogLevel::WARNING, component, message, __FUNCTION__, __LINE__)
#define LOG_ERROR(component, message)                                                              \
    Logger::getInstance().log(LogLevel::ERROR, component, message, __FUNCTION__, __LINE__)
#define LOG_CRITICAL(component, message)                                                           \
    Logger::getInstance().log(LogLevel::CRITICAL, component, message, __FUNCTION__, __LINE__)

// Stream-based logging macros
#define LOG_STREAM(level, component)                                                               \
    do {                                                                                           \
        std::ostringstream oss;                                                                    \
    oss

#define LOG_STREAM_END                                                                             \
    ;                                                                                              \
    Logger::getInstance().log(level, component, oss.str(), __FUNCTION__, __LINE__);                \
    }                                                                                              \
    while (0)

#define LOG_TRACE_STREAM(component) LOG_STREAM(LogLevel::TRACE, component)
#define LOG_DEBUG_STREAM(component) LOG_STREAM(LogLevel::DEBUG, component)
#define LOG_INFO_STREAM(component) LOG_STREAM(LogLevel::INFO, component)
#define LOG_WARNING_STREAM(component) LOG_STREAM(LogLevel::WARNING, component)
#define LOG_ERROR_STREAM(component) LOG_STREAM(LogLevel::ERROR, component)
#define LOG_CRITICAL_STREAM(component) LOG_STREAM(LogLevel::CRITICAL, component)

#endif /* LOGGER_H */