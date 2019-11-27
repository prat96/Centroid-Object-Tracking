/*
Created by pratheek on 2019-11-27.
*/
#include "centroidtracker.h"

CentroidTracker::CentroidTracker(int maxDisappeared) {
    this->nextObjectID = 0;

    // ordered dict "objects"
    // ordered dict "disappeared"

    this->maxDisappeared = maxDisappeared;
}


void CentroidTracker::register_Object(double centroid) {

}

void CentroidTracker::deregister_Object(double centroid) {

}

void CentroidTracker::update(vector<vector<int>> boxes) {
    if (!boxes.empty()) {
        cout << boxes[0].at(0) << " " << boxes.size() << endl;
    }
}
