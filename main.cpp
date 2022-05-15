#include <iostream>
#include <string>
#include <fmt/core.h>

#include <opencv2/opencv.hpp>

#include "fsm_manager/fsm_manager.h"
#include "fsm_manager/states.h"

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
    
    // TODO: get frames
    // TODO: pass frames through YOLO
    // TODO: match bounding boxes
    // TODO: update FSM
    // TODO: send MQTT message (with callback?)
    
    return 0;
}