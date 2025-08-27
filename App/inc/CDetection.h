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

// System Include Files
#include <opencv2/opencv.hpp>

// Own Include Files
#include "./App/inc/CSerialProtocol.h"
#include "./OS/inc/CSemaphore.h"
#include "./OS/inc/CThread.h"

class CDetection : public CThread {
  private:
    /**
     * @brief : Main routine for the thread
     *
     * @return - to join the thread
     */
    void run();

    void filterDetections(std::vector<cv::Rect>& detections,
                          CSerialProtocol::object_detection_frame_t* resultFrame,
                          int& largestDetectionIndex, const std::vector<uint8_t>& lineDetections);

  public:
    typedef enum {
        HOG_DETECTION_DEFAULT = 0,
        HOG_DETECTION_DAIMLER,
    } HogDetectorType;

    typedef struct {
        uint8_t hitThreshold;           // Percentage : 0 - 100
        uint8_t winStride;              // Number of pixels
        uint8_t padding;                // Number of pixels
        float scale;                    // scaling factor : must be greater than 1.0
        uint8_t finalThreshold;         // Percentage
        uint8_t nmsThreshold;           // Percentage
        uint8_t nmsNeighbors;           // Count
        HogDetectorType detectionModel;  // Detection model type
    } HogConfig;

    /**
     * @brief : Constructor
     *
     * @param threadIndex 	: Thread Index
     */
    CDetection(int threadIndex);

    /**
     * @brief : Destructor
     */
    ~CDetection();
};
/********************
 **  CLASS END
 *********************/
#endif /* CDETECTION_H */
