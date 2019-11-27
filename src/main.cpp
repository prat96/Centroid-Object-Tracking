#include <iostream>
#include <centroidtracker.h>
#include <opencv2/dnn.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/core/utils/trace.hpp>

using namespace cv;
using namespace std;

int main() {
    std::cout << "Hello, Tracker!" << std::endl;
    int W = 300;
    auto centroidTracker = new CentroidTracker(50);

    VideoCapture cap(0);
    if (!cap.isOpened()) {
        cout << "Cannot open camera";
    }

    String modelTxt = "../../deploy.prototxt";
    String modelBin = "../../res10_300x300_ssd_iter_140000.caffemodel";

    cout << "Loading model.." << endl;
    auto net = dnn::readNetFromCaffe(modelTxt, modelBin);

    cout << "Starting video stream" << endl;
    while (true) {
        Mat cameraFrame;
        cap.read(cameraFrame);

        Mat resized_camFrame;
        resize(cameraFrame, resized_camFrame, Size(300, 300));
        auto inputBlob = dnn::blobFromImage(resized_camFrame, 1.0, Size(W, W), Scalar(104.0, 177.0, 123.0));

        net.setInput(inputBlob);
        auto detection = net.forward("detection_out");
        Mat detectionMat(detection.size[2], detection.size[3], CV_32F, detection.ptr<float>());

        vector<vector<int>> boxes;

        float confidenceThreshold = 0.2;
        for (int i = 0; i < detectionMat.rows; i++) {
            float confidence = detectionMat.at<float>(i, 2);

            if (confidence > confidenceThreshold) {
                int xLeftTop = static_cast<int>(detectionMat.at<float>(i, 3) * cameraFrame.cols);
                int yLeftTop = static_cast<int>(detectionMat.at<float>(i, 4) * cameraFrame.rows);
                int xRightBottom = static_cast<int>(detectionMat.at<float>(i, 5) * cameraFrame.cols);
                int yRightBottom = static_cast<int>(detectionMat.at<float>(i, 6) * cameraFrame.rows);

                Rect object((int) xLeftTop, (int) yLeftTop, (int) (xRightBottom - xLeftTop),
                            (int) (yRightBottom - yLeftTop));
                rectangle(cameraFrame, object, Scalar(0, 255, 0), 2);

                boxes.insert(boxes.end(), {xLeftTop, yLeftTop, xRightBottom, yRightBottom});

//                cv::circle(cameraFrame, Point(boxes[0][0], boxes[0][1]), 4, Scalar(255, 0, 0), -1);
//                cv::circle(cameraFrame, Point(xRightBottom, yRightBottom), 4, Scalar(255, 255, 0), -1);

            }
        }
//        if (!boxes.empty()) {
//            cout << boxes[0].at(3) << " " << boxes.size() << endl;
//        }
        imshow("Detection", cameraFrame);

        if (waitKey(30) >= 0)
            break;
    }
    return 0;
}