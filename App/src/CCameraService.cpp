/***************************************************************************
 *============= Copyright by Darmstadt University of Applied Sciences =======
 ****************************************************************************
 * Filename        : CCameraService.cpp
 * Author          : Bharath Ramachandraiah (stbhrama@stud.h-da.de)
 * Description     : Camera Service which fills up the request with an image
 *
 ****************************************************************************/


// System Include Files
#include <iostream>

#include <fcntl.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

// Own Include Files
#include "./App/inc/CCameraService.h"
#include "./Common/inc/Logger.h"
#include "./Lib/inc/CRingBuffer.h"

// Namespace
using namespace cv;
using namespace std;
using namespace global;

// Macros
// Global variables
CRingBuffer<cv::Mat, FRAMERATE> g_framesBuffer;

void CCameraService::cloneMat(cv::Mat& destination, const cv::Mat& source) {
    destination = source.clone();
}


// Method Implementations
/**
 * @brief : Constructor
 *
 * @param threadIndex 	: Thread Index
 */
CCameraService::CCameraService(int threadIndex)
    : CThread(threadIndex, [this]() { this->run(); }),
      m_primaryCamera("/dev/video0", O_RDWR | O_NOCTTY | O_SYNC, S_IRWXU) {
    // nothing
}


/**
 * @brief : Destructor
 */
CCameraService::~CCameraService() {
    // nothing
}

int CCameraService::signal_type = 0;

void CCameraService::handleCameraSignal(int sig) {
    CCameraService::signal_type = sig;
}

void CCameraService::waitForNewFrame() {
    struct timespec delta = {
        .tv_sec = 0,                         // seconds
        .tv_nsec = (999999999 / FRAMERATE),  // nano seconds
    };

    while (nanosleep(&delta, &delta))
        ;
}
/**
 * @brief : Main routine for the thread
 *
 * @return - to join the thread
 */
void CCameraService::run() {
    // The Threads runs here
    LOG_INFO("CCameraService", "Camera Service " + std::to_string(this->getThreadIndex()) + " started with ID : " + std::to_string(pthread_self()));

    cv::Mat image;
    ssize_t wBytes;

    LOG_INFO("CCameraService", "Running Camera Service " + std::to_string(this->getThreadIndex()) + " : " + std::to_string(pthread_self()));

    while (1) {
        this->waitForNewFrame();

        // read new image to ring buffer
        if (this->m_primaryCamera.read(&image, 0, wBytes) != RC_SUCCESS)
            continue;

        // store the frame to ringbuffer for consumers
        if (g_framesBuffer.writeData(image, cloneMat) != RC_SUCCESS) {
            LOG_ERROR("CCameraService", "Ring Buffer Write error");
        }
    }
}
