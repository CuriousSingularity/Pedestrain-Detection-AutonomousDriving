/***************************************************************************
 *============= Copyright by Darmstadt University of Applied Sciences =======
 ****************************************************************************
 * Filename        : IServiceFactory.h
 * Author          : Bharath Ramachandraiah (stbhrama@stud.h-da.de)
 * Description     : Factory pattern interface for creating platform-specific services
 *
 ****************************************************************************/

#ifndef ISERVICEFACTORY_H
#define ISERVICEFACTORY_H

#include "global.h"
#include <memory>
#include <string>

// Forward declarations
class IDetectionAlgorithm;
class CCamera;
class CUart;
class CThread;

/**
 * @brief Platform types for factory selection
 */
enum class PlatformType {
    NVIDIA_JETSON = NVIDIA,
    RASPBERRY_PI = RSP,
    PC_PLATFORM = PC
};

/**
 * @brief Abstract factory interface for creating platform-specific services
 */
class IServiceFactory {
public:
    virtual ~IServiceFactory() = default;

    /**
     * @brief Create detection algorithm instance
     * @return std::unique_ptr<IDetectionAlgorithm> Detection algorithm instance
     */
    virtual std::unique_ptr<IDetectionAlgorithm> createDetectionAlgorithm() = 0;

    /**
     * @brief Create camera service instance
     * @param threadIndex Thread identifier
     * @return std::unique_ptr<CCamera> Camera service instance
     */
    virtual std::unique_ptr<CCamera> createCameraService(int threadIndex) = 0;

    /**
     * @brief Create communication service instance
     * @param threadIndex Thread identifier
     * @return std::unique_ptr<CUart> Communication service instance
     */
    virtual std::unique_ptr<CUart> createCommunicationService(int threadIndex) = 0;

    /**
     * @brief Get platform name
     * @return std::string Platform identifier
     */
    virtual std::string getPlatformName() const = 0;

    /**
     * @brief Get platform type
     * @return PlatformType Platform enumeration
     */
    virtual PlatformType getPlatformType() const = 0;
};

/**
 * @brief Factory manager for creating platform-specific factories
 */
class ServiceFactoryManager {
public:
    /**
     * @brief Create factory instance for specified platform
     * @param platformType Platform type
     * @return std::unique_ptr<IServiceFactory> Factory instance
     */
    static std::unique_ptr<IServiceFactory> createFactory(PlatformType platformType);

    /**
     * @brief Create factory instance for current platform (from global.h)
     * @return std::unique_ptr<IServiceFactory> Factory instance
     */
    static std::unique_ptr<IServiceFactory> createFactoryForCurrentPlatform();

private:
    ServiceFactoryManager() = default;
};

#endif /* ISERVICEFACTORY_H */