#include "Display.h"

Display::Display(const std::string& win) : name(win) {}

void Display::show(const cv::Mat& frame) {
    cv::imshow(name, frame);
}

void Display::showHelp(cv::Mat& frame, const std::string& mode) {
    cv::putText(frame, "Mode: " + mode,
        {10, frame.rows-40}, cv::FONT_HERSHEY_SIMPLEX, 0.6, {255,200,0}, 2);
}