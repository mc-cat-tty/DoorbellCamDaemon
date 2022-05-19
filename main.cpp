#include <iostream>
#include <fstream>
#include <string>
#include <fmt/core.h>

#include <opencv2/opencv.hpp>
#include <unistd.h>

#include "fsm_manager/fsm_manager.h"
#include "fsm_manager/states.h"
#include "object_detection/object_detection.h"

#define EVER ;;

const int min_argc_num = 5;  /**< minimum number of arguments that must be passed to the program */
const char* YOLO_MODEL_PATH = "config_files/yolov5m6.onnx";  /**< path of yolov5m6.onnx file */
const char* CLASS_NAMES_PATH = "config_files/classes.txt";  /**< path of classes.txt file */

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

void callback(fsm::State s) {    
    switch (s) {
        case fsm::State::WAITING_PERSON:
            debug_print("WAITING_PERSON");
            break;
        case fsm::State::PERSON_DETECTED:
            debug_print("PERSON_DETECTED");
            break;
        case fsm::State::PERSON_STILL:
            debug_print("PERSON_STILL");
            break;
        default:
            debug_print("UNKNOWN");
    }
}

int main(int argc, const char* argv[]) {
    #ifndef TESTING
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

    const std::string mrl = fmt::format("rtsp://{}:{}@{}:{} !max-buffers=1 !drop=true", username, password, cam_ip, cam_port);
    debug_print(mrl.c_str());
    #else
    if (argc < 2) {
        std::cerr << "Wrong number of arguments" << std::endl;
        return 1;
    }
    const std::string mrl(argv[1]); 
    #endif

    std::vector<std::string> class_names;
    std::ifstream in(CLASS_NAMES_PATH);
    if (!in) {
        std::cerr << "Errore while opening class names file" << std::endl;
        return 3;
    }
    std::string line;
    while (getline(in, line)) {
        class_names.push_back(line);
    }
    debug_print("class_names loaded");

    cv::Mat image;
    auto net = cv::dnn::readNet(YOLO_MODEL_PATH);  // loading YOLOv5 model
    auto detector = objdet::ObjectDetector(net, class_names);
    auto fsm_manager = fsm::FsmManager(callback, 10);
    std::vector<objdet::Detection> det_res;
    for (EVER) {
        cv::VideoCapture camera(mrl);
        camera.set(cv::CAP_PROP_BUFFERSIZE, 1);
        if (!camera.open(mrl)) {
            std::cerr << "Error while opening network video stream" << std::endl;
            return 2;
        }
        debug_print("Video stream opened");

        if (!camera.read(image)) {
            std::cerr << "Error while reading frame" << std::endl;
            #ifdef DEBUG
            if (cv::waitKey(1) >= 0) break;
            #else
            sleep(1);
            #endif
            continue;
        }

        det_res = detector.detect(image);
        bool person_in_frame = false;
        for (const objdet::Detection &d : det_res) {
            if (class_names[d.class_id] == "person")
                person_in_frame = true;
        }
        fsm_manager.nextState(person_in_frame);
        
        #ifdef DEBUG
        for (const objdet::Detection &d : det_res) {
            cv::rectangle(image, d.box, cv::Scalar(255, 0, 0), 1, 8, 0);
            putText(image, class_names[d.class_id], cv::Point(d.box.x, d.box.y-10), cv::FONT_HERSHEY_COMPLEX_SMALL, 1.0, cv::Scalar(255, 0, 0), 1.2);
        }

        cv::imshow(mrl, image);
        if (cv::waitKey(1) >= 0) break;
        #else
        usleep(500*1000);
        #endif

        camera.release();
    }

    // TODO: update FSM
    // TODO: send MQTT message (with callback?)
    
    return 0;
}