/*
Created by pratheek on 2019-11-27.
*/
#include <set>
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
//    cout << "Centroids: " << cX << " " << cY << endl;
    int object_ID = this->nextObjectID;
    this->objects.insert({object_ID, {cX, cY}});
    this->disappeared.insert({object_ID, 0});
    this->nextObjectID += 1;

    for (auto elem : this->objects) {
        std::cout << "CHECKING OBJECTS: " << elem.first << " " << elem.second.first << " " << elem.second.second
                  << "\n";
    }
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
        if (!disappeared.empty()) {
            for (auto elem : this->disappeared) {
                std::cout << "CHECKING DISAPPEARED: " << elem.first << " " << elem.second << " " << disappeared.size()
                          << endl;
                disappeared[elem.first]++;

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

// grab the set of object IDs and corresponding centroids
    else {
        vector<int> objectIDs;
        vector<vector<int>> objectCentroids;
        for (auto &object : this->objects) {
            objectIDs.push_back(object.first);
            objectCentroids.push_back({object.second.first, object.second.second});
        }

        vector<map<float, int>> Distances;
        vector<vector<float>> D;

        for (vector<vector<int>>::size_type i = 0; i < objectCentroids.size(); ++i) {
            vector<float> temp;
            map<float, int> temp_map;
            int kj = 0;
            for (vector<vector<int>>::size_type j = 0; j < inputCentroids.size(); ++j) {
                double dist = calcDistance(objectCentroids[i][0], objectCentroids[i][1], inputCentroids[j][0],
                                           inputCentroids[j][1]);

                temp_map.insert({dist, kj});
                temp.push_back(dist);
                ++kj;
            }
            D.push_back(temp);
            Distances.push_back(temp_map);
        }

        for (auto i: Distances) {
            cout << "SIZE: " << i.size() << " " << Distances.size()<< endl;
            for (auto j: i){
                cout << j.first << " " << j.second << endl;
            }
        }

        /*
        std::set<double> used;
        std::set<double> unused;

        for (auto d : Distances) {
            if (used.count(d.second)) { continue; }

            int objectID = objectIDs[d.second];
            this->objects.at(objectID) = {inputCentroids.at(objectID)[0], inputCentroids.at(objectID)[1]};
            this->disappeared.at(objectID) = 0;

            used.insert(objectID);
        }*/
    }
    return this->objects;
}