/***************************************************************************
*============= Copyright by Darmstadt University of Applied Sciences =======
****************************************************************************
* Filename        : CPEDESTRIANDETECTION.H
* Author          :
* Description     :
*
*
****************************************************************************/

#ifndef CPEDESTRIANDETECTION_H
#define CPEDESTRIANDETECTION_H
#include "CCom.h"
#include "CCamera.h"
#include "CSystemResource.h"
#include "CThread.h"
#include "CCameraDataProcessing.h"
#include "CSerialDataProcessing.h"
#include "CSemaphore.h"

class CPedestrianDetection {
private:
    CSystemResource m_sysRes;
    CCameraDataProcessing m_thread_camera;
    CSerialDataProcessing m_thread_comm;
public:

    CPedestrianDetection();

    void run();

    ~CPedestrianDetection();
};
/********************
**  CLASS END
*********************/
#endif /* CPEDESTRIANDETECTION_H */
