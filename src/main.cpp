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
        imshow("cam", cameraFrame);

        Mat resized_camFrame;
        resize(cameraFrame, resized_camFrame, Size(300,300));
        auto inputBlob = dnn::blobFromImage(resized_camFrame, 1.0, Size(W, W),Scalar(104.0, 177.0, 123.0) );

        net.setInput(inputBlob);
        auto detection = net.forward("detection_out");
        Mat detectionMat(detection.size[2], detection.size[3], CV_32F, detection.ptr<float>());

        float confidenceThreshold = 0.2;
        for (int i = 0; i < detectionMat.rows; i++)
        {
            float confidence = detectionMat.at<float>(i, 2);

            if (confidence > confidenceThreshold)
            {
                int xLeftBottom = static_cast<int>(detectionMat.at<float>(i, 3) * cameraFrame.cols);
                int yLeftBottom = static_cast<int>(detectionMat.at<float>(i, 4) * cameraFrame.rows);
                int xRightTop = static_cast<int>(detectionMat.at<float>(i, 5) * cameraFrame.cols);
                int yRightTop = static_cast<int>(detectionMat.at<float>(i, 6) * cameraFrame.rows);

                Rect object((int)xLeftBottom, (int)yLeftBottom,
                            (int)(xRightTop - xLeftBottom),
                            (int)(yRightTop - yLeftBottom));

                rectangle(cameraFrame, object, Scalar(0, 255, 0), 2);

            }
        }
        imshow("Detection", cameraFrame);

        if (waitKey(30) >= 0)
            break;
    }
    return 0;
}