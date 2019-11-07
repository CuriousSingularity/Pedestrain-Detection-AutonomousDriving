/***************************************************************************
 *============= Copyright by Darmstadt University of Applied Sciences =======
 ****************************************************************************
 * Filename        : CResource.h
 * Author          : Bharath Ramachandraiah (stbhrama@stud.h-da.de)
 * Description     : Resource class to interact with the system hardware.
 * 			eg: UART, Camera, FileSystem, MMC, I2C, SPI etc.
 *
 ****************************************************************************/


#ifndef CRESOURCE_H
#define CRESOURCE_H

//System Include Files
#include <string>

//Own Include Files
#include "./OS/inc/CSemaphore.h"
#include "./OS/inc/CMutex.h"

class CResource {
private:

	/**
	 * @brief : Resource / device node path 
	 */
	std::string m_resNodePath;

	/**
	 * @brief : Device access modes 
	 */
	int m_flags;

	/**
	 * @brief : Device permission modes
	 */
	mode_t m_mode;

	/**
	 * @brief : Open the device for reading or writing
	 *
	 * @return RC_t : status of open
	 */
	global::RC_t open();

	/**
	 * @brief : Close the device
	 *
	 * @return RC_t : status of close
	 */
	global::RC_t close();

protected:

	/**
	 * @brief : Mutex to read protect the resource
	 */
	CMutex m_mutex_r;

	/**
	 * @brief : Mutex to write protect the resource
	 */
	CMutex m_mutex_w;

	/**
	 * @brief : File Descriptor of the resource
	 */
	int m_fd_w;

	/**
	 * @brief : File Descriptor of the resource 
	 */
	int m_fd_r;

	/**
	 * @brief : Status of the Resource
	 */
	global::service_state_t m_status;

public:

	/**
	 * @brief : Constructor
	 *
	 * @param devPath 	: device node path
	 * @param oflag		: access mode flags
	 * @param mode		: permissio mode
	 */
	CResource(std::string devPath = "", int flag = 0, mode_t mode = 0);

	/**
	 * @brief : Destructor
	 */
	~CResource();

	/**
	 * @brief : Get the device node path
	 *
	 * @return string
	 */
	std::string getDeviceNode();

	/**
	 * @brief : Read from the device 
	 *
	 * @param buffer	: buffer to read the data from
	 * @param nByte		: number of bytes to be read
	 * @param rByte		: number of bytes actually read
	 *
	 * @return RC_t - status of read
	 */
	virtual global::RC_t read(void *buffer, const size_t nByte, ssize_t &rByte);

	/**
	 * @brief : Write to the device 
	 *
	 * @param buffer	: buffer to write the data to
	 * @param nByte		: number of bytes to be written
	 * @param rByte		: number of bytes actually written
	 *
	 * @return RC_t - status of read
	 */
	virtual global::RC_t write(const void *buffer, const size_t nByte, ssize_t &wByte);

	/**
	 * @brief : Device configuration - A pure virtual function
	 *
	 * @return RC_t - status of the device configuration
	 */
	virtual global::RC_t configure() = 0;
};
/********************
 **  CLASS END
 *********************/
#endif /* CRESOURCE_H */
