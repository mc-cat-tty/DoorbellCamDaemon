#pragma once

#include <vector>

#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>

namespace objdet {
    struct Detection {
        int class_id;  /**< class of the detected object */
        float confidence;  /**< detection confidence value */
        cv::Rect box;  /**< 2D bounding box that locates the object in the frame */
    };

    class ObjectDetector {
        private:
            cv::dnn::Net net;  /**< neural network */

            cv::Mat format_for_yolo5(const cv::Mat &source) const;
            // filter_bounding_boxes
            // unwrap_yolo5_result
        public:
            ObjectDetector(ObjectDetector &od) = default;

            /**
             * @brief Construct a new Object Detector object
             * 
             * @param n neural network to be used for detection
             */
            ObjectDetector(const cv::dnn::Net &n) : net(n) {};

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
            std::vector<Detection> detect(const cv::Mat &frame) const;
    };
}

#ifdef INLINE_ENABLED  // inline guard macro
#define INLINE inline
#include "object_detection.ipp"
#endif