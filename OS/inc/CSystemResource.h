/***************************************************************************
*============= Copyright by Darmstadt University of Applied Sciences =======
****************************************************************************
* Filename        : CSYSTEMRESOURCE.H
* Author          :
* Description     :
*
*
****************************************************************************/

#ifndef CSYSTEMRESOURCE_H
#define CSYSTEMRESOURCE_H
#include "CCom.h"
#include "CCamera.h"
class CSystemResource {
private:
    CCom m_serial;
    CCamera m_camera;
public:
    CSystemResource(){};

    ~CSystemResource(){};

    CCom& getSerialResourceReference();

    CCamera& getCameraResourceReference();
};
/********************
**  CLASS END
*********************/
#endif /* CSYSTEMRESOURCE_H */
