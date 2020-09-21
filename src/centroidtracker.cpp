/*
Created by pratheek on 2019-11-27.
*/
#include "centroidtracker.h"
#include <iterator>

using namespace std;

CentroidTracker::CentroidTracker(int maxDisappeared) {
    this->nextObjectID = 0;
    this->maxDisappeared = maxDisappeared;
}

double CentroidTracker::calcDistance(double x1, double y1, double x2, double y2) {
    double x = x1 - x2;
    double y = y1 - y2;
    double dist = sqrt((x * x) + (y * y));       //calculating Euclidean distance

    return dist;
}

void CentroidTracker::register_Object(int cX, int cY) {
    int object_ID = this->nextObjectID;
    this->objects.push_back({object_ID, {cX, cY}});
    this->disappeared.insert({object_ID, 0});
    this->nextObjectID += 1;
}

vector<float>::size_type findMin(const vector<float> &v, vector<float>::size_type pos = 0) {
    if (v.size() <= pos) return (v.size());
    vector<float>::size_type min = pos;
    for (vector<float>::size_type i = pos + 1; i < v.size(); i++) {
        if (v[i] < v[min]) min = i;
    }
    return (min);
}

std::vector<std::pair<int, std::pair<int, int>>> CentroidTracker::update(vector<vector<int>> boxes) {
    if (boxes.empty()) {
        auto it = this->disappeared.begin();
        while (it != this->disappeared.end()) {
            it->second++;
            if (it->second > this->maxDisappeared) {
                this->objects.erase(remove_if(this->objects.begin(), this->objects.end(), [it](auto &elem) {
                    return elem.first == it->first;
                }), this->objects.end());

                this->path_keeper.erase(it->first);

                it = this->disappeared.erase(it);
            } else {
                ++it;
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

        //find indices for cols
        for (auto v: Distances) {
            auto temp = findMin(v);
            cols.push_back(temp);
        }

        //rows calculation
        //sort each mat row for rows calculation
        vector<vector<float>> D_copy;
        for (auto v: Distances) {
            sort(v.begin(), v.end());
            D_copy.push_back(v);
        }

        // use cols calc to find rows
        // slice first elem of each column
        vector<pair<float, int>> temp_rows;
        int k = 0;
        for (auto i: D_copy) {
            temp_rows.push_back(make_pair(i[0], k));
            k++;
        }
        //print sorted indices of temp_rows
        for (auto const &x : temp_rows) {
            rows.push_back(x.second);
        }

        set<int> usedRows;
        set<int> usedCols;

        //loop over the combination of the (rows, columns) index tuples
        for (int i = 0; i < rows.size(); i++) {
            //if we have already examined either the row or column value before, ignore it
            if (usedRows.count(rows[i]) || usedCols.count(cols[i])) { continue; }
            //otherwise, grab the object ID for the current row, set its new centroid,
            // and reset the disappeared counter
            int objectID = objectIDs[rows[i]];
            for (int t = 0; t < this->objects.size(); t++) {
                if (this->objects[t].first == objectID) {
                    this->objects[t].second.first = inputCentroids[cols[i]].first;
                    this->objects[t].second.second = inputCentroids[cols[i]].second;
                }
            }
            this->disappeared[objectID] = 0;

            usedRows.insert(rows[i]);
            usedCols.insert(cols[i]);
        }

        // compute indexes we have NOT examined yet
        set<int> objRows;
        set<int> inpCols;

        //D.shape[0]
        for (int i = 0; i < objectCentroids.size(); i++) {
            objRows.insert(i);
        }
        //D.shape[1]
        for (int i = 0; i < inputCentroids.size(); i++) {
            inpCols.insert(i);
        }

        set<int> unusedRows;
        set<int> unusedCols;

        set_difference(objRows.begin(), objRows.end(), usedRows.begin(), usedRows.end(),
                       inserter(unusedRows, unusedRows.begin()));
        set_difference(inpCols.begin(), inpCols.end(), usedCols.begin(), usedCols.end(),
                       inserter(unusedCols, unusedCols.begin()));


        //If objCentroids > InpCentroids, we need to check and see if some of these objects have potentially disappeared
        if (objectCentroids.size() >= inputCentroids.size()) {
            // loop over unused row indexes
            for (auto row: unusedRows) {
                int objectID = objectIDs[row];
                this->disappeared[objectID] += 1;

                if (this->disappeared[objectID] > this->maxDisappeared) {
                    this->objects.erase(remove_if(this->objects.begin(), this->objects.end(), [objectID](auto &elem) {
                        return elem.first == objectID;
                    }), this->objects.end());

                    this->path_keeper.erase(objectID);

                    this->disappeared.erase(objectID);
                }
            }
        } else {
            for (auto col: unusedCols) {
                this->register_Object(inputCentroids[col].first, inputCentroids[col].second);
            }
        }
    }
    //loading path tracking points
    if (!objects.empty()) {
        for (auto obj: objects) {

            if (path_keeper[obj.first].size() > 30) {
                path_keeper[obj.first].erase(path_keeper[obj.first].begin());
            }
            path_keeper[obj.first].push_back(make_pair(obj.second.first, obj.second.second));
        }
    }


    return this->objects;
}
