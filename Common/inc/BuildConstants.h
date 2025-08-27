/***************************************************************************
 *============= Copyright by Darmstadt University of Applied Sciences =======
 ****************************************************************************
 * Filename        : BuildConstants.h
 * Author          : Bharath Ramachandraiah (stbhrama@stud.h-da.de)
 * Description     : Centralized build-time constants and configuration flags
 *
 ****************************************************************************/

#ifndef BUILDCONSTANTS_H
#define BUILDCONSTANTS_H

#include "global.h"

#include <cstdint>
#include <string_view>

namespace pedestrian_detection::build {

// 🏗️ Build Configuration Constants
struct BuildConfig {
// Build flags (set by Makefile)
#ifdef ALGO_TIME_MEASUREMENT
    static constexpr bool ENABLE_ALGO_TIME_MEASUREMENT = true;
#else
    static constexpr bool ENABLE_ALGO_TIME_MEASUREMENT = false;
#endif

#ifndef DISPLAY_CONNECTED
#define DISPLAY_CONNECTED 0
#endif
    static constexpr bool ENABLE_DISPLAY_CONNECTED = (DISPLAY_CONNECTED == 1);

#ifdef DEBUG_LOGGING
    static constexpr bool ENABLE_DEBUG_LOGGING = true;
#else
    static constexpr bool ENABLE_DEBUG_LOGGING = false;
#endif

#ifdef PERFORMANCE_PROFILING
    static constexpr bool ENABLE_PERFORMANCE_PROFILING = true;
#else
    static constexpr bool ENABLE_PERFORMANCE_PROFILING = false;
#endif

#ifdef VERBOSE_OUTPUT
    static constexpr bool ENABLE_VERBOSE_OUTPUT = true;
#else
    static constexpr bool ENABLE_VERBOSE_OUTPUT = false;
#endif

// Build type detection
#ifdef DEBUG
    static constexpr bool IS_DEBUG_BUILD = true;
    static constexpr std::string_view BUILD_TYPE = "DEBUG";
#else
    static constexpr bool IS_DEBUG_BUILD = false;
    static constexpr std::string_view BUILD_TYPE = "RELEASE";
#endif

    // Enable/Disable macros for backward compatibility
    static constexpr int DISABLE = 0;
    static constexpr int ENABLE = 1;
};

// 🚗 Detection Algorithm Constants
struct DetectionConstants {
    // HOG Configuration Parameters
    struct HOGConfig {
        static constexpr float DEFAULT_HIT_THRESHOLD = 95.0f;   // Range: 0-100
        static constexpr int DEFAULT_WIN_STRIDE = 16;           // Range: 1-32
        static constexpr int DEFAULT_PADDING = 8;               // Range: 0-64
        static constexpr float DEFAULT_SCALE = 1.26f;           // Range: > 1.0
        static constexpr float DEFAULT_FINAL_THRESHOLD = 0.0f;  // Range: 0-100
        static constexpr float DEFAULT_NMS_THRESHOLD = 0.0f;    // Range: 0-100
        static constexpr int DEFAULT_NMS_NEIGHBORS = 0;         // Range: 0-99

        // Validation functions
        static constexpr bool isValidHitThreshold(float threshold) noexcept {
            return threshold >= 0.0f && threshold <= 100.0f;
        }

        static constexpr bool isValidWinStride(int stride) noexcept {
            return stride >= 1 && stride <= 32;
        }

        static constexpr bool isValidPadding(int padding) noexcept {
            return padding >= 0 && padding <= 64;
        }

        static constexpr bool isValidScale(float scale) noexcept { return scale > 1.0f; }
    };

    // Detection filtering constants
    static constexpr int MIN_DETECTION_AREA = 1000;
    static constexpr int MAX_STRAIGHT_LINES = 6;
    static constexpr size_t MAX_DETECTION_RESULTS = 50;

    // Camera constants
    static constexpr float CAMERA_FOV_DEGREES = 62.0f;
    static constexpr float HALF_FOV = CAMERA_FOV_DEGREES / 2.0f;
};

// 📡 Communication Protocol Constants
struct ProtocolConstants {
    // Protocol bytes
    static constexpr uint8_t SOP = 0xFA;  // Start of Protocol
    static constexpr uint8_t EOP = 0xFD;  // End of Protocol
    static constexpr uint8_t DLC = 0x04;  // Data Length Code

    // Protocol structure size
    static constexpr size_t PROTOCOL_SIZE = 7;  // SOP + DLC + Data(4) + EOP
    static constexpr size_t DATA_SIZE = 4;      // Theta + Delta Theta + Reserved(2)

    // Protocol validation
    static constexpr bool isValidProtocolByte(uint8_t byte) noexcept { return byte != 0x00; }

    // Protocol field positions
    enum ProtocolPosition : size_t {
        POS_SOP = 0,
        POS_DLC = 1,
        POS_THETA = 2,
        POS_DELTA_THETA = 3,
        POS_RESERVED1 = 4,
        POS_RESERVED2 = 5,
        POS_EOP = 6
    };
};

// 🧵 Thread Configuration Constants
struct ThreadConstants {
    // Thread priorities (if supported by OS)
    static constexpr int PRIORITY_DETECTION = 10;
    static constexpr int PRIORITY_CAMERA = 8;
    static constexpr int PRIORITY_COM_TX = 6;
    static constexpr int PRIORITY_COM_RX = 6;
    static constexpr int PRIORITY_BACKGROUND = 1;

    // Thread stack sizes (in bytes)
    static constexpr size_t STACK_SIZE_DETECTION = 32768;  // 32KB
    static constexpr size_t STACK_SIZE_CAMERA = 16384;     // 16KB
    static constexpr size_t STACK_SIZE_COM_TX = 8192;      // 8KB
    static constexpr size_t STACK_SIZE_COM_RX = 8192;      // 8KB
    static constexpr size_t STACK_SIZE_BACKGROUND = 4096;  // 4KB

    // Timeout values (in milliseconds)
    static constexpr int TIMEOUT_CAMERA_CAPTURE = 1000;
    static constexpr int TIMEOUT_DETECTION = 5000;
    static constexpr int TIMEOUT_COMMUNICATION = 2000;
    static constexpr int TIMEOUT_MAILBOX = 100;
};

// 💾 Memory Management Constants
struct MemoryConstants {
    // Ring buffer sizes
    static constexpr size_t FRAME_BUFFER_SIZE = 10;     // Number of frames
    static constexpr size_t MAILBOX_BUFFER_SIZE = 100;  // Number of messages

    // Memory pool sizes (in bytes)
    static constexpr size_t DETECTION_POOL_SIZE = 1024 * 1024;  // 1MB
    static constexpr size_t FRAME_POOL_SIZE = 4 * 1024 * 1024;  // 4MB

    // Alignment requirements
    static constexpr size_t MEMORY_ALIGNMENT = 64;  // 64-byte alignment for SIMD

    // Maximum object sizes
    static constexpr size_t MAX_FRAME_SIZE = 1920 * 1080 * 3;  // Full HD RGB
    static constexpr size_t MAX_DETECTION_COUNT = 100;
};

// 🔧 Platform-Specific Constants
struct PlatformConstants {
// Platform detection
#ifdef TARGET_PLATFORM
    static constexpr int CURRENT_PLATFORM = TARGET_PLATFORM;
#else
    static constexpr int CURRENT_PLATFORM = PC;  // Default to PC
#endif

    // Platform-specific configurations
    static constexpr bool isNVIDIA() noexcept { return CURRENT_PLATFORM == NVIDIA; }

    static constexpr bool isRaspberryPi() noexcept { return CURRENT_PLATFORM == RSP; }

    static constexpr bool isPC() noexcept { return CURRENT_PLATFORM == PC; }

    // Platform capabilities
    static constexpr bool hasGPU() noexcept { return isNVIDIA() || isPC(); }

    static constexpr bool hasHighPerformance() noexcept { return isNVIDIA() || isPC(); }

    // Platform-specific default values
    static constexpr float getDefaultScaleFactor() noexcept {
        if (isNVIDIA())
            return 1.1f;
        if (isRaspberryPi())
            return 1.05f;
        return 1.2f;  // PC
    }

    static constexpr size_t getOptimalThreadCount() noexcept {
        if (isNVIDIA())
            return 4;
        if (isRaspberryPi())
            return 2;
        return 8;  // PC
    }
};

// 📊 Performance Monitoring Constants
struct PerformanceConstants {
    // Timing measurement intervals
    static constexpr int TIMING_SAMPLE_COUNT = 100;
    static constexpr int TIMING_HISTORY_SIZE = 1000;

    // Performance thresholds (in milliseconds)
    static constexpr float MAX_DETECTION_TIME_MS = 100.0f;
    static constexpr float MAX_FRAME_CAPTURE_TIME_MS = 50.0f;
    static constexpr float MAX_COMMUNICATION_TIME_MS = 10.0f;

    // FPS targets
    static constexpr float TARGET_FPS = 30.0f;
    static constexpr float MIN_ACCEPTABLE_FPS = 15.0f;

    // Memory usage thresholds (in bytes)
    static constexpr size_t MAX_MEMORY_USAGE = 512 * 1024 * 1024;      // 512MB
    static constexpr size_t WARNING_MEMORY_USAGE = 256 * 1024 * 1024;  // 256MB
};

// 🔍 Debug and Logging Constants
struct DebugConstants {
    // Log levels
    enum LogLevel : int {
        LOG_ERROR = 0,
        LOG_WARNING = 1,
        LOG_INFO = 2,
        LOG_DEBUG = 3,
        LOG_VERBOSE = 4
    };

    // Default log level based on build configuration
    static constexpr LogLevel DEFAULT_LOG_LEVEL =
        BuildConfig::IS_DEBUG_BUILD ? LOG_DEBUG : LOG_INFO;

    // Log file configurations
    static constexpr size_t MAX_LOG_FILE_SIZE = 10 * 1024 * 1024;  // 10MB
    static constexpr int MAX_LOG_FILES = 5;

    // Debug output configurations
    static constexpr bool ENABLE_CONSOLE_OUTPUT = true;
    static constexpr bool ENABLE_FILE_OUTPUT = BuildConfig::IS_DEBUG_BUILD;
    static constexpr bool ENABLE_TIMESTAMP = true;
    static constexpr bool ENABLE_THREAD_ID = BuildConfig::IS_DEBUG_BUILD;
};

// 🎯 Configuration Validation
class ConstantsValidator {
  public:
    // Compile-time validation of all constants
    static consteval bool validateAllConstants() noexcept {
        return validateDetectionConstants() && validateProtocolConstants() &&
               validateThreadConstants() && validateMemoryConstants() &&
               validatePerformanceConstants();
    }

  private:
    static consteval bool validateDetectionConstants() noexcept {
        return DetectionConstants::CAMERA_FOV_DEGREES > 0.0f &&
               DetectionConstants::CAMERA_FOV_DEGREES <= 180.0f &&
               DetectionConstants::MIN_DETECTION_AREA > 0 &&
               DetectionConstants::MAX_DETECTION_RESULTS > 0;
    }

    static consteval bool validateProtocolConstants() noexcept {
        return ProtocolConstants::PROTOCOL_SIZE >= 7 &&
               ProtocolConstants::SOP != ProtocolConstants::EOP &&
               ProtocolConstants::DATA_SIZE == 4;
    }

    static consteval bool validateThreadConstants() noexcept {
        return ThreadConstants::STACK_SIZE_DETECTION >= 4096 &&
               ThreadConstants::TIMEOUT_DETECTION > 0;
    }

    static consteval bool validateMemoryConstants() noexcept {
        return MemoryConstants::FRAME_BUFFER_SIZE > 0 && MemoryConstants::MAX_FRAME_SIZE > 0 &&
               MemoryConstants::MEMORY_ALIGNMENT > 0;
    }

    static consteval bool validatePerformanceConstants() noexcept {
        return PerformanceConstants::TARGET_FPS > 0.0f &&
               PerformanceConstants::MAX_DETECTION_TIME_MS > 0.0f;
    }
};

// Compile-time validation
static_assert(ConstantsValidator::validateAllConstants(),
              "One or more constants have invalid values");

}  // namespace pedestrian_detection::build

// 🔗 Convenience macros for backward compatibility
#define ENABLE_FLAG pedestrian_detection::build::BuildConfig::ENABLE
#define DISABLE_FLAG pedestrian_detection::build::BuildConfig::DISABLE

// Platform detection macros
#define IS_NVIDIA_PLATFORM() pedestrian_detection::build::PlatformConstants::isNVIDIA()
#define IS_RPI_PLATFORM() pedestrian_detection::build::PlatformConstants::isRaspberryPi()
#define IS_PC_PLATFORM() pedestrian_detection::build::PlatformConstants::isPC()

// Build configuration macros
#define IS_DEBUG_BUILD() pedestrian_detection::build::BuildConfig::IS_DEBUG_BUILD
#define IS_TIMING_ENABLED() pedestrian_detection::build::BuildConfig::ENABLE_ALGO_TIME_MEASUREMENT
#define IS_DISPLAY_ENABLED() pedestrian_detection::build::BuildConfig::ENABLE_DISPLAY_CONNECTED

#endif /* BUILDCONSTANTS_H */