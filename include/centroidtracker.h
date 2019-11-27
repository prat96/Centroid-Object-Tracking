/*
Created by pratheek on 2019-11-27.
*/

#ifndef C___CENTROIDTRACKER_H
#define C___CENTROIDTRACKER_H

#endif //C___CENTROIDTRACKER_H

#include <iostream>

class CentroidTracker {
public:
    CentroidTracker(int maxDisappeared);

    void register_Object(double centroid);

    void deregister_Object(double centroid);

    int getArg1();

    void setArg1(int arg1);

private:
    int maxDisappeared;  // Can be accessed via the setter/getter
    int nextObjectID;
};
