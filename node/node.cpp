#include "node.h"

#include <stdexcept>
#include <iostream>
#include <unistd.h>

#define EVER ;;
static const unsigned MAX_NOT_READ_FRAME = 10;

[[noreturn]] void Node::loop() {
    cv::Mat image;
    std::vector<objdet::Detection> det_res;
    unsigned not_read_frames = 0;

    for (EVER) {
        cv::VideoCapture camera(mrl);
        camera.set(cv::CAP_PROP_BUFFERSIZE, 1);
        if (!camera.open(mrl))
            throw std::runtime_error("Error while opening network video stream");
        
        #ifdef DEBUG
        std::cout << "Camera opened" << std::endl;
        #endif

        if (!camera.read(image)) {
            std::cerr << "Error while reading frame" << std::endl;
            not_read_frames++;
            if (not_read_frames >= MAX_NOT_READ_FRAME)
                throw std::runtime_error("Camera doesn't return frames");

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
            if (triggers.find(d.class_name) != triggers.end())
                person_in_frame = true;
        }
        fsm.nextState(person_in_frame);
        
        #ifdef DEBUG
        for (const objdet::Detection &d : det_res) {
            cv::rectangle(image, d.box, cv::Scalar(255, 0, 0), 1, 8, 0);
            putText(image, d.class_name, cv::Point(d.box.x, d.box.y-10), cv::FONT_HERSHEY_COMPLEX_SMALL, 1.0, cv::Scalar(255, 0, 0), 1.2);
        }

        cv::imshow(mrl, image);
        if (cv::waitKey(1) >= 0) break;
        #else
        usleep(500*1000);  // 0.5 sec
        #endif

        camera.release();
    }
}