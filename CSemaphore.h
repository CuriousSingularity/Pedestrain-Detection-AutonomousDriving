/***************************************************************************
*============= Copyright by Darmstadt University of Applied Sciences =======
****************************************************************************
* Filename        : CSEMAPHORE.H
* Author          :
* Description     :
*
*
****************************************************************************/

#ifndef CSEMAPHORE_H
#define CSEMAPHORE_H

#include <string>

class CSemaphore {
private:

    void open();

    void close();

public:

    CSemaphore(std::string name, int oflag, mode_t mode, unsigned int arg);

    ~CSemaphore();

    int wait();

    int trywait();

    int timedwait();

    int post();

private:
    std::string m_name;
    int oflag;
    mode_t mode;
    unsigned int value;
};
/********************
**  CLASS END
*********************/
#endif /* CSEMAPHORE_H */
