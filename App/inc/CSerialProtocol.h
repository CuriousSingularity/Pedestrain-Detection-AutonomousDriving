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


//Macros
#define SOP				(0xFA)
#define EOP				(0xFD)

// size
// change the paylock blocks according to the requirement
#define PAYLOAD_BLOCKS			10
#define BLOCK_SIZE			(sizeof(CSerialProtocol::object_detection_block_t))

#define SOP_SIZE			1
#define EOP_SIZE			1
#define DLC_SIZE			1
#define DLC				4
#define PAYLOAD_SIZE			(PAYLOAD_BLOCKS * BLOCK_SIZE)

#define SOP_INDEX			0
#define DLC_INDEX			(SOP_INDEX + SOP_SIZE)
#define PAYLOAD_INDEX			(DLC_INDEX + DLC_SIZE)
#define EOP_INDEX			(PAYLOAD_INDEX + DLC)

#define PROTOCOL_BUF_MAX_SIZE		(SOP_SIZE + DLC_SIZE + DLC + EOP_SIZE)

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

	typedef struct
	{
		int8_t theta;
		int8_t delta_theta;
		//uint16_t probability;
		//uint8_t opinion;
	} object_detection_block_t;

	typedef struct
	{
		std::vector<object_detection_block_t> blks;
	} object_detection_frame_t;

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
