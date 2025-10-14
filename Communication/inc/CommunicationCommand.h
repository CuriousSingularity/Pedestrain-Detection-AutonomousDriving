/***************************************************************************
 *============= Copyright by Darmstadt University of Applied Sciences =======
 ****************************************************************************
 * Filename        : CommunicationCommand.h
 * Author          : Bharath Ramachandraiah (stbhrama@stud.h-da.de)
 * Description     : Command pattern implementation for communication operations
 *
 ****************************************************************************/

#ifndef COMMUNICATIONCOMMAND_H
#define COMMUNICATIONCOMMAND_H

#include "../../App/inc/CSerialProtocol.h"
#include "../../Common/inc/ICommand.h"
#include "../../Common/inc/IDetectionAlgorithm.h"

#include <atomic>
#include <memory>

// Forward declarations
class CUart;

/**
 * @brief Command to send detection data via communication interface
 */
class SendDetectionDataCommand : public ICommand {
  private:
    std::shared_ptr<CUart> m_communicationService;
    std::vector<IDetectionAlgorithm::DetectionResult> m_detectionResults;
    std::unique_ptr<CSerialProtocol> m_protocol;
    static std::atomic<uint32_t> s_commandIdCounter;
    uint32_t m_commandId;

  public:
    /**
     * @brief Constructor
     * @param commService Communication service for sending data
     * @param results Detection results to send
     */
    SendDetectionDataCommand(std::shared_ptr<CUart> commService,
                             const std::vector<IDetectionAlgorithm::DetectionResult>& results);

    /**
     * @brief Execute the command
     * @return global::RC_t Return code indicating execution result
     */
    global::RC_t execute() override;

    /**
     * @brief Get command type
     * @return CommandType Command type enumeration
     */
    CommandType getType() const override { return CommandType::SEND_DETECTION_DATA; }

    /**
     * @brief Get command priority
     * @return CommandPriority Priority level
     */
    CommandPriority getPriority() const override { return CommandPriority::HIGH; }

    /**
     * @brief Get command description
     * @return std::string Human-readable command description
     */
    std::string getDescription() const override;

    /**
     * @brief Get command ID for tracking
     * @return uint32_t Unique command identifier
     */
    uint32_t getId() const override { return m_commandId; }
};

/**
 * @brief Command to configure camera settings
 */
class ConfigureCameraCommand : public ICommand {
  private:
    struct CameraConfig {
        int frameRate;
        cv::Size resolution;
        int brightness;
        int contrast;
        bool autoExposure;
    } m_config;

    static std::atomic<uint32_t> s_commandIdCounter;
    uint32_t m_commandId;

  public:
    /**
     * @brief Constructor
     * @param config Camera configuration parameters
     */
    explicit ConfigureCameraCommand(const CameraConfig& config);

    /**
     * @brief Execute the command
     * @return global::RC_t Return code indicating execution result
     */
    global::RC_t execute() override;

    /**
     * @brief Get command type
     * @return CommandType Command type enumeration
     */
    CommandType getType() const override { return CommandType::CONFIGURE_CAMERA; }

    /**
     * @brief Get command description
     * @return std::string Human-readable command description
     */
    std::string getDescription() const override;

    /**
     * @brief Get command ID for tracking
     * @return uint32_t Unique command identifier
     */
    uint32_t getId() const override { return m_commandId; }
};

/**
 * @brief Command to configure detection algorithm
 */
class ConfigureDetectionCommand : public ICommand {
  private:
    IDetectionAlgorithm::DetectionConfig m_config;
    std::shared_ptr<IDetectionAlgorithm> m_algorithm;
    static std::atomic<uint32_t> s_commandIdCounter;
    uint32_t m_commandId;

  public:
    /**
     * @brief Constructor
     * @param algorithm Detection algorithm to configure
     * @param config Detection configuration parameters
     */
    ConfigureDetectionCommand(std::shared_ptr<IDetectionAlgorithm> algorithm,
                              const IDetectionAlgorithm::DetectionConfig& config);

    /**
     * @brief Execute the command
     * @return global::RC_t Return code indicating execution result
     */
    global::RC_t execute() override;

    /**
     * @brief Undo the command (restore previous configuration)
     * @return global::RC_t Return code indicating undo result
     */
    global::RC_t undo() override;

    /**
     * @brief Check if command can be undone
     * @return bool True if command supports undo
     */
    bool canUndo() const override { return true; }

    /**
     * @brief Get command type
     * @return CommandType Command type enumeration
     */
    CommandType getType() const override { return CommandType::CONFIGURE_DETECTION; }

    /**
     * @brief Get command description
     * @return std::string Human-readable command description
     */
    std::string getDescription() const override;

    /**
     * @brief Get command ID for tracking
     * @return uint32_t Unique command identifier
     */
    uint32_t getId() const override { return m_commandId; }

  private:
    IDetectionAlgorithm::DetectionConfig m_previousConfig;
    bool m_hasPreviousConfig;
};

/**
 * @brief Emergency stop command
 */
class EmergencyStopCommand : public ICommand {
  private:
    static std::atomic<uint32_t> s_commandIdCounter;
    uint32_t m_commandId;

  public:
    /**
     * @brief Constructor
     */
    EmergencyStopCommand();

    /**
     * @brief Execute the command
     * @return global::RC_t Return code indicating execution result
     */
    global::RC_t execute() override;

    /**
     * @brief Get command type
     * @return CommandType Command type enumeration
     */
    CommandType getType() const override { return CommandType::EMERGENCY_STOP; }

    /**
     * @brief Get command priority
     * @return CommandPriority Priority level
     */
    CommandPriority getPriority() const override { return CommandPriority::CRITICAL; }

    /**
     * @brief Get command description
     * @return std::string Human-readable command description
     */
    std::string getDescription() const override { return "Emergency Stop - Halt all operations"; }

    /**
     * @brief Get command ID for tracking
     * @return uint32_t Unique command identifier
     */
    uint32_t getId() const override { return m_commandId; }
};

#endif /* COMMUNICATIONCOMMAND_H */