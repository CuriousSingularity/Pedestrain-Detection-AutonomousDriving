/***************************************************************************
 *============= Copyright by Darmstadt University of Applied Sciences =======
 ****************************************************************************
 * Filename        : CommandQueue.h
 * Author          : Bharath Ramachandraiah (stbhrama@stud.h-da.de)
 * Description     : Priority-based command queue implementation
 *
 ****************************************************************************/

#ifndef COMMANDQUEUE_H
#define COMMANDQUEUE_H

#include "../../Common/inc/ICommand.h"
#include <queue>
#include <mutex>
#include <condition_variable>
#include <memory>

/**
 * @brief Priority-based command queue implementation
 */
class CommandQueue : public ICommandQueue {
private:
    struct CommandComparator {
        bool operator()(const std::shared_ptr<ICommand>& lhs, 
                       const std::shared_ptr<ICommand>& rhs) const {
            // Higher priority values have higher precedence
            return lhs->getPriority() < rhs->getPriority();
        }
    };

    std::priority_queue<std::shared_ptr<ICommand>, 
                       std::vector<std::shared_ptr<ICommand>>, 
                       CommandComparator> m_commandQueue;
    
    mutable std::mutex m_queueMutex;
    std::condition_variable m_queueCondition;
    bool m_shutdown;
    size_t m_maxSize;

public:
    /**
     * @brief Constructor
     * @param maxSize Maximum queue size (0 = unlimited)
     */
    explicit CommandQueue(size_t maxSize = 0);

    /**
     * @brief Destructor
     */
    virtual ~CommandQueue();

    /**
     * @brief Add command to queue
     * @param command Shared pointer to command
     * @return global::RC_t Return code indicating success or failure
     */
    global::RC_t enqueue(std::shared_ptr<ICommand> command) override;

    /**
     * @brief Remove and return next command from queue
     * @return std::shared_ptr<ICommand> Next command or nullptr if empty
     */
    std::shared_ptr<ICommand> dequeue() override;

    /**
     * @brief Remove and return next command from queue with timeout
     * @param timeoutMs Timeout in milliseconds
     * @return std::shared_ptr<ICommand> Next command or nullptr if timeout
     */
    std::shared_ptr<ICommand> dequeueWithTimeout(uint32_t timeoutMs);

    /**
     * @brief Check if queue is empty
     * @return bool True if queue is empty
     */
    bool isEmpty() const override;

    /**
     * @brief Get queue size
     * @return size_t Number of commands in queue
     */
    size_t size() const override;

    /**
     * @brief Clear all commands from queue
     */
    void clear() override;

    /**
     * @brief Shutdown the queue (stops blocking operations)
     */
    void shutdown();

    /**
     * @brief Check if queue is shutdown
     * @return bool True if shutdown
     */
    bool isShutdown() const;

    /**
     * @brief Get commands by type
     * @param commandType Type of commands to retrieve
     * @return std::vector<std::shared_ptr<ICommand>> Commands of specified type
     */
    std::vector<std::shared_ptr<ICommand>> getCommandsByType(CommandType commandType);

    /**
     * @brief Remove commands by type
     * @param commandType Type of commands to remove
     * @return size_t Number of commands removed
     */
    size_t removeCommandsByType(CommandType commandType);

    /**
     * @brief Get queue statistics
     */
    struct QueueStats {
        size_t totalSize;
        size_t criticalCount;
        size_t highCount;
        size_t normalCount;
        size_t lowCount;
    };

    /**
     * @brief Get queue statistics
     * @return QueueStats Current queue statistics
     */
    QueueStats getStatistics() const;
};

#endif /* COMMANDQUEUE_H */