/***************************************************************************
*============= Copyright by Darmstadt University of Applied Sciences =======
****************************************************************************
* Filename        : CCAMERA.H
* Author          :
* Description     :
*
*
****************************************************************************/

#ifndef CCAMERA_H
#define CCAMERA_H
#include "CResource.h"
class CCamera : public CResource {
private:
    int configure();

public:
    CCamera();

    virtual ~CCamera();
};
/********************
**  CLASS END
*********************/
#endif /* CCAMERA_H */
