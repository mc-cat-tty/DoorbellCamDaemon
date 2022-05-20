#pragma once

#include <string>
#include <set>
#include <time.h>
#include <opencv2/opencv.hpp>

#include "../object_detection/object_detection.h"
#include "../fsm_manager/fsm_manager.h"


class Node {
    private:
        /**
         * @brief Private constructor of Node object
         * 
         */
        Node() {};

        objdet::ObjectDetector detector;  /**< Object that abstract detection from concrete implementation */
        fsm::FsmManager fsm;  /**< Finite State Machine */
        std::string mrl;  /**< Media Resource Link */
        std::set<std::string> triggers;  /**< Set of classes that trigger the doorbell */

    public:
        /**
         * @brief Returns a singleton of class Node
         * 
         * @return Node& is the same every time you call getInstance
         */
        [[nodiscard]] static Node& getInstance() {
            static Node instance;
            return instance;
        };

        /**
         * @brief Set the Detector object
         * 
         * @param object_detector new value of detector attribute
         * @return Node& returns the current instance (singleton) of Node
         */
        Node& setDetector(const objdet::ObjectDetector &object_detector) {
            this->detector = object_detector;
            return *this;
        };

        /**
         * @brief Set the Fsm object
         * 
         * @param fsm_manager new value for fsm attribute
         * @return Node& returns the current instance (singleton) of Node
         */
        Node& setFsm(const fsm::FsmManager &fsm_manager) {
            this->fsm = fsm_manager;
            return *this;
        }

        /**
         * @brief Set the Mrl object
         * 
         * @param stream_mrl new value of mrl attribute
         * @return Node& returns the current instance (singleton) of Node
         */
        Node& setMrl(const std::string &stream_mrl) {
            this->mrl = stream_mrl;
            return *this;
        }

        /**
         * @brief Set the Triggering Classes object
         * 
         * @param triggering_classes new value of triggers attributes
         * @return Node& returns the current instance (singleton) of Node
         */
        Node& setTriggeringClasses(const std::set<std::string> &triggering_classes) {
            this->triggers = triggering_classes;
            return *this;
        }

        /**
         * @brief event loop that spins forever (actually, it spins until an exception occurs)
         * 
         */
        [[noreturn]] void loop();
};