#include <opencv2/opencv.hpp>
#include "CameraProvider.h"
#include "KeyProcessor.h"
#include "FrameProcessor.h"
#include "Display.h"
#include "FaceDetector.h"
#include <map>
#include <filesystem>

std::map<Mode, std::string> modeNames = {
    {Mode::NORMAL,   "Normal"},   {Mode::INVERT,   "Invert"},
    {Mode::BLUR,     "Blur"},     {Mode::CANNY,    "Canny"},
    {Mode::SOBEL,    "Sobel"},    {Mode::BINARIZE, "Binarize"},
    {Mode::GLITCH,   "Glitch"},   {Mode::ZOOM_IN,  "Zoom In"},
    {Mode::ZOOM_OUT, "Zoom Out"}, {Mode::DRAW,     "Draw"},
    {Mode::OVERLAY,  "Overlay"},  {Mode::INFO,     "Info"},
    {Mode::FACE,     "Face"},
};

bool pressing = false;
cv::Point pt1(-1, -1), pt2(-1, -1), cross(-1, -1);
std::vector<cv::Rect> rects;
int wheelZoom = 100;
int brightnessBar = 50;
cv::Point overlayPos(0, 0);

void onTrackbar(int, void*) {}

void onMouse(int event, int x, int y, int flags, void*) {
    cross = {x, y};
    if (event == cv::EVENT_LBUTTONDOWN) {
        pressing = true;
        pt1 = pt2 = {x, y};
    } else if (event == cv::EVENT_MOUSEMOVE && pressing) {
        pt2 = {x, y};
    } else if (event == cv::EVENT_LBUTTONUP) {
        pressing = false;
        if (pt1.x >= 0)
            rects.push_back(cv::Rect(pt1, pt2));
        pt1 = pt2 = {-1, -1};
    } else if (event == cv::EVENT_MOUSEWHEEL) {
        int delta = cv::getMouseWheelDelta(flags);
        wheelZoom += delta > 0 ? 10 : -10;
        wheelZoom = std::max(50, std::min(200, wheelZoom));
    }
}

int main(int argc, char* argv[]) {
    std::string assetsDir = "./assets";
    if (!std::filesystem::exists(assetsDir)) {
        std::string exeDir = std::filesystem::path(argv[0]).parent_path().string();
        assetsDir = exeDir + "/assets";
        if (!std::filesystem::exists(assetsDir))
            assetsDir = exeDir + "/../assets";
    }

    std::string protoPath   = assetsDir + "/deploy.prototxt";
    std::string modelPath   = assetsDir + "/res10_300x300_ssd_iter_140000.caffemodel";
    std::string overlayPath = assetsDir + "/overlay.png";

    CameraProvider cam(0);
    if (!cam.isOpened()) {
        printf("no camera\n");
        return -1;
    }

    std::string win = "Lab7 - OpenCV";
    cv::namedWindow(win, cv::WINDOW_AUTOSIZE);
    cv::createTrackbar("Brightness", win, nullptr, 100, onTrackbar);
    cv::setTrackbarPos("Brightness", win, brightnessBar);

    KeyProcessor kp;
    FrameProcessor fp;
    Display disp(win);

    FaceDetector* fd = nullptr;
    if (std::filesystem::exists(protoPath) && std::filesystem::exists(modelPath)) {
        fd = new FaceDetector(protoPath, modelPath);
    } else {
        printf("WARNING: Face model files not found in: %s\n", assetsDir.c_str());
    }

    fp.loadOverlay(overlayPath);
    cv::setMouseCallback(win, onMouse, NULL);

    Mode prevMode = Mode::NORMAL;

    while (true) {
        cv::Mat frame = cam.getFrame();
        if (frame.empty()) continue;

        if (kp.getMode() != prevMode) {
            rects.clear();
            prevMode = kp.getMode();
        }

        brightnessBar = cv::getTrackbarPos("Brightness", win);
        float bright = 0.1f + (brightnessBar / 100.0f) * 2.9f;

        int key = cv::waitKey(30);
        if (kp.getMode() == Mode::OVERLAY) {
            if (key == 'w') overlayPos.y -= 5;
            if (key == 's') overlayPos.y += 5;
            if (key == 'a') overlayPos.x -= 5;
            if (key == 'd') overlayPos.x += 5;
        } else {
            if (key == 'w') wheelZoom = std::min(200, wheelZoom + 5);
            if (key == 's') wheelZoom = std::max(50,  wheelZoom - 5);
        }
        float zoom = wheelZoom / 100.0f;

        if (kp.getMode() == Mode::FACE && fd)
            fd->setFrame(frame);

        std::vector<cv::Rect> allRects;
        if (kp.getMode() == Mode::DRAW) {
            allRects = rects;
            if (pressing && pt1.x >= 0)
                allRects.push_back(cv::Rect(pt1, pt2));
        }

        cv::Mat result = fp.process(frame, kp.getMode(), bright, allRects, cross, zoom, overlayPos);

        if (kp.getMode() == Mode::FACE) {
            if (fd) {
                for (const auto& r : fd->getFaces(frame.size()))
                    cv::rectangle(result, r, {0, 255, 0}, 2);
            } else {
                cv::putText(result, "Model files not found!", {10, 60},
                    cv::FONT_HERSHEY_SIMPLEX, 0.7, {0, 0, 255}, 2);
            }
        }

        disp.showHelp(result, modeNames[kp.getMode()]);
        disp.show(result);

        if (key == 'q' || key == 27) break;
        if (key == 'c') rects.clear();
        kp.processKey(key);

        if (cv::getWindowProperty(win, cv::WND_PROP_VISIBLE) < 1) break;
    }

    delete fd;
    cv::destroyAllWindows();
    return 0;
}
