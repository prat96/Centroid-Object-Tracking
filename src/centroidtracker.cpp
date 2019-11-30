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
        map<float, int> temp_rows;
        int k = 0;
        for (auto i: D_copy) {
            temp_rows.insert({i[0], k});
            k++;
        }
        //print sorted indices of temp_rows
        for (auto const &x : temp_rows) {
            std::cout << x.first << ':' << x.second << std::endl;
            rows.push_back(x.second);
        }

        //sort mat by rows
        sort(D_copy.begin(), D_copy.end(), vecRowSort());

        // <--------------All print checks---------------->
        //rows check
        for (auto r: rows) {
            cout << "Rows: " << r << " " << flush;
        }
        cout << endl;

        //cols  check
        for (auto c: cols) {
            cout << "Cols: " << c << " " << flush;
        }
        cout << endl;

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
            cout << "D_copy: ";
            for (auto j : i) {
                cout << j << " " << flush;
            }
            cout << endl;
        }
        // <--------------All print checks---------------->

        set<int> usedRows;
        set<int> usedCols;

        //loop over the combination of the (rows, columns) index tuples
        for (int i = 0; i < rows.size(); i++) {
            //if we have already examined either the row or column value before, ignore it
            if (usedRows.count(rows[i]) || usedCols.count(cols[i])) { continue; }
            //otherwise, grab the object ID for the current row, set its new centroid,
            // and reset the disappeared counter
            int objectID = objectIDs[rows[i]];
            this->objects[objectID] = inputCentroids[cols[i]];
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
                    this->deregister_Object(objectID);
                }
            }
        } else {
            for (auto col: unusedCols){
                this->register_Object(inputCentroids[col].first, inputCentroids[col].second);
            }
        }
                      }
    cout << "<---------------------->" << endl;
    return this->objects;
}