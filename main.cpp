#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <fmt/core.h>

#include <opencv2/opencv.hpp>
#include <MQTTClient.h>

#include "fsm_manager/fsm_manager.h"
#include "fsm_manager/states.h"
#include "object_detection/object_detection.h"
#include "node/node.h"

const int min_argc_num = 7;  /**< minimum number of arguments that must be passed to the program */
const char* YOLO_MODEL_PATH = "config_files/yolov5s.onnx";  /**< path of yolov5m6.onnx file */
const char* CLASS_NAMES_PATH = "config_files/classes.txt";  /**< path of classes.txt file */
const char* MQTT_CLIENTID = "DoorbellCamPub";
const int MQTT_QOS = 1;
const int MQTT_RETAIN = 1;
const long MQTT_TIMEOUT = 10000L;

typedef struct {
    MQTTClient mqtt_client;
    const char *mqtt_topic;
} callback_arg_t;

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

void callback(fsm::State s, const void* args) {
    const callback_arg_t *arguments = static_cast<const callback_arg_t*>(args);
    const MQTTClient mqtt_client = arguments->mqtt_client;
    const char *mqtt_topic = arguments->mqtt_topic;
    MQTTClient_message mqtt_msg = MQTTClient_message_initializer;
    mqtt_msg.retained = MQTT_RETAIN;
    mqtt_msg.qos = MQTT_QOS;
    MQTTClient_deliveryToken token;
    
    char payload[64];
    switch (s) {
        case fsm::State::WAITING_PERSON:
            strcpy(payload, "WAITING_PERSON");
            break;
        case fsm::State::PERSON_DETECTED:
            strcpy(payload, "PERSON_DETECTED");
            break;
        case fsm::State::PERSON_STILL:
            strcpy(payload, "PERSON_STILL");
            break;
        default:
            strcpy(payload, "UNKNOWN");
    }
    debug_print(payload);

    mqtt_msg.payload = payload;
    mqtt_msg.payloadlen = strlen(payload);
    if (MQTTClient_publishMessage(mqtt_client, mqtt_topic, &mqtt_msg, &token) != MQTTCLIENT_SUCCESS) {
        // std::cerr << "Error while publishing MQTT message" << std::endl;
        throw std::runtime_error("Error while publishing MQTT message");  // Keepalive time probably reached
        return;
    }

    if (MQTTClient_waitForCompletion(mqtt_client, token, MQTT_TIMEOUT) != MQTTCLIENT_SUCCESS) {
        std::cerr << "Error while waiting for completion of MQTT message" << std::endl;
    }
}

int main(int argc, const char* argv[]) {
    #ifndef TESTING
    if (argc < min_argc_num) {
        std::cerr << "Wrong arguments" << std::endl;
        std::cerr << "Usage: " << argv[0] << " CAM_IP CAM_PORT USERNAME PASSWORD MQTT_ADDR MQTT_TOPIC" << std::endl;
        return 1;
    }

    const char *cam_ip = argv[1];
    const char *cam_port = argv[2];
    const char *username = argv[3];
    const char *password = argv[4];
    const char *mqtt_addr = argv[5];
    const char *mqtt_topic = argv[6];

    debug_print(cam_ip);
    debug_print(cam_port);
    debug_print(username);
    debug_print(password);
    debug_print(mqtt_addr);
    debug_print(mqtt_topic);

    const std::string mrl = fmt::format("rtspsrc location=rtsp://{}:{}@{}:{}/h264 latency=0 ! queue ! rtph264depay ! h264parse ! avdec_h264 ! videoconvert ! appsink drop=1", username, password, cam_ip, cam_port);
    debug_print(mrl.c_str());
    #else  // open vcap from local sample file
    if (argc < 2) {
        std::cerr << "Wrong number of arguments" << std::endl;
        return 1;
    }
    const std::string mrl(argv[1]); 
    #endif

    std::vector<std::string> class_names;
    std::ifstream in(CLASS_NAMES_PATH);
    if (!in) {
        std::cerr << "Error while opening class names file" << std::endl;
        return 2;
    }
    std::string line;
    while (getline(in, line)) {
        class_names.push_back(line);
    }
    debug_print("class_names loaded");

    MQTTClient mqtt_client;
    MQTTClient_connectOptions mqtt_opts = MQTTClient_connectOptions_initializer;
    mqtt_opts.keepAliveInterval = 64800;  // 18 hours
    mqtt_opts.cleansession = 1;
    MQTTClient_create(&mqtt_client, mqtt_addr, MQTT_CLIENTID, MQTTCLIENT_PERSISTENCE_DEFAULT, NULL);
    if (MQTTClient_connect(mqtt_client, &mqtt_opts) != MQTTCLIENT_SUCCESS) {
        std::cerr << "Error while opening MQTT connection to " << mqtt_addr << std::endl;
        return 3;
    }
    debug_print("MQTTClient object created and initialized");

    auto net = cv::dnn::readNet(YOLO_MODEL_PATH);  // loading YOLOv5 model
    auto detector = objdet::ObjectDetector(net, class_names);
    callback_arg_t callback_arg = {
        .mqtt_client = mqtt_client,
        .mqtt_topic = mqtt_topic
    };
    auto fsm_manager = fsm::FsmManager(callback, (void*) &callback_arg, 5);
    std::set<std::string> triggering_classes({"person"});
    auto event_manager = Node::getInstance()
        .setMrl(mrl)
        .setDetector(detector)
        .setFsm(fsm_manager)
        .setTriggeringClasses(triggering_classes);
    
    try {
        event_manager.loop();
    }
    catch (std::exception &e) {
        MQTTClient_disconnect(mqtt_client, MQTT_TIMEOUT);
        MQTTClient_destroy(&mqtt_client);
        std::cerr << e.what() << std::endl;
        return 4;
    }
    
    return 0;
}
