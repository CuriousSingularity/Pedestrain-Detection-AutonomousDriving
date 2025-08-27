/***************************************************************************
 *============= Copyright by Darmstadt University of Applied Sciences =======
 ****************************************************************************
 * Filename        : CUart.cpp
 * Author          : Bharath Ramachandraiah (stbhrama@stud.h-da.de)
 * Description     : Serial Communication such as uart, usart, etc.
 * 			Can be extended for SPI, I2C as a base class.
 *
 ****************************************************************************/


// System Include Files
#include <iostream>

#include <string.h>

// Own Include Files
#include "./Common/inc/Logger.h"
#include "./HAL/inc/CUart.h"

// Namespaces
using namespace global;
using namespace std;

// Method Implementations

/**
 * @brief : Constructor
 */
CUart::CUart(std::string devPath, int flags, mode_t mode) : CResource(devPath, flags, mode) {
    // set up the speed of tx and rx
    this->m_baudrate_tx = B115200;
    this->m_baudrate_rx = B115200;

    if (this->configure() != RC_SUCCESS) {
        this->m_status = service_UNAVAILABLE;
        LOG_ERROR("CUart", "Resource configuration failed for Device " + this->getDeviceNode());
    }

    LOG_INFO("CUart", "Uart port " + this->getDeviceNode() + " constructed");
}


/**
 * @brief : Destructor
 */
CUart::~CUart() {
    LOG_INFO("CUart", "Uart port " + this->getDeviceNode() + " destructed");
}


/**
 * @brief : Configure the uart communication channel
 * 		- Baudrate
 * 		- Parity Check
 *
 * @return RC_t : Status of the uart communication channel
 */
RC_t CUart::configure() {
    LOG_INFO("CUart", "Uart port " + this->getDeviceNode() + " configuration");

    if (this->m_status != service_READY)
        return RC_ERROR_INVALID_STATE;

    struct termios tty;

    memset(&tty, 0, sizeof(tty));

    if (tcgetattr(this->m_fd_w, &tty) != 0) {
        LOG_ERROR("CUart", "Error " + std::to_string(errno) + " from tcgetattr");
        return RC_ERROR_BAD_DATA;
    }

    // set baudrate speed
    cfsetospeed(&tty, this->m_baudrate_tx);
    cfsetispeed(&tty, this->m_baudrate_rx);

    // 8-Bit character
    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;

    // disable IGNBRK for mismatched speed tests; otherwise receive break
    // as \000 chars
    tty.c_iflag &= ~IGNBRK;  // disable break processing
    tty.c_lflag = 0;         // no signaling chars, no echo,

    // no canonical processing
    tty.c_oflag = 0;      // no remapping, no delays
    tty.c_cc[VMIN] = 1;   // read blocks
    tty.c_cc[VTIME] = 5;  // 0.5 seconds read timeout

    // enable reading
    tty.c_iflag &= ~(IXON | IXOFF | IXANY);  // shut off xon/xoff ctrl
    tty.c_cflag |= (CLOCAL | CREAD);         // ignore modem controls,
    tty.c_cflag &= ~(PARENB | PARODD);       // shut off parity
    tty.c_cflag |= 0;                        // 8n1 (no parity)
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CRTSCTS;

    if (tcsetattr(this->m_fd_w, TCSANOW, &tty) != 0) {
        LOG_ERROR("CUart", "Error " + std::to_string(errno) + " from tcsetattr");
        return RC_ERROR_BAD_PARAM;
    }

    return RC_SUCCESS;
}
