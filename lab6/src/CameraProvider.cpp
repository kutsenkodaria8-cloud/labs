#include "CameraProvider.h"
#include <stdexcept>

CameraProvider::CameraProvider(int idx) {
    cap.open(idx);
    if (!cap.isOpened())
        throw std::runtime_error("Cannot open camera");}

CameraProvider::~CameraProvider() {
    cap.release();}

bool CameraProvider::isOpened() const {
    return cap.isOpened();}

cv::Mat CameraProvider::getFrame() {
    cv::Mat frame;
    cap >> frame;
    return frame;}