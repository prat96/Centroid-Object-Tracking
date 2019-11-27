/*
Created by pratheek on 2019-11-27.
*/
#include "centroidtracker.h"

CentroidTracker::CentroidTracker(int maxDisappeared) {
    this->maxDisappeared = maxDisappeared;
    this->nextObjectID = 0;
}


void CentroidTracker::register_Object(double centroid) {

}

void CentroidTracker::deregister_Object(double centroid) {

}