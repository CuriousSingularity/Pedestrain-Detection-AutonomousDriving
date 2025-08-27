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

//Own Include Files
#include "./App/inc/CDetection.h"
#include "./Lib/inc/nms.h"
#include "./Lib/inc/CRingBuffer.h"
#include "./App/inc/CCameraService.h"
#include "./OS/inc/CMailBox.h"
#include "./App/inc/CSerialProtocol.h"
#include "./Common/inc/BuildConstants.h"

//Namespace
using namespace std;
using namespace global;
using namespace cv;
using namespace pedestrian_detection::build;

// 🚀 Modern configuration using BuildConstants
static const CDetection::hog_config_t hog_config_param {
		.hitThreshold	= static_cast<int>(DetectionConstants::HOGConfig::DEFAULT_HIT_THRESHOLD),
		.winStride		= DetectionConstants::HOGConfig::DEFAULT_WIN_STRIDE,
		.padding 		= DetectionConstants::HOGConfig::DEFAULT_PADDING,
		.scale 			= DetectionConstants::HOGConfig::DEFAULT_SCALE,
		.finalThreshold = static_cast<int>(DetectionConstants::HOGConfig::DEFAULT_FINAL_THRESHOLD),
		.nmsThreshold 	= static_cast<int>(DetectionConstants::HOGConfig::DEFAULT_NMS_THRESHOLD),
		.nmsNeighbors 	= DetectionConstants::HOGConfig::DEFAULT_NMS_NEIGHBORS,
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
CDetection::CDetection(int threadIndex) : CThread(threadIndex, [this]() { this->run(); })
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
	cout << "INFO\t: Detection Algorithm Service " << this->getThreadIndex() << " started with ID : " << pthread_self() << endl;

	// Hog detection
	Mat eachFrame, cannyMat, greyMat;
	HOGDescriptor hog;
	vector<Rect> detections;		// Vector of boxes where a detection was achieved
	vector<double> detection_weights;
	vector<Vec2f> lines; 			// will hold the results of the detection
	vector<uint8_t> nmsFiltered;

	// nms 
	vector<Rect> nmsDetections;

	// Variables for conditional compilation
	uint8_t counter = 0;
	double t_start = 0;

	if constexpr (BuildConfig::ENABLE_DISPLAY_CONNECTED) {
		namedWindow("Detected Image", cv::WINDOW_AUTOSIZE);
	}

	// select the algorithm from the configuration
	switch (hog_config_param.detectionModel)
	{
		case CDetection::HOG_DETECTION_DAIMLER:
			hog.winSize = Size(64, 128);
			hog.setSVMDetector(HOGDescriptor::getDefaultPeopleDetector());
			break;

		case CDetection::HOG_DETECTION_DEFAULT:
			hog.winSize = Size(48, 96);
			hog.setSVMDetector(HOGDescriptor::getDaimlerPeopleDetector());
			break;

		default:
			break;
	}

	int bigIndex = -1;
	extern CRingBuffer<cv::Mat, FRAMERATE> g_framesBuffer;
	extern CMailBox g__Mailboxes[THREAD_TOTAL_COUNT];

	CMailBox::mail_box_data_t dataToTx = 
	{
		.sid = global::SID_TX_DATA,
		.lid = CUart::UART_CHANNEL_1,
		.pDynamicData = 0,
	};

	CSerialProtocol::object_detection_frame_t *p_resultCollection = 0;

	cout << "INFO\t: Running Detection Algorithm Service " << this->getThreadIndex() << " started with ID : " << pthread_self() << endl;

	while (1)
	{
		// Detection Algorithm
		if (g_framesBuffer.readData(&eachFrame, CCameraService::cloneMat) == RC_SUCCESS)
		{
			if constexpr (BuildConfig::ENABLE_ALGO_TIME_MEASUREMENT) {
				// debugging the time
				t_start = getTickCount();
			}

			hog.detectMultiScale(
					eachFrame,							/* Source image */
					detections,							/* foundLocations, vector of Rect objects with the boxes where a person was detected */
					detection_weights,						/* Weights of each detection. Vector of same dimension as previous parameter */
					(float) hog_config_param.hitThreshold / 100,			/* hitThreshold: SVM threshold to filter final results */
					Size(hog_config_param.winStride, hog_config_param.winStride),	/* Windows stride: Horizontal and vertical step in pixels for the template matching process */
					Size(hog_config_param.padding, hog_config_param.padding),	/* Padding: PENDING */
					hog_config_param.scale,						/* Scale: Scale stride for the image pyramid */
					(float) hog_config_param.finalThreshold / 100			/* FinalThreshold: PENDING */
					);

			nms(detections, nmsDetections, (float) hog_config_param.nmsThreshold / 100, hog_config_param.nmsNeighbors);
			nmsFiltered.resize(nmsDetections.size(), 0);

			for (unsigned int i = 0 ; i < nmsDetections.size() ; i ++)
			{
				lines.clear();

				cvtColor(eachFrame(nmsDetections[i]), greyMat, COLOR_BGR2GRAY);
				Canny(greyMat, cannyMat, 50, 200, 3);
				HoughLines(cannyMat, lines, 1, CV_PI/180, 150, 0, 0 ); // runs the actual detection

				if (lines.size() > 4)
				{
					nmsFiltered[i] = 0;
				}
				else
				{
					nmsFiltered[i] = 1;
				}
			}

		p_resultCollection = new CSerialProtocol::object_detection_frame_t();

		if (p_resultCollection)
			this->filter_algorithm(nmsDetections, p_resultCollection, bigIndex, nmsFiltered);

		if constexpr (BuildConfig::ENABLE_ALGO_TIME_MEASUREMENT) {
			cout << endl << "Time elapsed: " << (getTickCount() - t_start) / getTickFrequency() << endl;
		}

		// release of data is done at the reception end
		dataToTx.pDynamicData = p_resultCollection;
		if (g__Mailboxes[THREAD_COM_TX_SERVICE].send(this->getThreadIndex(), dataToTx) != RC_SUCCESS)
		{
			cout << "ERROR\t: Failed to send the detected objects " << endl;
		}

		if constexpr (BuildConfig::ENABLE_DISPLAY_CONNECTED) {
			for (unsigned int i= 0; i < nmsDetections.size() ; i++)
			{
				rectangle(eachFrame, Point(nmsDetections[i].x, nmsDetections[i].y), Point(nmsDetections[i].x + nmsDetections[i].width, nmsDetections[i].y + nmsDetections[i].height), Scalar(0, 0, 255), 5, LINE_8);
				
				if (bigIndex >= 0)
					rectangle(eachFrame, Point(nmsDetections[bigIndex].x, nmsDetections[bigIndex].y), Point(nmsDetections[bigIndex].x + nmsDetections[bigIndex].width, nmsDetections[bigIndex].y + nmsDetections[bigIndex].height), Scalar(0, 255, 0), 5, LINE_8);
			}
			bigIndex = -1;

			//			rectangle(image, Point(0, 0), Point(20, 20), Scalar(0, 0, 0), -1);
			//			putText(image, to_string((int) counter), Point(0,0), FONT_HERSHEY_PLAIN, 4,  Scalar(255,255,255), 2 , LINE_AA , false);

			counter = (counter + 1) % 100;
			imshow( "Detected Image", eachFrame );
			waitKey(1);
		}

		} // End if (g_framesBuffer.readData(...) == RC_SUCCESS)
	} // End while (1)
}

void CDetection::filter_algorithm(vector<Rect> &nmsDetections, CSerialProtocol::object_detection_frame_t *p_resultCollection, int &bigIndex, const vector<uint8_t>& lineDetect)
{

#define THRESHOLD_AREA 		(0.1 * RESOLUTION_RESIZED_WIDTH * 0.26 * RESOLUTION_RESIZED_HEIGTH)
	uint32_t bigArea = THRESHOLD_AREA, area = 0;

	for (uint32_t index = 0; index < nmsDetections.size(); index++)
	{
		area = nmsDetections[index].width * nmsDetections[index].height;

		if (lineDetect[index] == 1)
		{
			if (area > bigArea)
			{
				bigIndex = index;
				bigArea = area;
			}
		}
	}

	if (bigArea >= THRESHOLD_AREA)
	{
		CSerialProtocol::object_detection_block_t blk;

		blk.theta = (ZERO_PIXEL_ANGLE + ANGELE_RESOLUTION * nmsDetections[bigIndex].x) * ANGLE_PRECISION_FACTOR;
		blk.delta_theta = (ANGELE_RESOLUTION * nmsDetections[bigIndex].width) * ANGLE_PRECISION_FACTOR;

		cout << "db:\t\t" << (int)blk.theta << "\t\t" << (int)blk.delta_theta << endl << endl;
		
		p_resultCollection->blks.push_back(blk);
	}
}


