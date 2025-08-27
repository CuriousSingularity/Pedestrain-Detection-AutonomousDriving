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
#include "./Lib/inc/CRingBuffer.h"

// Namespace
using namespace cv;
using namespace std;
using namespace global;

// Macros
// Global variables
CRingBuffer<cv::Mat, FRAMERATE> g_framesBuffer;

void CCameraService::cloneMat(cv::Mat& lhs, const cv::Mat& rhs) {
    lhs = rhs.clone();
}


// Method Implementations
/**
 * @brief : Constructor
 *
 * @param threadIndex 	: Thread Index
 */
CCameraService::CCameraService(int threadIndex)
    : CThread(threadIndex, [this]() { this->run(); }),
      m_camera_0("/dev/video0", O_RDWR | O_NOCTTY | O_SYNC, S_IRWXU) {
    // nothing
}


/**
 * @brief : Destructor
 */
CCameraService::~CCameraService() {
    // nothing
}

int CCameraService::signal_type = 0;

void CCameraService::__camera_cyclic__signal_handler(int sig) {
    CCameraService::signal_type = sig;
}

void CCameraService::wait_for_newFrame() {
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
    cout << "INFO\t: Camera Service " << this->getThreadIndex()
         << " started with ID : " << pthread_self() << endl;

    cv::Mat image;
    ssize_t wBytes;

    cout << "INFO\t: Running Camera Service " << this->getThreadIndex() << " : " << pthread_self()
         << endl;

    while (1) {
        this->wait_for_newFrame();

        // read new image to ring buffer
        if (this->m_camera_0.read(&image, 0, wBytes) != RC_SUCCESS)
            continue;

        // store the frame to ringbuffer for consumers
        if (g_framesBuffer.writeData(image, cloneMat) != RC_SUCCESS) {
            cout << "ERROR\t: Ring Buffer Write error" << endl;
        }
    }
}
