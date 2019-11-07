/***************************************************************************
 *============= Copyright by Darmstadt University of Applied Sciences =======
 ****************************************************************************
 * Filename        : CCom.h
 * Author          : Bharath Ramachandraiah (stbhrama@stud.h-da.de)
 * Description     : Serial Communication such as uart, usart, etc. 
 * 			Can be extended for SPI, I2C as a base class.
 *
 ****************************************************************************/


#ifndef CCOM_H
#define CCOM_H

//System Include Files
#include <termios.h>
#include <unistd.h>

//Own Include Files
#include "./OS/inc/CResource.h"

class CCom : public CResource {
private:

	/**
	 * @brief : Baudrate for the uart communication transmission channel
	 */
	speed_t	m_baudrate_tx;

	/**
	 * @brief : Baudrate for the uart communication reception channel
	 */
	speed_t	m_baudrate_rx;

	/**
	 * @brief : Configure the uart communication channel
	 * 		- Baudrate
	 * 		- Parity Check
	 *
	 * @return RC_t : Status of the uart communication channel 
	 */
	global::RC_t configure();

public:

	/**
	 * @brief : Constructor
	 */
	CCom();

	/**
	 * @brief : Destructor
	 */
	virtual ~CCom();

};
/********************
 **  CLASS END
 *********************/
#endif /* CCOM_H */
