#include "object_detection.h"

#include <algorithm>

using namespace objdet;

#ifndef INLINE_ENABLED
#define INLINE
#include "object_detection.ipp"
#endif

static const int YOLO_WIDTH = 640;
static const int YOLO_HEIGHT = 640;
static const float YOLO_WIDTH_F = 640.;
static const float YOLO_HEIGHT_F = 640.;

[[nodiscard]] cv::Mat ObjectDetector::format_for_yolo5(const cv::Mat &source) const {
    int rows = source.rows;
    int cols = source.cols;
    int max_dim = std::max(rows, cols);
    auto resized = cv::Mat::zeros(max_dim, max_dim, CV_8UC3);
    cv::Mat roi = resized(cv::Rect(0, 0, cols, rows));
    source.copyTo(roi);

    cv::Mat res;
    cv::dnn::blobFromImage(source, res, 1.0/255, cv::Size(YOLO_WIDTH, YOLO_HEIGHT), cv::Scalar(), true, false);
    return res;
}

[[nodiscard]] std::vector<Detection> ObjectDetector::detect(const cv::Mat &frame) const {
    auto square_frame = format_for_yolo5(frame);

    std::vector<cv::Mat> predictions;
    auto net_copy = this->net;
    net_copy.forward(predictions, net.getUnconnectedOutLayersNames());

    // unwrap_yolo5_result
    

    // filter_bounding_boxes
}