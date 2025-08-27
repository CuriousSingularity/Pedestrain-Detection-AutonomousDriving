/***************************************************************************
 *============= Copyright by Darmstadt University of Applied Sciences =======
 ****************************************************************************
 * Filename        : ICommand.h
 * Author          : Bharath Ramachandraiah (stbhrama@stud.h-da.de)
 * Description     : Command pattern interface for communication operations
 *
 ****************************************************************************/

#ifndef ICOMMAND_H
#define ICOMMAND_H

#include "global.h"

#include <memory>
#include <string>
#include <vector>

/**
 * @brief Command types for different operations
 */
enum class CommandType {
    SEND_DETECTION_DATA,
    CONFIGURE_CAMERA,
    CONFIGURE_DETECTION,
    REQUEST_STATUS,
    EMERGENCY_STOP,
    SYSTEM_RESET
};

/**
 * @brief Command priority levels
 */
enum class CommandPriority { LOW = 0, NORMAL = 1, HIGH = 2, CRITICAL = 3 };

/**
 * @brief Base command interface using Command pattern
 */
class ICommand {
  public:
    virtual ~ICommand() = default;

    /**
     * @brief Execute the command
     * @return global::RC_t Return code indicating execution result
     */
    virtual global::RC_t execute() = 0;

    /**
     * @brief Undo the command (if supported)
     * @return global::RC_t Return code indicating undo result
     */
    virtual global::RC_t undo() { return global::RC_ERROR; }

    /**
     * @brief Check if command can be undone
     * @return bool True if command supports undo
     */
    virtual bool canUndo() const { return false; }

    /**
     * @brief Get command type
     * @return CommandType Command type enumeration
     */
    virtual CommandType getType() const = 0;

    /**
     * @brief Get command priority
     * @return CommandPriority Priority level
     */
    virtual CommandPriority getPriority() const { return CommandPriority::NORMAL; }

    /**
     * @brief Get command description
     * @return std::string Human-readable command description
     */
    virtual std::string getDescription() const = 0;

    /**
     * @brief Get command ID for tracking
     * @return uint32_t Unique command identifier
     */
    virtual uint32_t getId() const = 0;
};

/**
 * @brief Command queue interface for managing command execution
 */
class ICommandQueue {
  public:
    virtual ~ICommandQueue() = default;

    /**
     * @brief Add command to queue
     * @param command Shared pointer to command
     * @return global::RC_t Return code indicating success or failure
     */
    virtual global::RC_t enqueue(std::shared_ptr<ICommand> command) = 0;

    /**
     * @brief Remove and return next command from queue
     * @return std::shared_ptr<ICommand> Next command or nullptr if empty
     */
    virtual std::shared_ptr<ICommand> dequeue() = 0;

    /**
     * @brief Check if queue is empty
     * @return bool True if queue is empty
     */
    virtual bool isEmpty() const = 0;

    /**
     * @brief Get queue size
     * @return size_t Number of commands in queue
     */
    virtual size_t size() const = 0;

    /**
     * @brief Clear all commands from queue
     */
    virtual void clear() = 0;
};

#endif /* ICOMMAND_H */