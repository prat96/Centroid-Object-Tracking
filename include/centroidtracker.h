/*
Created by pratheek on 2019-11-27.
*/

#ifndef C___CENTROIDTRACKER_H
#define C___CENTROIDTRACKER_H

#endif //C___CENTROIDTRACKER_H

#include <iostream>
#include <vector>
#include <map>

class CentroidTracker {
public:
    explicit CentroidTracker(int maxDisappeared);

    void register_Object(double centroid);

    void deregister_Object(int objectID);

    void update(std::vector<std::vector<int>> boxes);

private:
    int maxDisappeared;
    int nextObjectID;

    // <ID, centroid>
    std::map<int, std::pair<int, int>> objects;
    // <ID, count>
    std::map<int, int> disappeared;
};
