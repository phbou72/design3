#ifndef __KinectCalibrator__
#define __KinectCalibrator__

#include <iostream>

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"

#include "ObstaclesDetector.h"
#include "KinectTransformator.h"

class KinectCalibrator {
private:
    const static cv::Vec2f BASE_POSITION_FROM_ORIGIN;
    const static cv::Point LINE_1_1;
    const static cv::Point LINE_1_2;
    const static cv::Point LINE_2_1;
    const static cv::Point LINE_2_2;

    std::vector<cv::Point> _pointVector;

    std::vector<cv::Point> findCalibrationSquare(cv::Mat rgbMatrix);

    float findAndSetKinectAngle(cv::Mat depthMatrix);

    cv::Vec2f errorCorrect();

    int tableNumber;

public:
    bool calibrate(cv::Mat rgbMatrix, cv::Mat depthMatrix);

    KinectCalibrator();

    bool manualCalibration();

    void setTable(int);
};

#endif
