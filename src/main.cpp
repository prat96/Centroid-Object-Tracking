#include <iostream>
#include <centroidtracker.h>
#include <opencv2/dnn.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

using namespace cv;
using namespace std;

int main() {
    std::cout << "Hello, Tracker!" << std::endl;
    int W = 300;
    auto centroidTracker = new CentroidTracker(40);

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

//                int cX = int((xLeftTop + xRightBottom) / 2.0);
//                int cY = int((yLeftTop + yRightBottom) / 2.0);
//                cout << "CENTROID CHECK: " << cX << " " << cY << endl;
//                cv::circle(cameraFrame, Point(cX, cY), 4, Scalar(255, 0, 0), -1);
//                cv::circle(cameraFrame, Point(xRightBottom, yRightBottom), 4, Scalar(255, 255, 0), -1);

            }
        }

        auto objects = centroidTracker->update(boxes);

        for (auto obj: objects) {
            circle(cameraFrame, Point(obj.second.first, obj.second.second), 4, Scalar(255, 0, 0), -1);
            cout << "INT" << obj.first << endl;
            string ID = to_string(obj.first);
            cv::putText(cameraFrame, ID, Point(obj.second.first - 10, obj.second.second - 10),
                        FONT_HERSHEY_COMPLEX, 0.5, Scalar(0, 255, 0), 2);
        }


        imshow("Detection", cameraFrame);
//        waitKey(0);
        if (waitKey(30) >= 0)
            break;
    }
    return 0;
}