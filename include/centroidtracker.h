/*
Created by pratheek on 2019-11-27.
*/

#ifndef C___CENTROIDTRACKER_H
#define C___CENTROIDTRACKER_H

#endif //C___CENTROIDTRACKER_H


class CentroidTracker {
public:
    CentroidTracker(int arg1, bool arg2 = true);

    int myFunc(int arg3);

    int getArg1();

    void setArg1(int arg1);

private:
    int arg1;  // Can be accessed via the setter/getter
    bool arg2; // Cannot be accessed outside of the class
};
