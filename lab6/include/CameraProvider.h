#pragma once
#include <opencv2/opencv.hpp>

class CameraProvider {
public:
    CameraProvider(int idx=0);
    ~CameraProvider();
    bool isOpened() const;
    cv::Mat getFrame();
    cv::VideoCapture cap;
};