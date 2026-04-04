#pragma once
#include <opencv2/core/types.hpp>

enum class Mode {
    NORMAL = 0, INVERT, BLUR, CANNY,SOBEL, BINARIZE, GLITCH,ZOOM_IN, ZOOM_OUT, DRAW, OVERLAY, INFO, COUNT
};

class KeyProcessor {
public:
    KeyProcessor();
    bool processKey(int key);
    Mode getMode() const {return mode;}
    float getBrightness() const {return brightness;}
    bool isDrawing() const {return drawing;}
    void setDrawing(bool v) {drawing = v;}
    cv::Point getDrawStart() const {return drawStart;}
    void setDrawStart(cv::Point p) {drawStart = p;}

    Mode mode;
    float brightness;
    bool drawing;
    cv::Point drawStart;
};