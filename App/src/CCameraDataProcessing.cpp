/***************************************************************************
 *============= Copyright by Darmstadt University of Applied Sciences =======
 ****************************************************************************
 * Filename        : CCameraDataProcessing.h
 * Author          : Nicolas Ojeda Leon (stnioied@stdu.h-da.de)
 * 			Bharath Ramachandraiah (stbhrama@stud.h-da.de)
 * Description     : Camera Detection Algorithm is implemented here.
 *
 ****************************************************************************/

// System includes
#include <vector>
#include <opencv2/opencv.hpp>
#include <iostream>

//Own Include Files
#include "./App/inc/CCameraDataProcessing.h"
#include "./Lib/inc/nms.h"


//Namespace
using namespace std;
using namespace global;
using namespace cv;


// TODO NOL Cheack what to do with all this:
#define __SHOW_RESULT
// local types
typedef struct{
	int value;
	int max;
} parameter_t;

typedef enum{
	DET_DAIMLER,
	DET_DEFAULT
} detector_t;

// local constants
static HOGDescriptor hog;

/* HOG parameters - Trackbar */
static parameter_t hitThreshold = {55, 100};			// Percentage
static parameter_t winStride = {8, 32};			// Number of pixels
static parameter_t padding = {8, 64};				// Number of pixels
static parameter_t scale = {8, 99};				// This has to be handled as the value after the decimal point, i.e: 5 -> 1.05
static parameter_t finalThreshold = {0, 100};			// Percentage
static parameter_t nmsThreshold = {0, 100};			// Percentage
static parameter_t nmsNeighbors = {0, 20};			// Count

/* HOG detector used */
detector_t detector = DET_DEFAULT;

//Method Implementations
/**
 * @brief : Constructor
 *
 * @param threadIndex 	: Thread Index
 * @param sysResource	: Global resource pointer
 * @param entry		: Entry function for the thread
 * @param arg		: Arguments to the thread
 */
CCameraDataProcessing::CCameraDataProcessing(int threadIndex, const CSystemResource *sysResource, CThread::start_routine_t entry, void *arg) : CThread(threadIndex, sysResource, entry, arg)
{
	//nothing
}

/**
 * @brief : Destructor
 */
CCameraDataProcessing::~CCameraDataProcessing()
{
	// nothing
}


/**
 * @brief : Main routine for the thread
 *
 * @return - to join the thread
 */
void CCameraDataProcessing::run()
{
	// The Threads runs here
	cout << "INFO\t: Thread " << this->getThreadIndex() << " started with ID : " << pthread_self() << endl;

	// Locals declaration:
	vector<Rect> detections;		// Vector of boxes where a detection was achieved
	vector<double> detection_weights;
	Mat image;
	vector<Rect> nmsDetections;
	ssize_t rBytes = 0;

#ifdef __SHOW_RESULT
	namedWindow("Detected Image", cv::WINDOW_AUTOSIZE);
	uint8_t counter = 0;
#endif

	double t_start = 0;

	while (1)
	{
		cout << "INFO\t: Running Thread " << this->getThreadIndex() << " started with ID : " << pthread_self() << endl;

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
#ifdef __SHOW_RESULT
			for (unsigned int i= 0; i < nmsDetections.size() ; i++)
			{
				rectangle(image, Point(nmsDetections[i].x, nmsDetections[i].y), Point(nmsDetections[i].x + nmsDetections[i].width, nmsDetections[i].y + nmsDetections[i].height), Scalar(0, 0, 255), 5, LINE_8);
			}

			cout << endl << "Time elapsed: " << (getTickCount() - t_start) / getTickFrequency() << endl;

			//			rectangle(image, Point(0, 0), Point(20, 20), Scalar(0, 0, 0), -1);
			//			putText(image, to_string((int) counter), Point(0,0), FONT_HERSHEY_PLAIN, 4,  Scalar(255,255,255), 2 , LINE_AA , false);

			counter = (counter + 1) % 100;
			imshow( "Detected Image", image );
			waitKey(1);
#endif
		}
	}
}


/**
 * @brief : Friend function used to create the thread 
 *
 * @param arg : arguments to the thread
 *
 * @return 
 */
void *friend_run_camera(void *arg)
{
	if (arg)
		cout << "INFO\t: Thread " << __func__ << endl;

	CCameraDataProcessing *ptr = static_cast<CCameraDataProcessing *>(arg);

	if (ptr)
	{
		ptr->run();
	}

	return NULL;
}
