#include "FrameProcessor.h"

FrameProcessor::FrameProcessor() : frameCount(0), fps(0.0) {
    lastTime = std::chrono::steady_clock::now();
}

void FrameProcessor::loadOverlay(const std::string& path) {
    overlayImg = cv::imread(path, cv::IMREAD_UNCHANGED);}

cv::Mat FrameProcessor::process(const cv::Mat& frame, Mode mode, float brightness,
    const std::vector<cv::Rect>& rects, cv::Point crosshair,
    float zoom, cv::Point overlayPos)
{
    frameCount++;
    auto now = std::chrono::steady_clock::now();
    double elapsed = std::chrono::duration<double>(now - lastTime).count();
    if (elapsed >= 0.5) {
        fps = frameCount / elapsed;
        frameCount = 0;
        lastTime = now;
    }

    cv::Mat zoomed = applyZoom(frame, zoom);

    cv::Mat result;
    switch (mode) {
    case Mode::INVERT:   result = applyInvert(zoomed); break;
    case Mode::BLUR:     result = applyBlur(zoomed); break;
    case Mode::CANNY:    result = applyCanny(zoomed); break;
    case Mode::SOBEL:    result = applySobel(zoomed); break;
    case Mode::BINARIZE: result = applyBinarize(zoomed); break;
    case Mode::GLITCH:   result = applyGlitch(zoomed); break;
    case Mode::ZOOM_IN:  result = applyZoom(zoomed, 1.5f); break;
    case Mode::ZOOM_OUT: result = applyZoom(zoomed, 0.6f); break;
    case Mode::OVERLAY:  result = applyOverlay(zoomed, overlayPos); break;
    case Mode::INFO:     result = applyInfo(zoomed); break;
    default:             result = zoomed.clone(); break;
    }

    result.convertTo(result, -1, brightness, 0);
    drawRects(result, rects);
    if (crosshair.x >= 0)
        drawCrosshair(result, crosshair);

    cv::putText(result, "FPS: " + std::to_string((int)fps),
        {10, result.rows - 10}, cv::FONT_HERSHEY_SIMPLEX, 0.6, {0,255,0}, 2);

    return result;
}

cv::Mat FrameProcessor::applyInvert(const cv::Mat& f) {
    cv::Mat out;
    cv::bitwise_not(f, out);
    return out;
}

cv::Mat FrameProcessor::applyBlur(const cv::Mat& f) {
    cv::Mat out;
    cv::GaussianBlur(f, out, {15,15}, 0);
    return out;
}

cv::Mat FrameProcessor::applyCanny(const cv::Mat& f) {
    cv::Mat gray, edges, out;
    cv::cvtColor(f, gray, cv::COLOR_BGR2GRAY);
    cv::Canny(gray, edges, 80, 200);
    cv::cvtColor(edges, out, cv::COLOR_GRAY2BGR);
    return out;
}

cv::Mat FrameProcessor::applySobel(const cv::Mat& f) {
    cv::Mat gray, gx, gy, absGx, absGy, out;
    cv::cvtColor(f, gray, cv::COLOR_BGR2GRAY);
    cv::Sobel(gray, gx, CV_16S, 1, 0);
    cv::Sobel(gray, gy, CV_16S, 0, 1);
    cv::convertScaleAbs(gx, absGx);
    cv::convertScaleAbs(gy, absGy);
    cv::addWeighted(absGx, 0.5, absGy, 0.5, 0, out);
    cv::cvtColor(out, out, cv::COLOR_GRAY2BGR);
    return out;
}

cv::Mat FrameProcessor::applyBinarize(const cv::Mat& f) {
    cv::Mat gray, thr, out;
    cv::cvtColor(f, gray, cv::COLOR_BGR2GRAY);
    cv::threshold(gray, thr, 128, 255, cv::THRESH_BINARY);
    cv::cvtColor(thr, out, cv::COLOR_GRAY2BGR);
    return out;
}

cv::Mat FrameProcessor::applyGlitch(const cv::Mat& f) {
    cv::Mat out = f.clone();
    std::vector<cv::Mat> ch;
    cv::split(out, ch);
    int shift = 10;
    cv::Mat tmp = cv::Mat::zeros(f.size(), CV_8UC1);
    if (shift < f.cols) {
        cv::Rect src(shift, 0, f.cols - shift, f.rows);
        cv::Rect dst(0, 0, f.cols - shift, f.rows);
        ch[2](src).copyTo(tmp(dst));
    }
    ch[2] = tmp;
    cv::merge(ch, out);
    return out;
}

cv::Mat FrameProcessor::applyZoom(const cv::Mat& f, float factor) {
    if (factor == 1.0f) return f.clone();
    cv::Mat out;
    int w = std::min((int)(f.cols / factor), f.cols);
    int h = std::min((int)(f.rows / factor), f.rows);
    int x = std::max(0, (f.cols - w) / 2);
    int y = std::max(0, (f.rows - h) / 2);
    cv::resize(f(cv::Rect(x, y, w, h)), out, f.size());
    return out;
}

cv::Mat FrameProcessor::applyOverlay(const cv::Mat& f, cv::Point pos) {
    cv::Mat out = f.clone();
    if (overlayImg.empty()) return out;
    cv::Mat resized;
    cv::resize(overlayImg, resized, {out.cols/3, out.rows/3});
    int c = resized.channels();
    for (int i = 0; i < resized.rows; i++) {
        for (int j = 0; j < resized.cols; j++) {
            int dx = j + pos.x;
            int dy = i + pos.y;
            if (dx < 0 || dy < 0 || dx >= out.cols || dy >= out.rows) continue;
            if (c == 4) {
                cv::Vec4b px = resized.at<cv::Vec4b>(i, j);
                if (px[3] > 0)
                    out.at<cv::Vec3b>(dy, dx) = {px[0], px[1], px[2]};
            } else {
                out.at<cv::Vec3b>(dy, dx) = resized.at<cv::Vec3b>(i, j);
            }
        }
    }
    return out;
}

cv::Mat FrameProcessor::applyInfo(const cv::Mat& f) {
    cv::Mat out = f.clone();
    cv::Scalar mean = cv::mean(out);
    std::string txt = "Avg B:" + std::to_string((int)mean[0])
        + " G:" + std::to_string((int)mean[1])
        + " R:" + std::to_string((int)mean[2]);
    cv::putText(out, txt, {10, 30}, cv::FONT_HERSHEY_SIMPLEX, 0.6, {0,200,255}, 2);
    cv::putText(out, "Frames: " + std::to_string(frameCount),
        {10, 60}, cv::FONT_HERSHEY_SIMPLEX, 0.6, {0,200,255}, 2);
    return out;
}

void FrameProcessor::drawRects(cv::Mat& f, const std::vector<cv::Rect>& rects) {
    for (auto& r : rects)
        cv::rectangle(f, r, {0,255,0}, 2);
}

void FrameProcessor::drawCrosshair(cv::Mat& f, cv::Point p) {
    int sz = 20;
    cv::line(f, {p.x-sz, p.y}, {p.x+sz, p.y}, {0,0,255}, 2);
    cv::line(f, {p.x, p.y-sz}, {p.x, p.y+sz}, {0,0,255}, 2);
}