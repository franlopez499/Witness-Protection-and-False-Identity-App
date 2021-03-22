#ifndef FACEDETECTORCASCADE_H
#define FACEDETECTORCASCADE_H
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/objdetect/objdetect.hpp>

#include <vector>

class FaceDetectorCascade {
public:

    static FaceDetectorCascade* getInstance();

    std::vector<cv::Rect> detect_face_rectangles(const cv::Mat &frame);
    std::vector<cv::Rect_<int>> detectEyes(const cv::Mat& frame);
    std::vector<cv::Rect> detectFaces(const cv::Mat& frame);
private:
    static FaceDetectorCascade* instance;
    ~FaceDetectorCascade();
    explicit FaceDetectorCascade();
    cv::CascadeClassifier face_cascade;
    cv::CascadeClassifier eye_cascade;


};


#endif // FACEDETECTORCASCADE_H
