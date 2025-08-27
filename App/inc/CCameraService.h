/***************************************************************************
 *============= Copyright by Darmstadt University of Applied Sciences =======
 ****************************************************************************
 * Filename        : CCameraService.h
 * Author          : Bharath Ramachandraiah (stbhrama@stud.h-da.de)
 * Description     : Serial Data Processing thread - packet reception and processing
 * 			it with Service-ID, Local-ID; predefined protocol.
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
     * @brief : Camera channel 0
     */
    CCamera m_camera_0;

    /**
     * @brief : Main routine for the thread
     *
     * @return - to join the thread
     */
    void run();

    static int signal_type;

    static void __camera_cyclic__signal_handler(int sig);

    void wait_for_newFrame();

  public:
    /**
     * @brief : Constructor
     *
     * @param threadIndex 	: Thread Index
     */
    CCameraService(int threadIndex);

    /**
     * @brief : Destructor
     */
    ~CCameraService();

    static void cloneMat(cv::Mat& lhs, const cv::Mat& rhs);
};
/********************
 **  CLASS END
 *********************/
#endif /* CCAMERASERVICE_H */
