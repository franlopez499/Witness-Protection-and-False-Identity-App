#include "facedetector.h"

#include <sstream>
#include <vector>
#include <string>
#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;


FaceDetector* FaceDetector::instance = nullptr;

FaceDetector::FaceDetector() : confidence_threshold_(0.5), input_image_height_(300), input_image_width_(300),
                               scale_factor_(1.0), mean_values_({104., 177.0, 123.0})
{


    network_ = dnn::readNetFromCaffe(PROJECT_PATH "deploy.prototxt", PROJECT_PATH "res10_300x300_ssd_iter_140000_fp16.caffemodel");

    if (network_.empty()) {
        ostringstream ss;
        ss << "Failed to load network with the following settings:\n"
           << "Configuration: " + string(PROJECT_PATH "deploy.prototxt") + "\n"
           << "Binary: " + string(PROJECT_PATH "res10_300x300_ssd_iter_140000_fp16.caffemodel") + "\n";
        throw invalid_argument(ss.str());
    }
}
FaceDetector::~FaceDetector()
{
    delete instance;
}
FaceDetector* FaceDetector::getInstance()
{
    if (instance == nullptr) instance = new FaceDetector();
    return instance;
}

vector<Rect> FaceDetector::detect_face_rectangles(const Mat &frame)
{
    Mat aux = Mat::zeros(Size(300,300), CV_8UC1);
    resize(frame,aux,Size(300,300));
    Mat input_blob = dnn::blobFromImage(frame, scale_factor_, Size(input_image_width_, input_image_height_),
                                                mean_values_, false, false);
    network_.setInput(input_blob, "data");
    Mat detection = network_.forward("detection_out");
    Mat detection_matrix(detection.size[2], detection.size[3], CV_32F, detection.ptr<float>());

    vector<Rect> faces;

    for (int i = 0; i < detection_matrix.rows; i++) {
        float confidence = detection_matrix.at<float>(i, 2);

        if (confidence < confidence_threshold_) {
            continue;
        }
        int x_left_bottom = static_cast<int>(detection_matrix.at<float>(i, 3) * frame.cols);
        int y_left_bottom = static_cast<int>(detection_matrix.at<float>(i, 4) * frame.rows);
        int x_right_top = static_cast<int>(detection_matrix.at<float>(i, 5) * frame.cols);
        int y_right_top = static_cast<int>(detection_matrix.at<float>(i, 6) * frame.rows);

        faces.emplace_back(x_left_bottom, y_left_bottom, (x_right_top - x_left_bottom), (y_right_top - y_left_bottom));
    }

    return faces;
}
