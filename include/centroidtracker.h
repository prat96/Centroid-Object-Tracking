/*
Created by pratheek on 2019-11-27.
*/

#ifndef C___CENTROIDTRACKER_H
#define C___CENTROIDTRACKER_H

#endif //C___CENTROIDTRACKER_H

#include <iostream>
#include <vector>
#include <map>
#include <cmath>
#include <set>
#include <algorithm>

class CentroidTracker {
public:
    explicit CentroidTracker(int maxDisappeared);

    void register_Object(int cX, int cY);

    std::vector<std::pair<int, std::pair<int, int>>> update(std::vector<std::vector<int>> boxes);

    // <ID, centroids>
    std::vector<std::pair<int, std::pair<int, int>>> objects;

    //make buffer for path tracking
    std::map<int, std::vector<std::pair<int, int>>> path_keeper;
private:
    int maxDisappeared;

    int nextObjectID;

    static double calcDistance(double x1, double y1, double x2, double y2);

    // <ID, count>
    std::map<int, int> disappeared;
};
