#pragma once

#include <vector>
#include <tuple>

#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>

namespace objdet {
    struct Detection {
        int class_id;  /**< class of the detected object */
        std::string class_name;  /**< class' name of the detected object */
        float confidence;  /**< detection confidence value */
        cv::Rect box;  /**< 2D bounding box that locates the object in the frame */
    };

    class ObjectDetector {
        private:
            cv::dnn::Net net;  /**< neural network */
            std::vector<std::string> class_names;  /**< vector of class names */

            /**
             * @brief Format source frame for YOLOv5 network
             * 
             * @param source image
             * @return cv::Mat square blob ready to be processed by YOLO
             */
            [[nodiscard]] cv::Mat format_for_yolo5(const cv::Mat &source) const;

            /**
             * @brief Unwrap YOLOv5 result into three vectors
             * 
             * @param img input frame
             * @param predictions input predictions vector
             * @param classes vector of class names
             * @param confidence_threshold input confidence threshold
             * @param score_threshold intput score threshold
             * @return std::tuple<std::vector<int>, std::vector<float>, std::vector<cv::Rect>> 
             */
            [[nodiscard]] std::tuple<std::vector<int>, std::vector<float>, std::vector<cv::Rect>> unwrap_yolo5_result(const cv::Mat &img, const std::vector<cv::Mat> &predictions, const std::vector<std::string> &classes, float confidence_theshold, float score_threshold) const;

            /**
             * @brief Filter bounding boxes that come from the YOLO net and keep non overlapping ones
             * 
             * @param boxes input boxes
             * @param confidences input confidence vector
             * @param score_threshold input confidence threshold
             * @param nms_threshold input nms threshold
             * @return std::vector<int> output indexes that are not overlapping
             */
            [[nodiscard]] std::vector<int> filter_bounding_boxes(const std::vector<cv::Rect> &boxes, const std::vector<float> &confidences, float score_threshold, float nms_threshold) const ;
        public:
            ObjectDetector()                    = default;
            ObjectDetector(ObjectDetector &od)  = default;

            /**
             * @brief Construct a new Object Detector object
             * 
             * @param n neural network to be used for detection
             */
            ObjectDetector(const cv::dnn::Net &n, const std::vector<std::string> &classes) : net(n), class_names(classes) {};

            /**
             * @brief Set net attribute to n
             * 
             * @param n new value of net
             */
            void setNet(const cv::dnn::Net &n);

            /**
             * @brief Detects objects inside frame and returns them
             * 
             * @param frame input frame
             * @return std::vector<Detection> vector of bounding boxes with their respective confidence and class
             */
            [[nodiscard]] std::vector<Detection> detect(const cv::Mat &frame) const;
    };
}

#ifdef INLINE_ENABLED  // inline guard macro
#define INLINE inline
#include "object_detection.ipp"
#endif