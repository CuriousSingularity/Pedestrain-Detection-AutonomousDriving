/***************************************************************************
 *============= Copyright by Darmstadt University of Applied Sciences =======
 ****************************************************************************
 * Filename        : CCameraService.h
 * Author          : Bharath Ramachandraiah (stbhrama@stud.h-da.de)
 * Description     : Camera service thread for continuous frame capture and buffering
 * 			Manages camera hardware and provides frames to detection algorithms
 *
 ****************************************************************************/


#ifndef CCAMERASERVICE_H
#define CCAMERASERVICE_H

// System Include Files

// Own Include Files
#include "./HAL/inc/CCamera.h"
#include "./OS/inc/CSemaphore.h"
#include "./OS/inc/CThread.h"

class CCameraService : public CThread {
  private:
    /**
     * @brief Primary camera device instance
     * Main camera hardware interface for frame capture
     */
    CCamera m_primaryCamera;

    /**
     * @brief Main thread execution routine
     * Continuously captures frames from camera and stores in ring buffer
     * Runs at configured framerate until thread termination
     */
    void run();

    static int signal_type;        ///< Signal type for camera timing

    /**
     * @brief Handle camera timing signals
     * Static signal handler for camera frame timing control
     * @param sig Signal number received
     */
    static void handleCameraSignal(int sig);

    /**
     * @brief Wait for next frame timing
     * Implements frame rate control by waiting for next capture cycle
     */
    void waitForNewFrame();

  public:
    /**
     * @brief Constructor
     * Initializes camera service with specified thread index
     * @param threadIndex Unique identifier for this thread instance
     */
    CCameraService(int threadIndex);

    /**
     * @brief Destructor
     * Cleans up camera service resources
     */
    ~CCameraService();

    /**
     * @brief Clone OpenCV matrix data
     * Static utility function to safely copy cv::Mat objects
     * @param destination Target matrix to receive copied data
     * @param source Source matrix to copy from
     */
    static void cloneMat(cv::Mat& destination, const cv::Mat& source);
};
/********************
 **  CLASS END
 *********************/
#endif /* CCAMERASERVICE_H */
