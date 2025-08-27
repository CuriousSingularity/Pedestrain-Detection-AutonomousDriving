/***************************************************************************
 *============= Copyright by Darmstadt University of Applied Sciences =======
 ****************************************************************************
 * Filename        : AsyncDetection.h
 * Author          : Bharath Ramachandraiah (stbhrama@stud.h-da.de)
 * Description     : C++20 coroutines for asynchronous detection operations
 *
 ****************************************************************************/

#ifndef ASYNCDETECTION_H
#define ASYNCDETECTION_H

#include "IDetectionAlgorithm.h"
#include "global.h"

#include <chrono>
#include <concepts>
#include <coroutine>
#include <future>
#include <memory>
#include <optional>

#include <opencv2/opencv.hpp>

namespace pedestrian_detection::async {

/**
 * @brief Task template for coroutine return types
 */
template <typename T>
struct Task {
    struct promise_type {
        T value{};
        std::exception_ptr exception{};

        Task get_return_object() {
            return Task{std::coroutine_handle<promise_type>::from_promise(*this)};
        }

        std::suspend_never initial_suspend() { return {}; }
        std::suspend_never final_suspend() noexcept { return {}; }

        void return_value(T val) { value = std::move(val); }

        void unhandled_exception() { exception = std::current_exception(); }
    };

    std::coroutine_handle<promise_type> coro;

    Task(std::coroutine_handle<promise_type> h) : coro(h) {}
    ~Task() {
        if (coro) {
            coro.destroy();
        }
    }

    // Move-only type
    Task(const Task&) = delete;
    Task& operator=(const Task&) = delete;
    Task(Task&& other) noexcept : coro(std::exchange(other.coro, {})) {}
    Task& operator=(Task&& other) noexcept {
        if (this != &other) {
            if (coro) {
                coro.destroy();
            }
            coro = std::exchange(other.coro, {});
        }
        return *this;
    }

    T get() {
        if (!coro) {
            throw std::runtime_error("Coroutine handle is null");
        }
        if (coro.promise().exception) {
            std::rethrow_exception(coro.promise().exception);
        }
        return std::move(coro.promise().value);
    }

    bool is_ready() const { return coro && coro.done(); }
};

/**
 * @brief Generator for streaming detection results
 */
template <typename T>
struct Generator {
    struct promise_type {
        T current_value{};

        Generator get_return_object() {
            return Generator{std::coroutine_handle<promise_type>::from_promise(*this)};
        }

        std::suspend_always initial_suspend() { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }

        std::suspend_always yield_value(T value) {
            current_value = std::move(value);
            return {};
        }

        void unhandled_exception() { throw; }

        void return_void() {}
    };

    std::coroutine_handle<promise_type> coro;

    Generator(std::coroutine_handle<promise_type> h) : coro(h) {}
    ~Generator() {
        if (coro) {
            coro.destroy();
        }
    }

    // Move-only type
    Generator(const Generator&) = delete;
    Generator& operator=(const Generator&) = delete;
    Generator(Generator&& other) noexcept : coro(std::exchange(other.coro, {})) {}
    Generator& operator=(Generator&& other) noexcept {
        if (this != &other) {
            if (coro) {
                coro.destroy();
            }
            coro = std::exchange(other.coro, {});
        }
        return *this;
    }

    struct iterator {
        std::coroutine_handle<promise_type> coro;

        iterator(std::coroutine_handle<promise_type> h) : coro(h) {}

        iterator& operator++() {
            coro.resume();
            if (coro.done()) {
                coro = {};
            }
            return *this;
        }

        T operator*() const { return coro.promise().current_value; }

        bool operator==(const iterator& other) const { return coro == other.coro; }
    };

    iterator begin() {
        if (coro) {
            coro.resume();
            if (coro.done()) {
                return iterator{{}};
            }
        }
        return iterator{coro};
    }

    iterator end() { return iterator{{}}; }
};

/**
 * @brief Awaitable for frame processing
 */
struct FrameAwaitable {
    cv::Mat frame;
    std::shared_ptr<IDetectionAlgorithm> algorithm;
    mutable std::optional<std::vector<IDetectionAlgorithm::DetectionResult>> results;

    bool await_ready() const noexcept {
        return false;  // Always suspend to allow async processing
    }

    void await_suspend(std::coroutine_handle<> handle) const {
        // Process frame asynchronously
        std::thread([this, handle]() {
            try {
                results = algorithm->detectRange(frame);
                handle.resume();
            } catch (...) {
                handle.resume();
            }
        }).detach();
    }

    std::vector<IDetectionAlgorithm::DetectionResult> await_resume() const {
        if (results) {
            return std::move(*results);
        }
        return {};
    }
};

/**
 * @brief Awaitable timer for periodic operations
 */
struct TimerAwaitable {
    std::chrono::milliseconds duration;

    TimerAwaitable(std::chrono::milliseconds ms) : duration(ms) {}

    bool await_ready() const noexcept { return duration.count() <= 0; }

    void await_suspend(std::coroutine_handle<> handle) const {
        std::thread([duration = this->duration, handle]() {
            std::this_thread::sleep_for(duration);
            handle.resume();
        }).detach();
    }

    void await_resume() const noexcept {}
};

/**
 * @brief Async detection service using coroutines
 */
class AsyncDetectionService {
  private:
    std::shared_ptr<IDetectionAlgorithm> m_algorithm;
    std::atomic<bool> m_running{false};

  public:
    explicit AsyncDetectionService(std::shared_ptr<IDetectionAlgorithm> algorithm)
        : m_algorithm(std::move(algorithm)) {}

    /**
     * @brief Process single frame asynchronously
     * @param frame Input frame
     * @return Task<std::vector<IDetectionAlgorithm::DetectionResult>> Detection results
     */
    Task<std::vector<IDetectionAlgorithm::DetectionResult>> processFrameAsync(cv::Mat frame) {
        co_return co_await FrameAwaitable{std::move(frame), m_algorithm};
    }

    /**
     * @brief Continuous frame processing generator
     * @param frameSource Function that provides frames
     * @param interval Processing interval
     * @return Generator<std::vector<IDetectionAlgorithm::DetectionResult>> Stream of results
     */
    Generator<std::vector<IDetectionAlgorithm::DetectionResult>>
    processFrameStream(std::function<cv::Mat()> frameSource,
                       std::chrono::milliseconds interval = std::chrono::milliseconds{33}) {
        m_running = true;

        while (m_running) {
            cv::Mat frame = frameSource();
            if (!frame.empty()) {
                auto results = co_await FrameAwaitable{frame, m_algorithm};
                co_yield results;
            }

            co_await TimerAwaitable{interval};
        }
    }

    /**
     * @brief Batch process multiple frames
     * @param frames Vector of frames to process
     * @return Task<std::vector<std::vector<IDetectionAlgorithm::DetectionResult>>> Batch results
     */
    Task<std::vector<std::vector<IDetectionAlgorithm::DetectionResult>>>
    processBatchAsync(std::vector<cv::Mat> frames) {
        std::vector<std::vector<IDetectionAlgorithm::DetectionResult>> results;
        results.reserve(frames.size());

        for (auto& frame : frames) {
            auto frameResults = co_await FrameAwaitable{std::move(frame), m_algorithm};
            results.emplace_back(std::move(frameResults));
        }

        co_return results;
    }

    /**
     * @brief Process frames with timeout
     * @param frame Input frame
     * @param timeout Maximum processing time
     * @return Task<std::optional<std::vector<IDetectionAlgorithm::DetectionResult>>> Results or
     * timeout
     */
    Task<std::optional<std::vector<IDetectionAlgorithm::DetectionResult>>>
    processFrameWithTimeout(cv::Mat frame, std::chrono::milliseconds timeout) {
        auto start = std::chrono::steady_clock::now();

        auto results = co_await FrameAwaitable{std::move(frame), m_algorithm};

        auto elapsed = std::chrono::steady_clock::now() - start;
        if (elapsed > timeout) {
            co_return std::nullopt;
        }

        co_return results;
    }

    /**
     * @brief Stop continuous processing
     */
    void stop() { m_running = false; }

    /**
     * @brief Check if service is running
     * @return bool True if running
     */
    bool isRunning() const { return m_running; }
};

/**
 * @brief Factory function for creating async detection service
 */
template <pedestrian_detection::concepts::DetectionAlgorithm AlgorithmType>
std::unique_ptr<AsyncDetectionService> createAsyncDetectionService() {
    auto algorithm = std::make_shared<AlgorithmType>();
    return std::make_unique<AsyncDetectionService>(algorithm);
}

}  // namespace pedestrian_detection::async

#endif /* ASYNCDETECTION_H */