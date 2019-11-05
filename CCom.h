/***************************************************************************
*============= Copyright by Darmstadt University of Applied Sciences =======
****************************************************************************
* Filename        : CCOM.H
* Author          :
* Description     :
*
*
****************************************************************************/

#ifndef CCOM_H
#define CCOM_H
#include "CResource.h"
class CCom : public CResource {
private:
    int configure();
    
public:
    CCom();

    virtual ~CCom();

};
/********************
**  CLASS END
*********************/
#endif /* CCOM_H */
