#pragma once
#include <opencv2/opencv.hpp>
#include <string>

class Display {
public:
    Display(const std::string& win);
    void show(const cv::Mat& frame);
    void showHelp(cv::Mat& frame, const std::string& mode);
    std::string name;
};