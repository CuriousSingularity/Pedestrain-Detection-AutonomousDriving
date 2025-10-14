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
#include <iostream>
#include <vector>

// Own Include Files
#include "./App/inc/CCameraService.h"
#include "./App/inc/CDetection.h"
#include "./App/inc/CSerialProtocol.h"
#include "./Common/inc/BuildConstants.h"
#include "./Common/inc/Logger.h"
#include "./Lib/inc/CRingBuffer.h"
#include "./Lib/inc/nms.h"
#include "./OS/inc/CMailBox.h"

// Namespace
using namespace std;
using namespace global;
using namespace cv;
using namespace pedestrian_detection::build;

/**
 * @brief Default HOG detection configuration parameters
 * Uses BuildConstants for centralized parameter management
 */
static const CDetection::HogConfig hog_config_param{
    .hitThreshold = static_cast<int>(DetectionConstants::HOGConfig::DEFAULT_HIT_THRESHOLD),
    .winStride = DetectionConstants::HOGConfig::DEFAULT_WIN_STRIDE,
    .padding = DetectionConstants::HOGConfig::DEFAULT_PADDING,
    .scale = DetectionConstants::HOGConfig::DEFAULT_SCALE,
    .finalThreshold = static_cast<int>(DetectionConstants::HOGConfig::DEFAULT_FINAL_THRESHOLD),
    .nmsThreshold = static_cast<int>(DetectionConstants::HOGConfig::DEFAULT_NMS_THRESHOLD),
    .nmsNeighbors = DetectionConstants::HOGConfig::DEFAULT_NMS_NEIGHBORS,
    .detectionModel = CDetection::HOG_DETECTION_DEFAULT,
};

// Method Implementations
/**
 * @brief Constructor
 * Initializes detection thread with lambda-based execution
 * 
 * @param threadIndex Unique thread identifier for this detection service
 */
CDetection::CDetection(int threadIndex) : CThread(threadIndex, [this]() { this->run(); }) {
    // Constructor initialization handled by initializer list
}

/**
 * @brief : Destructor
 */
CDetection::~CDetection() {
    // nothing
}

#include <unistd.h>
/**
 * @brief : Main routine for the thread
 *
 * @return - to join the thread
 */
void CDetection::run() {
    // The Threads runs here
    LOG_INFO("CDetection", "Detection Algorithm Service " + std::to_string(this->getThreadIndex()) + " started with ID : " + std::to_string(pthread_self()));

    // Hog detection
    Mat eachFrame, cannyMat, greyMat;
    HOGDescriptor hog;
    vector<Rect> detections;  // Vector of boxes where a detection was achieved
    vector<double> detection_weights;
    vector<Vec2f> lines;  // will hold the results of the detection
    vector<uint8_t> nmsFiltered;

    // Non-Maximum Suppression results
    vector<Rect> nmsDetections;

    // Performance measurement variables (conditional compilation)
    uint8_t counter = 0;        // Frame counter for performance logging
    double t_start = 0;         // Timer start for algorithm measurement

    if constexpr (BuildConfig::ENABLE_DISPLAY_CONNECTED) {
        namedWindow("Detected Image", cv::WINDOW_AUTOSIZE);
    }

    // Configure HOG detector based on selected model
    switch (hog_config_param.detectionModel) {
    case CDetection::HOG_DETECTION_DAIMLER:
        hog.winSize = Size(64, 128);  // Daimler model window size
        hog.setSVMDetector(HOGDescriptor::getDefaultPeopleDetector());
        break;

    case CDetection::HOG_DETECTION_DEFAULT:
        hog.winSize = Size(48, 96);   // Default model window size
        hog.setSVMDetector(HOGDescriptor::getDaimlerPeopleDetector());
        break;

    default:
        break;
    }

    int largestDetectionIndex = -1;  // Index of largest valid detection
    extern CRingBuffer<cv::Mat, FRAMERATE> g_framesBuffer;
    extern CMailBox g__Mailboxes[THREAD_TOTAL_COUNT];

    CMailBox::MailBoxData dataToTx = {
        .sid = global::SID_TX_DATA,
        .lid = CUart::UART_CHANNEL_1,
        .dynamicData = 0,
    };

    CSerialProtocol::object_detection_frame_t* p_resultCollection = 0;

    LOG_INFO("CDetection", "Running Detection Algorithm Service " + std::to_string(this->getThreadIndex()) + " started with ID : " + std::to_string(pthread_self()));

    while (1) {
        // Detection Algorithm
        if (g_framesBuffer.readData(&eachFrame, CCameraService::cloneMat) == RC_SUCCESS) {
            if constexpr (BuildConfig::ENABLE_ALGO_TIME_MEASUREMENT) {
                // debugging the time
                t_start = getTickCount();
            }

            hog.detectMultiScale(
                eachFrame,  /* Source image */
                detections, /* foundLocations, vector of Rect objects with the boxes where a person
                               was detected */
                detection_weights, /* Weights of each detection. Vector of same dimension as
                                      previous parameter */
                (float)hog_config_param.hitThreshold /
                    100, /* hitThreshold: SVM threshold to filter final results */
                Size(hog_config_param.winStride,
                     hog_config_param.winStride), /* Windows stride: Horizontal and vertical step in
                                                     pixels for the template matching process */
                Size(hog_config_param.padding, hog_config_param.padding), /* Padding: PENDING */
                hog_config_param.scale, /* Scale: Scale stride for the image pyramid */
                (float)hog_config_param.finalThreshold / 100 /* FinalThreshold: PENDING */
            );

            // Apply Non-Maximum Suppression to remove overlapping detections
            nms(detections, nmsDetections, (float)hog_config_param.nmsThreshold / 100,
                hog_config_param.nmsNeighbors);
            // Initialize line detection filter results (0 = filtered out, 1 = passed)
            nmsFiltered.resize(nmsDetections.size(), 0);

            // Apply line detection filtering to reduce false positives
            for (unsigned int i = 0; i < nmsDetections.size(); i++) {
                lines.clear();

                // Extract detection region and convert to grayscale
                cvtColor(eachFrame(nmsDetections[i]), greyMat, COLOR_BGR2GRAY);
                // Apply Canny edge detection
                Canny(greyMat, cannyMat, 50, 200, 3);
                // Detect lines using Hough transform
                HoughLines(cannyMat, lines, 1, CV_PI / 180, 150, 0, 0);

                // Filter based on line count (too many lines = likely background)
                if (lines.size() > 4) {
                    nmsFiltered[i] = 0;  // Too many lines, likely false positive
                } else {
                    nmsFiltered[i] = 1;  // Valid detection candidate
                }
            }

            p_resultCollection = new CSerialProtocol::object_detection_frame_t();

            if (p_resultCollection)
                this->filterDetections(nmsDetections, p_resultCollection, largestDetectionIndex, nmsFiltered);

            if constexpr (BuildConfig::ENABLE_ALGO_TIME_MEASUREMENT) {
                LOG_DEBUG("CDetection", "Time elapsed: " + std::to_string((getTickCount() - t_start) / getTickFrequency()));
            }

            // release of data is done at the reception end
            dataToTx.dynamicData = p_resultCollection;
            if (g__Mailboxes[THREAD_COM_TX_SERVICE].send(this->getThreadIndex(), dataToTx) !=
                RC_SUCCESS) {
                LOG_ERROR("CDetection", "Failed to send the detected objects");
            }

            if constexpr (BuildConfig::ENABLE_DISPLAY_CONNECTED) {
                for (unsigned int i = 0; i < nmsDetections.size(); i++) {
                    rectangle(eachFrame, Point(nmsDetections[i].x, nmsDetections[i].y),
                              Point(nmsDetections[i].x + nmsDetections[i].width,
                                    nmsDetections[i].y + nmsDetections[i].height),
                              Scalar(0, 0, 255), 5, LINE_8);

                    // Highlight the largest valid detection in green
                    if (largestDetectionIndex >= 0)
                        rectangle(eachFrame,
                                  Point(nmsDetections[largestDetectionIndex].x, nmsDetections[largestDetectionIndex].y),
                                  Point(nmsDetections[largestDetectionIndex].x + nmsDetections[largestDetectionIndex].width,
                                        nmsDetections[largestDetectionIndex].y + nmsDetections[largestDetectionIndex].height),
                                  Scalar(0, 255, 0), 5, LINE_8);
                }
                largestDetectionIndex = -1;  // Reset for next frame

                //			rectangle(image, Point(0, 0), Point(20, 20), Scalar(0, 0, 0), -1);
                //			putText(image, to_string((int) counter), Point(0,0), FONT_HERSHEY_PLAIN,
                //4,  Scalar(255,255,255), 2 , LINE_AA , false);

                counter = (counter + 1) % 100;
                imshow("Detected Image", eachFrame);
                waitKey(1);
            }

        }  // End if (g_framesBuffer.readData(...) == RC_SUCCESS)
    }  // End while (1)
}

void CDetection::filterDetections(vector<Rect>& detections,
                                  CSerialProtocol::object_detection_frame_t* resultFrame,
                                  int& largestDetectionIndex, const vector<uint8_t>& lineDetections) {

#define THRESHOLD_AREA (0.1 * RESOLUTION_RESIZED_WIDTH * 0.26 * RESOLUTION_RESIZED_HEIGHT)
    uint32_t largestArea = THRESHOLD_AREA, currentArea = 0;

    for (uint32_t index = 0; index < detections.size(); index++) {
        currentArea = detections[index].width * detections[index].height;

        if (lineDetections[index] == 1) {
            if (currentArea > largestArea) {
                largestDetectionIndex = index;
                largestArea = currentArea;
            }
        }
    }

    if (largestArea >= THRESHOLD_AREA) {
        CSerialProtocol::object_detection_block_t detectionBlock;

        detectionBlock.theta = (ZERO_PIXEL_ANGLE + ANGLE_RESOLUTION * detections[largestDetectionIndex].x) *
                    ANGLE_PRECISION_FACTOR;
        detectionBlock.delta_theta =
            (ANGLE_RESOLUTION * detections[largestDetectionIndex].width) * ANGLE_PRECISION_FACTOR;

        LOG_DEBUG("CDetection", "Debug: theta = " + std::to_string((int)detectionBlock.theta) + " delta_theta = " + std::to_string((int)detectionBlock.delta_theta));

        resultFrame->blks.push_back(detectionBlock);
    }
}
