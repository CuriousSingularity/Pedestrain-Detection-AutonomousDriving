/***************************************************************************
 *============= Copyright by Darmstadt University of Applied Sciences =======
 ****************************************************************************
 * Filename        : CResource.cpp
 * Author          : Bharath Ramachandraiah (stbhrama@stud.h-da.de)
 * Description     : Resource class to interact with the system hardware.
 * 			eg: UART, Camera, FileSystem, MMC, I2C, SPI etc.
 *
 ****************************************************************************/


// System Include Files
#include <iostream>

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

// Own Include Files
#include "./Common/inc/Logger.h"
#include "./OS/inc/CResource.h"

// Method Implementations
using namespace std;
using namespace global;

/**
 * @brief : Constructor
 *
 * @param devPath 	: device node path
 * @param oflag		: access mode flags
 * @param mode		: permissio mode
 */
CResource::CResource(string devPath, int flag, mode_t mode) {
    this->m_fd_w = 0;
    this->m_fd_r = 0;
    this->m_resNodePath = devPath;
    this->m_flags = flag;
    this->m_mode = mode;
    this->m_status = service_UNDEFINED;

    if (this->open() != RC_SUCCESS) {
        this->m_status = service_UNAVAILABLE;
        LOG_ERROR("CResource", "Failed to open " + this->m_resNodePath + " with error code " + std::to_string(errno));
    } else {
        this->m_status = service_READY;
        LOG_INFO("CResource", "Device " + this->m_resNodePath + " opened for operation");
    }

    // the hardware has read and write discriptor to be same.
    // but for software resource, this is configurable.
    this->m_fd_r = this->m_fd_w;
}


/**
 * @brief : Destructor
 */
CResource::~CResource() {
    if (this->close() != RC_SUCCESS) {
        this->m_status = service_UNDEFINED;
        LOG_ERROR("CResource", "Failed to close " + this->m_resNodePath + " with error code " + std::to_string(errno));
    } else {
        this->m_status = service_UNDEFINED;
        LOG_INFO("CResource", "Device " + this->m_resNodePath + " closed from operation");
    }
}


/**
 * @brief : Open the device for reading or writing
 *
 * @return RC_t : status of open
 */
RC_t CResource::open() {
    RC_t ret = RC_SUCCESS;

    if (this->m_status != service_UNDEFINED)
        return RC_ERROR_INVALID_STATE;

    if (this->m_mutex_w.lock() != RC_SUCCESS) {
        LOG_ERROR("CResource", "Resource lock failed for Device " + this->m_resNodePath + " during open");
        return RC_ERROR_BUSY;
    }

    if (!this->m_resNodePath.empty()) {
        this->m_fd_w = ::open(this->m_resNodePath.c_str(), this->m_flags, this->m_mode);

        if (this->m_fd_w == -1) {
            LOG_ERROR("CResource", "Resource open failed for Device " + this->m_resNodePath);

            ret = RC_ERROR_OPEN;
        }
    }

    if (this->m_mutex_w.unlock() != RC_SUCCESS) {
        LOG_ERROR("CResource", "Resource unlock failed for Device " + this->m_resNodePath + " during open");
    }

    return ret;
}


/**
 * @brief : Close the device
 *
 * @return RC_t : status of close
 */
RC_t CResource::close() {
    RC_t ret = RC_SUCCESS;

    if (this->m_status != service_READY)
        return RC_ERROR_INVALID_STATE;

    if (this->m_mutex_w.lock() != RC_SUCCESS) {
        LOG_ERROR("CResource", "Resource lock failed for Device " + this->m_resNodePath + " during close");
        return RC_ERROR_BUSY;
    }

    this->m_status = service_UNDEFINED;

    ::close(this->m_fd_w);
    ::close(this->m_fd_r);

    this->m_fd_r = 0;
    this->m_fd_w = 0;

    if (this->m_mutex_w.unlock() != RC_SUCCESS) {
        LOG_ERROR("CResource", "Resource unlock failed for Device " + this->m_resNodePath + " during close");
    }

    return ret;
}


/**
 * @brief : Read from the device
 *
 * @param buffer	: buffer to read the data from
 * @param nByte		: number of bytes to be read
 * @param rByte		: number of bytes actually read
 *
 * @return RC_t - status of read
 */
RC_t CResource::read(void* buffer, const size_t nByte, ssize_t& rByte) {
    RC_t ret = RC_ERROR_READ_FAILS;

    if (this->m_status != service_READY)
        return RC_ERROR_INVALID_STATE;

    if (buffer) {
        if (this->m_mutex_r.lock() != RC_SUCCESS) {
            LOG_ERROR("CResource", "Resource lock failed for Device " + this->m_resNodePath + " during read");
            return RC_ERROR_BUSY;
        }

        if ((rByte = ::read(this->m_fd_r, buffer, nByte)) == -1) {
            LOG_ERROR("CResource", "Read failed for Device " + this->m_resNodePath);

            ret = RC_ERROR_READ_FAILS;
        } else {
            ret = RC_SUCCESS;
        }

        if (this->m_mutex_r.unlock() != RC_SUCCESS) {
            LOG_ERROR("CResource", "Resource unlock failed for Device " + this->m_resNodePath + " during read");
        }
    } else {
        ret = RC_ERROR_NULL;
    }

    return ret;
}

/**
 * @brief : Write to the device
 *
 * @param buffer	: buffer to write the data to
 * @param nByte		: number of bytes to be written
 * @param rByte		: number of bytes actually written
 *
 * @return RC_t - status of read
 */
RC_t CResource::write(const void* buffer, const size_t nByte, ssize_t& rByte) {
    RC_t ret = RC_ERROR_WRITE_FAILS;

    if (this->m_status != service_READY)
        return RC_ERROR_INVALID_STATE;

    if (buffer) {
        if (this->m_mutex_w.lock() != RC_SUCCESS) {
            LOG_ERROR("CResource", "Resource lock failed for Device " + this->m_resNodePath + " during write");
            return RC_ERROR_BUSY;
        }

        if ((rByte = ::write(this->m_fd_w, buffer, nByte)) == -1) {
            LOG_ERROR("CResource", "Write failed for Device " + this->m_resNodePath);

            ret = RC_ERROR_WRITE_FAILS;
        } else {
            ret = RC_SUCCESS;
        }

        if (this->m_mutex_w.unlock() != RC_SUCCESS) {
            LOG_ERROR("CResource", "Resource unlock failed for Device " + this->m_resNodePath + " during write");
        }
    } else {
        ret = RC_ERROR_NULL;
    }

    return ret;
}


/**
 * @brief : Get the device node path
 *
 * @return string
 */
string CResource::getDeviceNode() {
    return (this->m_resNodePath);
}
