/***************************************************************************
 *============= Copyright by Darmstadt University of Applied Sciences =======
 ****************************************************************************
 * Filename        : CDetection.h
 * Author          : Nicolas Ojeda Leon (stnioied@stdu.h-da.de)
 * 			Bharath Ramachandraiah (stbhrama@stud.h-da.de)
 * Description     : Camera Detection Algorithm is implemented here.
 *
 ****************************************************************************/

// System includes
#include <vector>
#include <iostream>
#include <opencv2/opencv.hpp>

//Own Include Files
#include "./App/inc/CDetection.h"
#include "./Lib/inc/nms.h"

//Namespace
using namespace std;
using namespace global;
using namespace cv;

//Macros
#define DISABLE					0
#define ENABLE					1
#define DISPLAY_CONNECTED		DISABLE


// local constants
static HOGDescriptor hog;

static const CDetection::hog_config_t hog_config_param {
	.hitThreshold	= 55,	// Range : 0-100
	.winStride		= 8,	// Range : 1-32
	.padding 		= 8,	// Range : 0-64
	.scale 			= 1.08,	// Range : > 1.0
	.finalThreshold = 0,	// Range : 0-100
	.nmsThreshold 	= 0,	// Range : 0-100
	.nmsNeighbors 	= 0,	// Range : 0-100
	.detectionModel	= CDetection::HOG_DETECTION_DEFAULT,
};

//Method Implementations
/**
 * @brief : Constructor
 *
 * @param threadIndex 	: Thread Index
 * @param sysResource	: Global resource pointer
 * @param entry		: Entry function for the thread
 * @param arg		: Arguments to the thread
 */
CDetection::CDetection(int threadIndex, CThread::start_routine_t entry, void *arg) : CThread(threadIndex, entry, arg)
{
	//nothing
}

/**
 * @brief : Destructor
 */
CDetection::~CDetection()
{
	// nothing
}

#include <unistd.h>
/**
 * @brief : Main routine for the thread
 *
 * @return - to join the thread
 */
void CDetection::run()
{
	// The Threads runs here
	cout << "INFO\t: Detection Algorithm Thread " << this->getThreadIndex() << " started with ID : " << pthread_self() << endl;

	while (1)
	{
		sleep(1);
		cout << getpid() << endl;
	}

#if 0
	// Locals declaration:
	vector<Rect> detections;		// Vector of boxes where a detection was achieved
	vector<double> detection_weights;
	Mat image;
	vector<Rect> nmsDetections;
	ssize_t rBytes = 0;

#if (defined(DISPLAY_CONNECTED) && (DISPLAY_CONNECTED == ENABLE))
	namedWindow("Detected Image", cv::WINDOW_AUTOSIZE);
	uint8_t counter = 0;
#endif

	double t_start = 0;

	while (1)
	{
		cout << "INFO\t: Camera Running Thread " << this->getThreadIndex() << " started with ID : " << pthread_self() << endl;

		// Hog run:
		// TODO NOL: Acquire frame from camera
		if (RC_SUCCESS == this->m_pSysRes->getCameraResourceReference()->read(&image, 0, rBytes))
		{
			if (detector == DET_DAIMLER){
				hog.winSize = Size(48, 96);
				hog.setSVMDetector(HOGDescriptor::getDaimlerPeopleDetector());
			}
			else{
				hog.winSize = Size(64, 128);
				hog.setSVMDetector(HOGDescriptor::getDefaultPeopleDetector());
			}

			t_start = getTickCount();
			hog.detectMultiScale(	image,									/* Source image */
					detections,					/* foundLocations, vector of Rect objects with the boxes where a person was detected */
					detection_weights,				/* Weights of each detection. Vector of same dimension as previous parameter */
					(float) hitThreshold.value / 100,		/* hitThreshold: SVM threshold to filter final results */
					Size(winStride.value, winStride.value),		/* Windows stride: Horizontal and vertical step in pixels for the template matching process */
					Size(padding.value, padding.value),		/* Padding: PENDING */
					1 + (float) scale.value / 100,			/* Scale: Scale stride for the image pyramid */
					(float) finalThreshold.value / 100		/* FinalThreshold: PENDING */
					);
			//	cout << "Found " << detections.size() << " matches";


			nms(detections, nmsDetections, (float) nmsThreshold.value / 100, nmsNeighbors.value);

			//	cout << " - After NMS: " << nmsDetections.size() << " matches" << endl;
#if (defined(DISPLAY_CONNECTED) && (DISPLAY_CONNECTED == ENABLE))
			for (unsigned int i= 0; i < nmsDetections.size() ; i++)
			{
				rectangle(image, Point(nmsDetections[i].x, nmsDetections[i].y), Point(nmsDetections[i].x + nmsDetections[i].width, nmsDetections[i].y + nmsDetections[i].height), Scalar(0, 0, 255), 5, LINE_8);
			}


			//			rectangle(image, Point(0, 0), Point(20, 20), Scalar(0, 0, 0), -1);
			//			putText(image, to_string((int) counter), Point(0,0), FONT_HERSHEY_PLAIN, 4,  Scalar(255,255,255), 2 , LINE_AA , false);

			counter = (counter + 1) % 100;
			imshow( "Detected Image", image );
			waitKey(1);
#endif
			cout << endl << "Time elapsed: " << (getTickCount() - t_start) / getTickFrequency() << endl;
		}
	}
	#endif
}


/**
 * @brief : Friend function used to create the thread 
 *
 * @param arg : arguments to the thread
 *
 * @return 
 */
void *friend_detection(void *arg)
{
	if (arg)
		cout << "INFO\t: Thread " << __func__ << endl;

	CDetection *ptr = static_cast<CDetection *>(arg);

	if (ptr)
	{
		ptr->run();
	}

	return NULL;
}
