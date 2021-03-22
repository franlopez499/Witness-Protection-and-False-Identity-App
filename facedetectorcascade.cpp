#include "facedetectorcascade.h"


#include <sstream>
#include <vector>
#include <string>
#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;

FaceDetectorCascade* FaceDetectorCascade::instance = nullptr;

FaceDetectorCascade::FaceDetectorCascade()
{
    face_cascade.load(PROJECT_PATH "haarcascade_frontalface_alt2.xml");
    eye_cascade.load(PROJECT_PATH "haarcascade_eye.xml");
    if (face_cascade.empty()) {
        ostringstream ss;
        ss << "Failed to load haarcascade for face detection with the following settings:\n"
           << "Configuration: " + string(PROJECT_PATH "haarcascade_frontalface_alt2.xml") + "\n";
        throw invalid_argument(ss.str());
    }
    if (eye_cascade.empty()) {
        ostringstream ss;
        ss << "Failed to load haarcascade for eye detection with the following settings:\n"
           << "Configuration: " + string(PROJECT_PATH "haarcascade_eye.xml") + "\n";
        throw invalid_argument(ss.str());
    }
}
FaceDetectorCascade::~FaceDetectorCascade()
{
    delete instance;
}
FaceDetectorCascade* FaceDetectorCascade::getInstance()
{
    if (instance == nullptr) instance = new FaceDetectorCascade();
    return instance;
}
vector<Rect_<int>> FaceDetectorCascade::detectEyes(const Mat& frame)
{
    vector<Rect_<int>> eyes;
    eye_cascade.detectMultiScale(frame, eyes, 1.20, 3, 0|CASCADE_SCALE_IMAGE, Size(30, 30));
    return eyes;
}

vector<Rect> FaceDetectorCascade::detectFaces(const Mat& frame)
{
    vector<Rect> faces;
    face_cascade.detectMultiScale(frame, faces, 1.20, 3, 0|CASCADE_SCALE_IMAGE, Size(30, 30));
    return faces;
}
