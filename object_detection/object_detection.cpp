#include "object_detection.h"

#include <algorithm>
#include <iostream>  // TODO: remove

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
    std::cout << "format" << std::endl;
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

[[nodiscard]] std::tuple<std::vector<int>, std::vector<float>, std::vector<cv::Rect>> ObjectDetector::unwrap_yolo5_result(const cv::Mat &img, const std::vector<cv::Mat> &predictions, float score_threshold) const {
    std::cout << "unwrap_yolo5_result" << std::endl;
    const float x_factor = img.cols / YOLO_WIDTH_F;
    const float y_factor = img.rows / YOLO_HEIGHT_F;
    float *data = (float*) predictions[0].data;
    const int dimentions = 85;
    const int rows = 25200;
    
    std::vector<int> ids;
    std::vector<float> confidences;
    std::vector<cv::Rect> boxes;
    for (int i=0; i<rows; i++) {
        float confidence = data[4];
        if (confidence >= .4) {
            float *classes_scores = data+5;
            cv::Mat scores(1, ids.size(), CV_32FC1, classes_scores);
            cv::Point class_id;
            double max_class_score;
            minMaxLoc(scores, 0, &max_class_score, 0, &class_id);
            if (max_class_score > score_threshold) {
                confidences.push_back(confidence);
                ids.push_back(class_id.x);

                float x = data[0];
                float y = data[1];
                float w = data[2];
                float h = data[3];
                int left = int((x - 0.5 * w) * x_factor);
                int top = int((y - 0.5 * h) * y_factor);
                int width = int(w * x_factor);
                int height = int(h * y_factor);
                boxes.push_back(cv::Rect(left, top, width, height));
            }
        }

        data += dimentions;
    }

    return {ids, confidences, boxes};
}

[[nodiscard]] std::vector<int> ObjectDetector::filter_bounding_boxes(const std::vector<cv::Rect> &boxes, const std::vector<float> &confidences, float score_threshold, float nms_threshold) const {
    std::cout << "filter bounding boxes" << std::endl;
    std::vector<int> result;
    cv::dnn::NMSBoxes(boxes, confidences, score_threshold, nms_threshold, result);
    return result;
}

[[nodiscard]] std::vector<Detection> ObjectDetector::detect(const cv::Mat &frame) const {
    std::cout << "detect" << std::endl;
    auto square_blob = format_for_yolo5(frame);
    std::cout << "return format" << std::endl;

    std::vector<cv::Mat> predictions;
    auto net_copy = this->net;
    net_copy.setInput(square_blob);
    std::cout << "forwarding" << std::endl;
    net_copy.forward(predictions, net_copy.getUnconnectedOutLayersNames());
    std::cout << "forwarded" << std::endl;

    auto [class_ids, confidences, bounding_boxes] = unwrap_yolo5_result(frame, predictions, .25);

    auto filtered_boxes_idx = filter_bounding_boxes(bounding_boxes, confidences, .25, .45);

    std::vector<Detection> output;
    for (int i=0; i<filtered_boxes_idx.size(); i++) {
        int idx = filtered_boxes_idx[i];
        output.push_back({
            .class_id   = class_ids[idx],
            .confidence = confidences[idx],
            .box        = bounding_boxes[idx]
        });
    }
    return output;
}