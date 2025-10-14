/***************************************************************************
 *============= Copyright by Darmstadt University of Applied Sciences =======
 ****************************************************************************
 * Filename        : Logger.cpp
 * Author          : Bharath Ramachandraiah (stbhrama@stud.h-da.de)
 * Description     : Singleton logger implementation with multiple output destinations
 *
 ****************************************************************************/

#include "../inc/Logger.h"
#include <chrono>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <thread>
#include <filesystem>

#ifdef __linux__
#include <unistd.h>
#include <sys/syscall.h>
#endif

// ConsoleLogDestination Implementation
ConsoleLogDestination::ConsoleLogDestination(bool useColors) 
    : m_enabled(true), m_useColors(useColors) {
}

global::RC_t ConsoleLogDestination::write(const LogEntry& entry) {
    if (!m_enabled) {
        return global::RC_SUCCESS;
    }

    std::ostringstream oss;
    
    if (m_useColors) {
        oss << getColorCode(entry.level);
    }
    
    oss << "[" << entry.timestamp << "] "
        << "[" << Logger::getInstance().levelToString(entry.level) << "] "
        << "[" << entry.component << "] "
        << "[TID:" << entry.threadId << "] "
        << entry.message;
    
    if (!entry.function.empty()) {
        oss << " (" << entry.function << ":" << entry.line << ")";
    }
    
    if (m_useColors) {
        oss << getResetCode();
    }
    
    oss << std::endl;
    
    std::cout << oss.str();
    return global::RC_SUCCESS;
}

global::RC_t ConsoleLogDestination::flush() {
    std::cout.flush();
    return global::RC_SUCCESS;
}

std::string ConsoleLogDestination::getColorCode(LogLevel level) const {
    switch (level) {
        case LogLevel::TRACE:    return "\033[37m";  // White
        case LogLevel::DEBUG:    return "\033[36m";  // Cyan
        case LogLevel::INFO:     return "\033[32m";  // Green
        case LogLevel::WARNING:  return "\033[33m";  // Yellow
        case LogLevel::ERROR:    return "\033[31m";  // Red
        case LogLevel::CRITICAL: return "\033[35m";  // Magenta
        default:                 return "";
    }
}

std::string ConsoleLogDestination::getResetCode() const {
    return "\033[0m";
}

// FileLogDestination Implementation
FileLogDestination::FileLogDestination(const std::string& filename, 
                                       size_t maxFileSize, 
                                       int maxBackupFiles)
    : m_filename(filename), m_enabled(true), 
      m_maxFileSize(maxFileSize), m_maxBackupFiles(maxBackupFiles) {
    m_file.open(m_filename, std::ios::app);
}

FileLogDestination::~FileLogDestination() {
    if (m_file.is_open()) {
        m_file.close();
    }
}

global::RC_t FileLogDestination::write(const LogEntry& entry) {
    if (!m_enabled || !m_file.is_open()) {
        return global::RC_ERROR;
    }

    // Check if file rotation is needed
    if (getFileSize() > m_maxFileSize) {
        rotateFile();
    }

    m_file << "[" << entry.timestamp << "] "
           << "[" << Logger::getInstance().levelToString(entry.level) << "] "
           << "[" << entry.component << "] "
           << "[TID:" << entry.threadId << "] "
           << entry.message;
    
    if (!entry.function.empty()) {
        m_file << " (" << entry.function << ":" << entry.line << ")";
    }
    
    m_file << std::endl;
    return global::RC_SUCCESS;
}

global::RC_t FileLogDestination::flush() {
    if (m_file.is_open()) {
        m_file.flush();
    }
    return global::RC_SUCCESS;
}

global::RC_t FileLogDestination::rotateFile() {
    if (m_file.is_open()) {
        m_file.close();
    }

    // Rotate backup files
    for (int i = m_maxBackupFiles - 1; i > 0; --i) {
        std::string oldName = m_filename + "." + std::to_string(i);
        std::string newName = m_filename + "." + std::to_string(i + 1);
        std::rename(oldName.c_str(), newName.c_str());
    }

    // Move current file to .1
    std::string backupName = m_filename + ".1";
    std::rename(m_filename.c_str(), backupName.c_str());

    // Open new file
    m_file.open(m_filename, std::ios::out | std::ios::trunc);
    return m_file.is_open() ? global::RC_SUCCESS : global::RC_ERROR;
}

size_t FileLogDestination::getFileSize() const {
    if (!m_file.is_open()) {
        return 0;
    }
    
    try {
        // Use filesystem to get file size
        return std::filesystem::file_size(m_filename);
    } catch (const std::filesystem::filesystem_error&) {
        return 0;
    }
}

// Logger Implementation
Logger::Logger() : m_minLevel(LogLevel::INFO), m_enabled(true) {
}

Logger& Logger::getInstance() {
    static Logger instance;
    return instance;
}

global::RC_t Logger::addDestination(std::unique_ptr<ILogDestination> destination) {
    if (!destination) {
        return global::RC_ERROR_BAD_PARAM;
    }
    
    std::lock_guard<std::mutex> lock(m_logMutex);
    m_destinations.push_back(std::move(destination));
    return global::RC_SUCCESS;
}

void Logger::log(LogLevel level, const std::string& component, const std::string& message,
                 const std::string& function, int line) {
    if (!m_enabled || level < m_minLevel) {
        return;
    }

    LogEntry entry;
    entry.level = level;
    entry.timestamp = getCurrentTimestamp();
    entry.component = component;
    entry.message = message;
    entry.threadId = getCurrentThreadId();
    entry.function = function;
    entry.line = line;

    std::lock_guard<std::mutex> lock(m_logMutex);
    for (auto& destination : m_destinations) {
        if (destination && destination->isEnabled()) {
            destination->write(entry);
        }
    }
}

global::RC_t Logger::flush() {
    std::lock_guard<std::mutex> lock(m_logMutex);
    global::RC_t result = global::RC_SUCCESS;
    
    for (auto& destination : m_destinations) {
        if (destination && destination->isEnabled()) {
            if (destination->flush() != global::RC_SUCCESS) {
                result = global::RC_ERROR;
            }
        }
    }
    
    return result;
}

void Logger::clearDestinations() {
    std::lock_guard<std::mutex> lock(m_logMutex);
    m_destinations.clear();
}

std::string Logger::getCurrentTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;

    std::ostringstream oss;
    oss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    oss << '.' << std::setfill('0') << std::setw(3) << ms.count();
    return oss.str();
}

std::string Logger::levelToString(LogLevel level) const {
    switch (level) {
        case LogLevel::TRACE:    return "TRACE";
        case LogLevel::DEBUG:    return "DEBUG";
        case LogLevel::INFO:     return "INFO";
        case LogLevel::WARNING:  return "WARN";
        case LogLevel::ERROR:    return "ERROR";
        case LogLevel::CRITICAL: return "CRIT";
        default:                 return "UNKNOWN";
    }
}

global::thread_id_t Logger::getCurrentThreadId() const {
#ifdef __linux__
    return static_cast<global::thread_id_t>(syscall(SYS_gettid));
#else
    return static_cast<global::thread_id_t>(std::hash<std::thread::id>{}(std::this_thread::get_id()));
#endif
}