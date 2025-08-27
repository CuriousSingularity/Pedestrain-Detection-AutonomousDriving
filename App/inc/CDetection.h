/***************************************************************************
 *============= Copyright by Darmstadt University of Applied Sciences =======
 ****************************************************************************
 * Filename        : CDetection.h
 * Author          : Nicolas Ojeda Leon (stnioied@stdu.h-da.de)
 * 			Bharath Ramachandraiah (stbhrama@stud.h-da.de)
 * Description     : Pedestrian detection service using HOG algorithm with filtering
 *
 ****************************************************************************/

#ifndef CDETECTION_H
#define CDETECTION_H

// System Include Files
#include <opencv2/opencv.hpp>

// Own Include Files
#include "./App/inc/CSerialProtocol.h"
#include "./OS/inc/CSemaphore.h"
#include "./OS/inc/CThread.h"

class CDetection : public CThread {
  private:
    /**
     * @brief Main detection thread execution routine
     * Continuously processes frames from camera buffer and performs
     * pedestrian detection using HOG algorithm with additional filtering
     */
    void run();

    /**
     * @brief Filter and process detection results
     * Applies area filtering and line detection to remove false positives,
     * selects the largest valid detection and calculates angular position
     * 
     * @param detections Vector of detected bounding rectangles
     * @param resultFrame Output frame structure to populate with results
     * @param largestDetectionIndex Reference to store index of largest detection
     * @param lineDetections Vector indicating which detections passed line filtering
     */
    void filterDetections(std::vector<cv::Rect>& detections,
                          CSerialProtocol::object_detection_frame_t* resultFrame,
                          int& largestDetectionIndex, const std::vector<uint8_t>& lineDetections);

  public:
    /**
     * @brief HOG detector model enumeration
     * Defines available pedestrian detection models
     */
    typedef enum {
        HOG_DETECTION_DEFAULT = 0,  ///< Standard OpenCV HOG detector
        HOG_DETECTION_DAIMLER,      ///< Daimler pedestrian detector model
    } HogDetectorType;

    /**
     * @brief HOG detection algorithm configuration
     * Contains all parameters for tuning detection performance
     */
    typedef struct {
        uint8_t hitThreshold;           ///< Detection confidence threshold (0-100%)
        uint8_t winStride;              ///< Sliding window step size in pixels
        uint8_t padding;                ///< Border padding in pixels
        float scale;                    ///< Image pyramid scale factor (>1.0)
        uint8_t finalThreshold;         ///< Final detection threshold (0-100%)
        uint8_t nmsThreshold;           ///< Non-maximum suppression threshold (0-100%)
        uint8_t nmsNeighbors;           ///< Minimum neighbors for NMS grouping
        HogDetectorType detectionModel; ///< HOG detector model to use
    } HogConfig;

    /**
     * @brief Constructor
     * Initializes detection service with specified thread index
     * @param threadIndex Unique identifier for this thread instance
     */
    CDetection(int threadIndex);

    /**
     * @brief Destructor
     * Cleans up detection service resources
     */
    ~CDetection();
};
/********************
 **  CLASS END
 *********************/
#endif /* CDETECTION_H */
