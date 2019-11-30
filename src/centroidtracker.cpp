/*
Created by pratheek on 2019-11-27.
*/
#include <set>
#include <algorithm>
#include "centroidtracker.h"

using namespace std;

CentroidTracker::CentroidTracker(int maxDisappeared) {
    this->nextObjectID = 0;
    this->maxDisappeared = maxDisappeared;
}

double CentroidTracker::calcDistance(double x1, double y1, double x2, double y2) {
    double x = x1 - x2; //calculating number to square in next step
    double y = y1 - y2;
    double dist = sqrt((x * x) + (y * y));       //calculating Euclidean distance

    return dist;
}

void CentroidTracker::register_Object(int cX, int cY) {
    int object_ID = this->nextObjectID;
    this->objects.insert({object_ID, {cX, cY}});
    this->disappeared.insert({object_ID, 0});
    this->nextObjectID += 1;
}

void CentroidTracker::deregister_Object(int objectID) {
    cout << "Deregistered object: " << objectID << endl;
    if (!this->objects.empty()) {
        this->objects.erase(objectID);
        this->disappeared.erase(objectID);
    }
}

std::map<int, std::pair<int, int>> CentroidTracker::update(vector<vector<int>> boxes) {

    if (boxes.empty()) {
        if (!this->disappeared.empty()) {
            for (auto elem : this->disappeared) {
                this->disappeared[elem.first]++;

                if (elem.second > this->maxDisappeared) {
                    this->deregister_Object(elem.first);
                }
            }
        }
        return this->objects;
    }

    // initialize an array of input centroids for the current frame
    std::map<int, std::vector<int>> inputCentroids;
    // loop over the bounding box rectangles
    int k = 0;
    for (auto b : boxes) {
        // use the bounding box coordinates to derive the centroid
        int cX = int((b[0] + b[2]) / 2.0);
        int cY = int((b[1] + b[3]) / 2.0);
        inputCentroids.insert(pair<int, vector<int>>({k, {cX, cY}}));
        k++;
    }


//if we are currently not tracking any objects take the input centroids and register each of them
    if (this->objects.empty()) {
        for (auto &inp: inputCentroids) {
            this->register_Object(inp.second[0], inp.second[1]);
        }
    }

// otherwise, there are currently tracking objects so we need to try to match the
// input centroids to existing object centroids
    else {
        vector<int> objectIDs;
        vector<vector<int>> objectCentroids;
        for (auto &object : this->objects) {
            objectIDs.push_back(object.first);
            objectCentroids.push_back({object.second.first, object.second.second});
        }

        vector<map<float, int>> Distances;

        for (vector<vector<int>>::size_type i = 0; i < objectCentroids.size(); ++i) {
            map<float, int> temp_map;
            for (vector<vector<int>>::size_type j = 0; j < inputCentroids.size(); ++j) {
                double dist = calcDistance(objectCentroids[i][0], objectCentroids[i][1], inputCentroids[j][0],
                                           inputCentroids[j][1]);

                temp_map.insert({dist, j});
            }
            Distances.push_back(temp_map);
        }

        for (auto i: Distances) {
            cout << "D: ";
            for (auto d: i) {
                cout << " " << d.first << " " << flush;
            }
            cout << endl;
        }

//        for (auto i: Distances) {
//            cout << "D index: ";
//            for (auto d: i) {
//                cout << " " << d.second << " " << flush;
//            }
//            cout << endl;
//        }



        vector<int> rows;
        vector<int> cols;

        struct vecRowSort {
            bool operator()(const map<float, int> &first, const map<float, int> &second) const {
                return first.begin()->first < second.begin()->first;
            }
        };

        sort(Distances.begin(), Distances.end(), vecRowSort());

        for (auto i: Distances) {
            rows.push_back(i.begin()->second);
        }

        cout << "D size: " << Distances.size() << endl;
        cout << "Row size: " << rows.size() << endl;

        set<double> used;
        set<double> unused;

        for (auto r: rows) {
            cout << "Check r:" << r << endl;
            if (used.count(r)) { continue; }

            int objectID = objectIDs[r];
            this->objects[objectID] = {inputCentroids[r][0], inputCentroids[r][1]};
            this->disappeared[objectID] = 0;

            used.insert(r);
        }

        // compute indexes we have NOT examined yet
        set_symmetric_difference(rows.begin(), rows.end(), used.begin(), used.end(), inserter(unused, unused.begin()));

        if (objectCentroids.size() >= inputCentroids.size()) {
            // loop over unused row indexes
            for (auto row: unused) {

                int objectID = objectIDs[row];
                this->disappeared[objectID]++;

                if (this->disappeared[objectID] > this->maxDisappeared) {
                    this->deregister_Object(objectID);
                }
            }
        } else {
            for (auto row: unused) {
                this->register_Object(inputCentroids[row][0], inputCentroids[row][1]);
            }
        }
    }
    for (auto obj: this->objects) {
//        cout << obj.first << endl;
    }
    cout << "<---------------------->" << endl;
    return this->objects;
}