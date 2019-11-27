/*
Created by pratheek on 2019-11-27.
*/

class CentroidTracker {
public:
    CentroidTracker (int arg1, bool arg2 = true);
    int myFunc (int arg3);
    int getArg1 ();
    void setArg1 (int arg1);
private:
    int arg1;  // Can be accessed via the setter/getter
    bool arg2; // Cannot be accessed outside of the class
};

CentroidTracker::CentroidTracker(int arg1, bool arg2 = true) {
    this.arg1 = arg1;
    this.arg2 = arg2;
}

int CentroidTracker::myFunc (int arg3) {
    return arg3;
}

// Getter
int CentroidTracker::getArg1 () {
    return this.arg1;
}

// Setter
void CentroidTracker::setArg1 (int arg1) {
    this.arg1 = arg1;
}