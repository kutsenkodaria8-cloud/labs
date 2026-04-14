#!/bin/bash
sudo apt update
sudo apt install -y libopencv-dev cmake gcc g++ build-essential wget

mkdir -p assets
wget -nc -P assets https://raw.githubusercontent.com/opencv/opencv/master/samples/dnn/face_detector/deploy.prototxt
wget -nc -P assets https://raw.githubusercontent.com/opencv/opencv_3rdparty/dnn_samples_face_detector_20170830/res10_300x300_ssd_iter_140000.caffemodel