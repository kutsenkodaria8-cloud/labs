#include "KeyProcessor.h"
#include <opencv2/opencv.hpp>

KeyProcessor::KeyProcessor()
    : mode(Mode::NORMAL), brightness(1.0f), drawing(false), drawStart(0,0) {}

bool KeyProcessor::processKey(int key) {
    if (key == 27 || key == 'q') return false;

    switch (key) {
        case '0': mode = Mode::NORMAL; break;
        case '1': mode = Mode::INVERT; break;
        case '2': mode = Mode::BLUR; break;
        case '3': mode = Mode::CANNY; break;
        case '4': mode = Mode::SOBEL; break;
        case '5': mode = Mode::BINARIZE; break;
        case '6': mode = Mode::GLITCH; break;
        case '7': mode = Mode::ZOOM_IN; break;
        case '8': mode = Mode::ZOOM_OUT; break;
        case '9': mode = Mode::DRAW; break;
        case 'o': mode = Mode::OVERLAY; break;
        case 'i': mode = Mode::INFO; break;
        default: break;
    }
    return true;
}