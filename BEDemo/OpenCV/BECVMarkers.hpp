//
//  BECVMarkers.hpp
//  BEDemo
//
//  Created by YuliSun on 17/03/2017.
//  Copyright © 2017 SODEC. All rights reserved.
//

#ifndef BECVMarkers_hpp
#define BECVMarkers_hpp

#import <iostream>
#import "aruco.hpp"
#import <opencv2/calib3d.hpp>

using namespace std;
using namespace cv;
using namespace aruco;

class BECVMarkers {

public:
    BECVMarkers(float length, PREDEFINED_DICTIONARY_NAME preDefine = DICT_ARUCO_ORIGINAL);
    bool detect(Mat& image);
    void draw(Mat& image);
    void axis(Mat& image, Mat cameraMatrix, Mat distCoeffs, Mat rvec, Mat tvec);
    void estimate(Mat cameraMatrix, Mat distCoeffs, Mat& rvec, Mat& tvec);
    int getId();
    
private:
    float markerLength;
    Ptr<Dictionary> dict;
    Ptr<DetectorParameters> params;
    
    Mat objPoints;
    
    vector<vector<Point2f>> corners;
    vector<int> markerIds;
};

#endif /* BECVMarkers_hpp */
