using namespace objdet;

INLINE void ObjectDetector::setNet(const cv::dnn::Net &n) {
    this->net = n;
}