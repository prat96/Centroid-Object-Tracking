/*
Created by pratheek on 2019-11-27.
*/
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

std::vector<float>::size_type findMin(const std::vector<float> &v, std::vector<float>::size_type pos = 0) {
    if (v.size() <= pos) return (v.size());
    std::vector<float>::size_type min = pos;
    for (std::vector<float>::size_type i = pos + 1; i < v.size(); i++) {
        if (v[i] < v[min]) min = i;
    }
    return (min);
}

std::map<int, std::pair<int, int>> CentroidTracker::update(vector<vector<int>> boxes) {
    struct vecRowSort {
        bool operator()(const vector<float> &first, const vector<float> &second) const {
            return first[0] < second[0];
        }
    };

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
    vector<pair<int, int>> inputCentroids;
    for (auto b : boxes) {
        int cX = int((b[0] + b[2]) / 2.0);
        int cY = int((b[1] + b[3]) / 2.0);
        inputCentroids.push_back(make_pair(cX, cY));
    }

//if we are currently not tracking any objects take the input centroids and register each of them
    if (this->objects.empty()) {
        for (auto i: inputCentroids) {
            this->register_Object(i.first, i.second);
        }
    }

// otherwise, there are currently tracking objects so we need to try to match the
// input centroids to existing object centroids
    else {
        vector<int> objectIDs;
        vector<pair<int, int>> objectCentroids;
        for (auto object : this->objects) {
            objectIDs.push_back(object.first);
            objectCentroids.push_back(make_pair(object.second.first, object.second.second));
        }

//        Calculate Distances
        vector<vector<float>> Distances;
        for (int i = 0; i < objectCentroids.size(); ++i) {
            vector<float> temp_D;
            for (vector<vector<int>>::size_type j = 0; j < inputCentroids.size(); ++j) {
                double dist = calcDistance(objectCentroids[i].first, objectCentroids[i].second, inputCentroids[j].first,
                                           inputCentroids[j].second);

                temp_D.push_back(dist);
            }
            Distances.push_back(temp_D);
        }

        // load rows and cols
        vector<int> cols;
        vector<int> rows;

        //find indices for rows
        for (auto v: Distances){
            auto temp = findMin(v);
            rows.push_back(temp);
        }

//        for (auto r: rows){
//            cout << "Rows: " << r << " " << flush;
//        }
//        cout << endl;

        //sort each mat row for col calculation
        vector<vector<float>> D_copy;
        for (auto v: Distances) {
            sort(v.begin(), v.end());
            D_copy.push_back(v);
        }

        // sort mat by rows
        sort(D_copy.begin(), D_copy.end(), vecRowSort());


        /*
        // Distances check
        for (auto i: Distances) {
            cout << "D: ";
            for (auto j : i) {
                cout << j << " " << flush;
            }
            cout << endl;
        }

        // D_copy check
        for (auto i: D_copy) {
            cout << "D: ";
            for (auto j : i) {
                cout << j << " " << flush;
            }
            cout << endl;
        }
        */


        /*
        vector<int> cols;
        vector<int> rows;

        struct vecRowSort {
            bool operator()(const map<float, int> &first, const map<float, int> &second) const {
                return first.begin()->first < second.begin()->first;
            }
        };


        // load rows
        for (auto i: Distances) {
            rows.push_back(i.begin()->second);
        }

        map<float, int>::iterator it;

        //load cols

        cout << "D size: " << Distances.size() << endl;
        cout << "Row size: " << rows.size() << endl;

        set<double> used;
        set<double> unused;

        for (auto r: rows) {
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
        */
    }
    cout << "<---------------------->" << endl;
    return this->objects;
}