/***************************************************************************
*============= Copyright by Darmstadt University of Applied Sciences =======
****************************************************************************
* Filename        : CRESOURCE.H
* Author          :
* Description     :
*
*
****************************************************************************/

#ifndef CRESOURCE_H
#define CRESOURCE_H
#include "CSemaphore.h"
#include "CMutex.h"
#include <string>
class CResource {
private:
    std::string m_resNodePath;
    int m_oflag;
    int m_mode;
    CMutex m_mutex;
public:

    CResource(std::string devPath, int oflag, int mode);

    ~CResource();

    int open();

    int close();

    virtual ssize_t read(void * buffer, size_t nbyte);

    virtual ssize_t write(void * buffer, size_t nbyte);

    virtual int configure() =0;
protected:
    int m_fd;
};
/********************
**  CLASS END
*********************/
#endif /* CRESOURCE_H */
