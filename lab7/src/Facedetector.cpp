#include "FaceDetector.h"

FaceDetector::FaceDetector(const std::string& proto, const std::string& model) {
    net = cv::dnn::readNetFromCaffe(proto, model);
    worker = std::thread(&FaceDetector::workerLoop, this);
}

FaceDetector::~FaceDetector() {
    running = false;
    worker.join();
}

void FaceDetector::setFrame(const cv::Mat& frame) {
    std::lock_guard<std::mutex> lock(mtx);
    inputFrame = frame.clone();
    hasNewFrame = true;
}

std::vector<cv::Rect> FaceDetector::getFaces(const cv::Size& frameSize) {
    std::lock_guard<std::mutex> lock(mtx);
    return faces;
}

void FaceDetector::workerLoop() {
    while (running) {
        cv::Mat frame;
        {
            std::lock_guard<std::mutex> lock(mtx);
            if (!hasNewFrame) continue;
            frame = inputFrame.clone();
            hasNewFrame = false;
        }

        cv::Mat blob = cv::dnn::blobFromImage(frame, 1.0, cv::Size(300, 300),
                                               cv::Scalar(104.0, 177.0, 123.0));
        net.setInput(blob);
        cv::Mat detections = net.forward();

        cv::Mat det = detections.reshape(1, detections.total() / 7);

        std::vector<cv::Rect> found;
        for (int i = 0; i < det.rows; i++) {
            float conf = det.at<float>(i, 2);
            if (conf < 0.5f) continue;

            int x1 = (int)(det.at<float>(i, 3) * frame.cols);
            int y1 = (int)(det.at<float>(i, 4) * frame.rows);
            int x2 = (int)(det.at<float>(i, 5) * frame.cols);
            int y2 = (int)(det.at<float>(i, 6) * frame.rows);

            x1 = std::max(0, x1); y1 = std::max(0, y1);
            x2 = std::min(frame.cols, x2); y2 = std::min(frame.rows, y2);
            found.push_back(cv::Rect(x1, y1, x2 - x1, y2 - y1));
        }

        {
            std::lock_guard<std::mutex> lock(mtx);
            faces = found;
            //std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
    }
}