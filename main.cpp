#include <iostream>
#include <string>
#include <fmt/core.h>

#include <opencv2/opencv.hpp>

#include "fsm_manager/fsm_manager.h"
#include "fsm_manager/states.h"
#include "object_detection/object_detection.h"

#define EVER ;;

const int min_argc_num = 5;  /**< minimum number of arguments that must be passed to the program */

/**
 * @brief Print debug string if DEBUG macro is defined
 * 
 * @param str String to be printed
 */
void debug_print(const char *str) {
    #ifdef DEBUG
    std::cout << str << std::endl;
    #endif
}

int main(int argc, const char* argv[]) {
    if (argc < min_argc_num) {
        std::cerr << "Wrong arguments" << std::endl;
        std::cerr << "Usage: " << argv[0] << " CAM_IP CAM_PORT USERNAME PASSWORD" << std::endl;
        return 1;
    }

    const char *cam_ip = argv[1];
    const char *cam_port = argv[2];
    const char *username = argv[3];
    const char *password = argv[4];

    debug_print(cam_ip);
    debug_print(cam_port);
    debug_print(username);
    debug_print(password);

    const std::string mrl = fmt::format("rtsp://{}:{}@{}:{}/h.265", username, password, cam_ip, cam_port);
    debug_print(mrl.c_str());

    cv::VideoCapture camera(mrl);
    if (!camera.open(mrl)) {
        std::cerr << "Error while opening network video stream" << std::endl;
        return 2;
    }
    debug_print("Video stream opened");
    
    cv::Mat image;
    auto net = cv::dnn::readNet("yolov5s.onnx");  // loading YOLOv5 model
    auto detector = objdet::ObjectDetector(net);
    std::vector<objdet::Detection> det_res;
    for (EVER) {
        if (!camera.read(image)) {
            std::cerr << "Error while reading frame" << std::endl;
            #ifdef DEBUG
            cv::waitKey();
            #endif
            continue;
        }

        det_res = detector.detect(image);
        
        #ifdef DEBUG
        std::cout << det_res.size() << std::endl;
        for (const objdet::Detection &d : det_res) {
            cv::rectangle(image, d.box, cv::Scalar(255, 0, 0), 1, 8, 0);
        }

        cv::imshow(mrl, image);
        if (cv::waitKey(1) >= 0) break;
        #endif
    }

    // TODO: pass frames through YOLO
    // TODO: match bounding boxes
    // TODO: update FSM
    // TODO: send MQTT message (with callback?)
    
    return 0;
}