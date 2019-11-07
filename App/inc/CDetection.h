/***************************************************************************
 *============= Copyright by Darmstadt University of Applied Sciences =======
 ****************************************************************************
 * Filename        : CDetection.h
 * Author          : Nicolas Ojeda Leon (stnioied@stdu.h-da.de)
 * 			Bharath Ramachandraiah (stbhrama@stud.h-da.de)
 * Description     : Camera Detection Algorithm is implemented here.
 *
 ****************************************************************************/

#ifndef CDETECTION_H
#define CDETECTION_H

//System Include Files

//Own Include Files
#include "./OS/inc/CThread.h"
#include "./OS/inc/CSemaphore.h"

class CDetection : public CThread {
private:

	/**
	 * @brief : Main routine for the thread
	 *
	 * @return - to join the thread
	 */
	void run();

public:

	typedef enum
	{
		HOG_DETECTION_DEFAULT = 0,
		HOG_DETECTION_DAIMLER,
	} hog_detector_t;

	typedef struct 
	{
		uint8_t hitThreshold;			// Percentage : 0 - 100
		uint8_t winStride;				// Number of pixels
		uint8_t padding;				// Number of pixels
		float 	scale;					// scaling factor : must be greater than 1.0
		uint8_t finalThreshold;			// Percentage
		uint8_t nmsThreshold;			// Percentage
		uint8_t nmsNeighbors;			// Count
		hog_detector_t detectionModel;	// Detection model type
	} hog_config_t;

	/**
	 * @brief : Constructor
	 *
	 * @param threadIndex 	: Thread Index
	 * @param sysResource	: Global resource pointer
	 * @param entry		: Entry function for the thread
	 * @param arg		: Arguments to the thread
	 */
	CDetection(int threadIndex, CThread::start_routine_t entry = NULL, void *arg = NULL);

	/**
	 * @brief : Destructor
	 */
	~CDetection();

	/**
	 * @brief : Friend function used to create the thread 
	 *
	 * @param arg : arguments to the thread
	 *
	 * @return 
	 */
	friend void *friend_detection(void *arg);

};
/********************
 **  CLASS END
 *********************/
#endif /* CDETECTION_H */
