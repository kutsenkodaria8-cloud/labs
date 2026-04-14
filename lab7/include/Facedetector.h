#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <thread>
#include <mutex>
#include <atomic>
#include <vector>

class FaceDetector {
public:
    FaceDetector(const std::string& proto, const std::string& model);
    ~FaceDetector();

    void setFrame(const cv::Mat& frame);
    std::vector<cv::Rect> getFaces(const cv::Size& frameSize);

private:
    void workerLoop();

    cv::dnn::Net net;
    std::thread worker;
    std::mutex mtx;
    std::atomic<bool> running{true};

    cv::Mat inputFrame;
    bool hasNewFrame = false;
    std::vector<cv::Rect> faces;
};