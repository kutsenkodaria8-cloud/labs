#pragma once
#include <opencv2/opencv.hpp>
#include "KeyProcessor.h"
#include <chrono>

class FrameProcessor {
public:
    FrameProcessor();
    void loadOverlay(const std::string& path);
    cv::Mat process(const cv::Mat& frame, Mode mode, float brightness,const std::vector<cv::Rect>& rects, cv::Point crosshair,
                    float zoom, cv::Point overlayPos);
    cv::Mat overlayImg;
    int frameCount;
    double fps;
    std::chrono::steady_clock::time_point lastTime;

private:
    cv::Mat applyInvert(const cv::Mat& f);
    cv::Mat applyBlur(const cv::Mat& f);
    cv::Mat applyCanny(const cv::Mat& f);
    cv::Mat applySobel(const cv::Mat& f);
    cv::Mat applyBinarize(const cv::Mat& f);
    cv::Mat applyGlitch(const cv::Mat& f);
    cv::Mat applyZoom(const cv::Mat& f, float factor);
    cv::Mat applyOverlay(const cv::Mat& f, cv::Point pos);
    cv::Mat applyInfo(const cv::Mat& f);
    void drawRects(cv::Mat& f, const std::vector<cv::Rect>& rects);
    void drawCrosshair(cv::Mat& f, cv::Point p);
};