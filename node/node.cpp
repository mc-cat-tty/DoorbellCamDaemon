#include "node.h"

#include <stdexcept>
#include <iostream>
#include <unistd.h>

#define EVER ;;
static const unsigned MAX_NOT_READ_FRAME = 10;

[[noreturn]] void Node::loop() {
    cv::Mat image;
    cv::Mat prev_image;
    std::vector<objdet::Detection> det_res;
    unsigned not_read_frames = 0;

    cv::VideoCapture camera(mrl);
    camera.set(cv::CAP_PROP_BUFFERSIZE, 1);
    if (!camera.open(mrl))
        throw std::runtime_error("Error while opening network video stream");

    #ifdef DEBUG
    std::cout << "Camera opened" << std::endl;
    #endif

    if (!camera.read(prev_image)) {
        std::cerr << "Error while reading prev frame" << std::endl;
        not_read_frames++;
    }

    for (EVER) {
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
        
        cv::Mat res;
        cv::GaussianBlur(image, image, cv::Size(51, 31), 0);
        cv::GaussianBlur(prev_image, prev_image, cv::Size(51, 31), 0);
        cv::absdiff(prev_image, image, res);
        cv::cvtColor(res, res, cv::COLOR_BGR2GRAY);
        cv::threshold(res, res, 10, 255, cv::THRESH_BINARY);
        cv::SimpleBlobDetector::Params params;
        params.filterByColor = true;
        params.blobColor = 255;
        params.filterByArea = true;
        params.minArea = 100;
        params.filterByConvexity = true;
        params.minConvexity = 0.1;
        params.maxConvexity = 100;
        params.minDistBetweenBlobs = 2;
        std::vector<cv::KeyPoint> keyPoints;
        cv::Ptr<cv::SimpleBlobDetector> blobDect = cv::SimpleBlobDetector::create(params); 
        blobDect->detect(res, keyPoints);
        cv::drawKeypoints(res, keyPoints, res, cv::Scalar(0, 255, 0), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

        cv::imshow(mrl, res);

        // det_res = detector.detect(image);
        // bool person_in_frame = false;
        // for (const objdet::Detection &d : det_res) {
        //     if (triggers.find(d.class_name) != triggers.end())
        //         person_in_frame = true;
        // }
        // fsm.nextState(person_in_frame);
        
        #ifdef DEBUG
        // for (const objdet::Detection &d : det_res) {
        //     cv::rectangle(image, d.box, cv::Scalar(255, 0, 0), 1, 8, 0);
        //     putText(image, d.class_name, cv::Point(d.box.x, d.box.y-10), cv::FONT_HERSHEY_COMPLEX_SMALL, 1.0, cv::Scalar(255, 0, 0), 1.2);
        // }

        // cv::imshow(mrl, image);
        if (cv::waitKey(1) >= 0) break;
        #endif

        usleep(500*1000);
        image.copyTo(prev_image);
    }

    camera.release();
}
