#include "node.h"

#include <stdexcept>
#include <iostream>
#include <unistd.h>
#include <cmath>

#define EVER ;;
static const unsigned MAX_NOT_READ_FRAME = 10;

[[noreturn]] void Node::loop() {
    cv::Mat image;
    cv::Mat prev_image;
    std::vector<objdet::Detection> det_res;
    unsigned not_read_frames = 0;
    bool yolo_detection = false;
    uint32_t prev_ratio_wb = 0;

    cv::VideoCapture camera(mrl);
    camera.set(cv::CAP_PROP_BUFFERSIZE, 1);

    if (!camera.open(mrl))
        throw std::runtime_error("Error while opening network video stream");

    if (!camera.read(prev_image)) {
        std::cerr << "Error while reading prev frame" << std::endl;
        not_read_frames++;
    }

    for (EVER) {
        if (!camera.open(mrl))
            throw std::runtime_error("Error while opening network video stream");

        #ifdef DEBUG
        std::cout << "Camera opened" << std::endl;
        #endif

        if (!camera.read(image)) {
            std::cerr << "Error while reading frame" << std::endl;
            not_read_frames++;

            #ifdef DEBUG
            if (cv::waitKey(1) >= 0) break;
            #else
            sleep(1);
            #endif

            continue;
        }

        if (image.empty()) {
            std::cerr << "Empty frame" << std::endl;
            not_read_frames++;
        }

        if (not_read_frames >= MAX_NOT_READ_FRAME)
            throw std::runtime_error("Camera doesn't return frames");
        
        bool motion_detection_triggered = false;
        cv::Mat res;
        cv::Mat image_copy;
        image.copyTo(image_copy);
        cv::GaussianBlur(image_copy, image_copy, cv::Size(51, 31), 0);
        cv::GaussianBlur(prev_image, prev_image, cv::Size(51, 31), 0);
        cv::absdiff(prev_image, image_copy, res);
        cv::cvtColor(res, res, cv::COLOR_BGR2GRAY);
        cv::threshold(res, res, 32, 255, cv::THRESH_BINARY);
        const uint32_t foreground_count = cv::countNonZero(res);
        const uint32_t background_count = res.size[0]*res.size[1] - foreground_count;
        const float ratio_wb = static_cast<float>(foreground_count) / background_count;
        
        if (std::abs(ratio_wb-prev_ratio_wb) > 0.001) {
            motion_detection_triggered = true;
            yolo_detection = yolo_detection ? false : true;

            #ifdef DEBUG
            std::cout << "Transitioned to detection=" << yolo_detection << std::endl;
            #endif
        }
        prev_ratio_wb = ratio_wb;

        bool person_in_frame = false;
        if (yolo_detection) {
            det_res = detector.detect(image);
            for (const objdet::Detection &d : det_res) {
                if (triggers.find(d.class_name) != triggers.end())
                    person_in_frame = true;
            }
        }
        fsm.nextState(person_in_frame);
        
        #ifdef DEBUG
        if (yolo_detection) {
            for (const objdet::Detection &d : det_res) {
                cv::rectangle(image, d.box, cv::Scalar(255, 0, 0), 1, 8, 0);
                putText(image, d.class_name, cv::Point(d.box.x, d.box.y-10), cv::FONT_HERSHEY_COMPLEX_SMALL, 1.0, cv::Scalar(255, 0, 0), 1.2);
            }
        }

        cv::imshow(mrl, image);
        if (cv::waitKey(1) >= 0) break;
        #endif

        image.copyTo(prev_image);
        camera.release();
    }

}
