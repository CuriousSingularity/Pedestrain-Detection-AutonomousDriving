/***************************************************************************
*============= Copyright by Darmstadt University of Applied Sciences =======
****************************************************************************
* Filename        : CCAMERADATAPROCESSING.CPP
* Author          :
* Description     :
*
*
****************************************************************************/

// System includes
#include <vector>
#include <opencv2/opencv.hpp>

// Own includes
#include "./App/inc/CCameraDataProcessing.h"
#include "./OS/inc/CSemaphore.h"
#include "./App/inc/nms.h"

using namespace std;
using namespace cv;

//Method Implementations

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
static parameter_t hitThreshold = {0, 100};			// Percentage
static parameter_t winStride = {8, 32};			// Number of pixels
static parameter_t padding = {8, 64};				// Number of pixels
static parameter_t scale = {8, 99};				// This has to be handled as the value after the decimal point, i.e: 5 -> 1.05
static parameter_t finalThreshold = {0, 100};			// Percentage
static parameter_t nmsThreshold = {0, 100};			// Percentage
static parameter_t nmsNeighbors = {0, 20};			// Count

/* HOG detector used */
detector_t detector = DET_DEFAULT;

CCameraDataProcessing::CCameraDataProcessing() : CThread(1, NULL, NULL)
{

}

CCameraDataProcessing::~CCameraDataProcessing()
{

}
void * CCameraDataProcessing::run(void * arg){
	vector<Rect> detections;		// Vector of boxes where a detection was achieved
	vector<double> detection_weights;
	Mat image;

	// TODO NOL: Acquire frame from camera

	if (detector == DET_DAIMLER){
		hog.winSize = Size(48, 96);
		hog.setSVMDetector(HOGDescriptor::getDaimlerPeopleDetector());
	}
	else{
		hog.winSize = Size(64, 128);
		hog.setSVMDetector(HOGDescriptor::getDefaultPeopleDetector());
	}

//	double t_start = getTickCount();
	hog.detectMultiScale(	image,									/* Source image */
							detections,					/* foundLocations, vector of Rect objects with the boxes where a person was detected */
							detection_weights,				/* Weights of each detection. Vector of same dimension as previous parameter */
							(float) hitThreshold.value / 100,		/* hitThreshold: SVM threshold to filter final results */
							Size(winStride.value, winStride.value),		/* Windows stride: Horizontal and vertical step in pixels for the template matching process */
							Size(padding.value, padding.value),		/* Padding: PENDING */
							1 + (float) scale.value / 100,			/* Scale: Scale stride for the image pyramid */
							(float) finalThreshold.value / 100		/* FinalThreshold: PENDING */
							);
//	cout << endl << "Time elapsed: " << (getTickCount() - t_start) / getTickFrequency() << endl;
//	cout << "Found " << detections.size() << " matches";


	vector<Rect> nmsDetections;
	nms(detections, nmsDetections, (float) nmsThreshold.value / 100, nmsNeighbors.value);

//	cout << " - After NMS: " << nmsDetections.size() << " matches" << endl;

	for (unsigned int i= 0; i < nmsDetections.size() ; i++)
	{
		rectangle(image, Point(nmsDetections[i].x, nmsDetections[i].y), Point(nmsDetections[i].x + nmsDetections[i].width, nmsDetections[i].y + nmsDetections[i].height), Scalar(0, 0, 255), 5, LINE_8);
	}

	namedWindow("Detected Image", cv::WINDOW_AUTOSIZE);
	imshow( "Detected Image", image );
	
}
typename CThread::start_routine_t CCameraDataProcessing::getPointerToStartRoutine(){}
