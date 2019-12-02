#include <iostream>
#include <centroidtracker.h>
#include <opencv2/dnn.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

using namespace cv;
using namespace std;

int main() {
    std::cout << "Hello, Tracker!" << std::endl;
    auto centroidTracker = new CentroidTracker(20);

    VideoCapture cap(0);
//    VideoCapture cap("../../test2.mp4");
    if (!cap.isOpened()) {
        cout << "Cannot open camera";
    }

    String modelTxt = "../model/deploy.prototxt";
    String modelBin = "../model/res10_300x300_ssd_iter_140000.caffemodel";

    cout << "Loading model.." << endl;
    auto net = dnn::readNetFromCaffe(modelTxt, modelBin);

    cout << "Starting video stream" << endl;
    while (cap.isOpened()) {
        Mat cameraFrame;
        cap.read(cameraFrame);

        resize(cameraFrame, cameraFrame, Size(400, 300));
        auto inputBlob = dnn::blobFromImage(cameraFrame, 1.0, Size(400, 300), Scalar(104.0, 177.0, 123.0));

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
            }
        }

        auto objects = centroidTracker->update(boxes);

        if (!objects.empty()) {
            for (auto obj: objects) {
                circle(cameraFrame, Point(obj.second.first, obj.second.second), 4, Scalar(255, 0, 0), -1);
                string ID = std::to_string(obj.first);
                cv::putText(cameraFrame, ID, Point(obj.second.first - 10, obj.second.second - 10),
                            FONT_HERSHEY_COMPLEX, 0.5, Scalar(0, 255, 0), 2);
            }

            //drawing the path
            for (auto obj: objects) {
                int k = 1;
                for (int i = 1; i < centroidTracker->path_keeper[obj.first].size(); i++) {
                    int thickness = int(sqrt(20 / float(k + 1) * 2.5));
                    cv::line(cameraFrame,
                             Point(centroidTracker->path_keeper[obj.first][i - 1].first, centroidTracker->path_keeper[obj.first][i - 1].second),
                             Point(centroidTracker->path_keeper[obj.first][i].first, centroidTracker->path_keeper[obj.first][i].second),
                             Scalar(0, 0, 255), thickness);
                    k += 1;
                }
            }
        }
        imshow("Detection", cameraFrame);
        char c = (char) waitKey(15);
        if (c == 27)
            break;
    }
    delete centroidTracker;
    return 0;
}