/***************************************************************************
 *============= Copyright by Darmstadt University of Applied Sciences =======
 ****************************************************************************
 * Filename        : ModernConstants.h
 * Author          : Bharath Ramachandraiah (stbhrama@stud.h-da.de)
 * Description     : C++20 constexpr, consteval, and constinit features
 *
 ****************************************************************************/

#ifndef MODERNCONSTANTS_H
#define MODERNCONSTANTS_H

#include <string_view>
#include <array>
#include <cmath>
#include <numbers>
#include <opencv2/opencv.hpp>
#include "global.h"

namespace pedestrian_detection::constants {

// C++20 constinit for compile-time initialization
constinit inline const float CAMERA_FOV_DEGREES = 62.0f;
constinit inline const float HALF_FOV = CAMERA_FOV_DEGREES / 2.0f;
constinit inline const int MIN_DETECTION_AREA = 1000;
constinit inline const int MAX_STRAIGHT_LINES = 6;
constinit inline const size_t MAX_DETECTION_RESULTS = 50;

// Constexpr constants for compile-time computation
constexpr float PI = std::numbers::pi_v<float>;
constexpr float DEG_TO_RAD = PI / 180.0f;
constexpr float RAD_TO_DEG = 180.0f / PI;

// Consteval functions for compile-time-only evaluation
consteval float degreesToRadians(float degrees) noexcept {
    return degrees * DEG_TO_RAD;
}

consteval float radiansToDegrees(float radians) noexcept {
    return radians * RAD_TO_DEG;
}

consteval cv::Size calculateOptimalSize(int width, int height, float scaleFactor) noexcept {
    return cv::Size(static_cast<int>(width * scaleFactor), 
                   static_cast<int>(height * scaleFactor));
}

consteval size_t calculateBufferSize(size_t elementSize, size_t count) noexcept {
    return elementSize * count;
}

// Constexpr data structures
struct PlatformConfig {
    std::string_view name;
    int platformId;
    bool supportsGPU;
    float defaultScaleFactor;
    
    constexpr PlatformConfig(std::string_view n, int id, bool gpu, float scale)
        : name(n), platformId(id), supportsGPU(gpu), defaultScaleFactor(scale) {}
};

// C++20 constexpr std::array initialization
constexpr std::array<PlatformConfig, 3> PLATFORM_CONFIGS = {{
    PlatformConfig{"NVIDIA Jetson", NVIDIA, true, 1.1f},
    PlatformConfig{"Raspberry Pi", RSP, false, 1.05f},
    PlatformConfig{"PC Platform", PC, true, 1.2f}
}};

// Constexpr algorithm configuration
struct AlgorithmConstants {
    static constexpr float DEFAULT_HIT_THRESHOLD = 0.5f;
    static constexpr float DEFAULT_SCALE_FACTOR = 1.1f;
    static constexpr int DEFAULT_MIN_NEIGHBORS = 3;
    static constexpr cv::Size DEFAULT_MIN_SIZE{30, 30};
    static constexpr cv::Size DEFAULT_MAX_SIZE{};
    
    // Constexpr validation functions
    static constexpr bool isValidThreshold(float threshold) noexcept {
        return threshold >= 0.0f && threshold <= 1.0f;
    }
    
    static constexpr bool isValidScaleFactor(float scale) noexcept {
        return scale > 1.0f && scale <= 2.0f;
    }
    
    static constexpr bool isValidMinNeighbors(int neighbors) noexcept {
        return neighbors >= 0 && neighbors <= 10;
    }
};

// Constexpr protocol constants
struct ProtocolConstants {
    static constexpr uint8_t SOP = 0xFA;
    static constexpr uint8_t EOP = 0xFD;
    static constexpr uint8_t DLC = 0x04;
    static constexpr size_t PROTOCOL_SIZE = 7;  // SOP + DLC + Data(4) + EOP
    
    // Consteval protocol validation
    static consteval bool isValidProtocolByte(uint8_t byte) noexcept {
        return byte != 0x00;  // Simple validation example
    }
    
    // Constexpr protocol structure
    struct ProtocolFrame {
        uint8_t sop;
        uint8_t dlc;
        uint8_t theta;
        uint8_t deltaTheta;
        uint8_t reserved1;
        uint8_t reserved2;
        uint8_t eop;
        
        constexpr ProtocolFrame() 
            : sop(SOP), dlc(DLC), theta(0), deltaTheta(0), reserved1(0), reserved2(0), eop(EOP) {}
        
        constexpr ProtocolFrame(uint8_t t, uint8_t dt) 
            : sop(SOP), dlc(DLC), theta(t), deltaTheta(dt), reserved1(0), reserved2(0), eop(EOP) {}
        
        constexpr bool isValid() const noexcept {
            return sop == SOP && eop == EOP && dlc == DLC;
        }
    };
};

// Constexpr mathematical utilities
class MathUtils {
public:
    // Constexpr angle calculations
    static constexpr float calculateAngle(float centerX, float frameCenterX, float frameWidth) noexcept {
        const float pixelOffset = centerX - frameCenterX;
        const float pixelsPerDegree = frameWidth / CAMERA_FOV_DEGREES;
        const float angle = pixelOffset / pixelsPerDegree;
        
        // Constexpr clamp
        return (angle < -HALF_FOV) ? -HALF_FOV : 
               (angle > HALF_FOV) ? HALF_FOV : angle;
    }
    
    static constexpr float calculateDeltaAngle(float width, float frameWidth) noexcept {
        const float pixelsPerDegree = frameWidth / CAMERA_FOV_DEGREES;
        return width / pixelsPerDegree;
    }
    
    // Constexpr area calculation
    static constexpr int calculateArea(int width, int height) noexcept {
        return width * height;
    }
    
    // Constexpr distance calculation
    static constexpr float calculateDistance(float x1, float y1, float x2, float y2) noexcept {
        const float dx = x2 - x1;
        const float dy = y2 - y1;
        return std::sqrt(dx * dx + dy * dy);
    }
    
    // Consteval compile-time square root (for validation)
    static consteval float constexprSqrt(float x) noexcept {
        if (x < 0) return -1; // Error case
        float guess = x / 2.0f;
        float prev = 0;
        while (guess != prev) {
            prev = guess;
            guess = (guess + x / guess) / 2.0f;
        }
        return guess;
    }
};

// Constexpr error code mappings
struct ErrorCodeMapping {
    global::RC_t code;
    std::string_view description;
    bool isRecoverable;
    
    constexpr ErrorCodeMapping(global::RC_t c, std::string_view desc, bool recoverable)
        : code(c), description(desc), isRecoverable(recoverable) {}
};

constexpr std::array<ErrorCodeMapping, 10> ERROR_MAPPINGS = {{
    ErrorCodeMapping{global::RC_SUCCESS, "Success", true},
    ErrorCodeMapping{global::RC_ERROR, "General error", true},
    ErrorCodeMapping{global::RC_ERROR_NULL, "Null pointer", false},
    ErrorCodeMapping{global::RC_ERROR_MEMORY, "Memory error", false},
    ErrorCodeMapping{global::RC_ERROR_RANGE, "Range error", true},
    ErrorCodeMapping{global::RC_ERROR_TIMEOUT, "Timeout", true},
    ErrorCodeMapping{global::RC_ERROR_INVALID, "Invalid operation", true},
    ErrorCodeMapping{global::RC_ERROR_BUSY, "Resource busy", true},
    ErrorCodeMapping{global::RC_ERROR_BUFFER_FULL, "Buffer full", true},
    ErrorCodeMapping{global::RC_ERROR_BUFFER_EMTPY, "Buffer empty", true}
}};

// Constexpr lookup function
constexpr std::string_view getErrorDescription(global::RC_t code) noexcept {
    for (const auto& mapping : ERROR_MAPPINGS) {
        if (mapping.code == code) {
            return mapping.description;
        }
    }
    return "Unknown error";
}

constexpr bool isRecoverableError(global::RC_t code) noexcept {
    for (const auto& mapping : ERROR_MAPPINGS) {
        if (mapping.code == code) {
            return mapping.isRecoverable;
        }
    }
    return false;
}

// Constexpr configuration validation
class ConfigValidator {
public:
    static constexpr bool validateDetectionConfig(float threshold, float scale, int neighbors) noexcept {
        return AlgorithmConstants::isValidThreshold(threshold) &&
               AlgorithmConstants::isValidScaleFactor(scale) &&
               AlgorithmConstants::isValidMinNeighbors(neighbors);
    }
    
    static constexpr bool validateFrameSize(int width, int height) noexcept {
        return width > 0 && height > 0 && width <= 4096 && height <= 4096;
    }
    
    static constexpr bool validateAngle(float angle) noexcept {
        return angle >= -HALF_FOV && angle <= HALF_FOV;
    }
    
    static consteval size_t calculateMaxDetections(size_t frameWidth, size_t frameHeight) noexcept {
        // Estimate based on minimum detection size
        const size_t minDetectionSize = 30 * 30;  // 30x30 pixels
        const size_t frameArea = frameWidth * frameHeight;
        return frameArea / minDetectionSize;  // Theoretical maximum
    }
};

// Constexpr version information
struct VersionInfo {
    static constexpr int MAJOR = 2;
    static constexpr int MINOR = 0;
    static constexpr int PATCH = 0;
    static constexpr std::string_view BUILD_TYPE = "C++20";
    
    static constexpr int getVersionNumber() noexcept {
        return MAJOR * 10000 + MINOR * 100 + PATCH;
    }
    
    static consteval bool isCompatibleVersion(int major, int minor) noexcept {
        return major == MAJOR && minor <= MINOR;
    }
};

} // namespace pedestrian_detection::constants

#endif /* MODERNCONSTANTS_H */