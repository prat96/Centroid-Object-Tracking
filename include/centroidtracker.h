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

    void deregister_Object(double centroid);

    void update(std::vector<std::vector<int>> boxes);

private:
    int maxDisappeared;  // Can be accessed via the setter/getter
    int nextObjectID;
    std::map<int, std::pair<int, int>> objects;
    std::map<int, std::pair<int, int>> disappeared;
};
