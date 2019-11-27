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
        if (waitKey(30) >= 0)
            break;
    }
    return 0;
}