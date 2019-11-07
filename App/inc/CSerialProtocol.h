/***************************************************************************
 *============= Copyright by Darmstadt University of Applied Sciences =======
 ****************************************************************************
 * Filename        : CSerialProtocol.h
 * Author          : Bharath Ramachandraiah (stbhrama@stud.h-da.de)
 * Description     : Protocol parser from the serial port
 *
 ****************************************************************************/


#ifndef CSERIALPROTOCOL_H
#define CSERIALPROTOCOL_H

//System Include Files
#include <stdint.h>
#include <vector>

//Own Include Files
#include "./global.h"
#include "./HAL/inc/CUart.h"

class CSerialProtocol {
private:

public:

	/**
	 * @brief : structure to store the lidar information
	 */
	typedef struct
	{
		uint16_t 	angle;
		uint16_t	area_angle;
		uint16_t	distance;
	} object_detection_lidar_t;

	/**
	 * @brief : Constructor
	 */
	CSerialProtocol();

	/**
	 * @brief : Destructor
	 */
	virtual ~CSerialProtocol();

	/**
	 * @brief : Read the lidar detected objects
	 *
	 * @param detectedObjects : Objects from the lidar are filled into the vector
	 *
	 * @return : status
	 */
	global::RC_t readRequest(std::vector<object_detection_lidar_t> &detectedObjects, CUart *pComResource);

};
/********************
 **  CLASS END
 *********************/
#endif /* CSERIALPROTOCOL_H */
