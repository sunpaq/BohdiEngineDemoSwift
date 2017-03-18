//
//  BECVMarkers.cpp
//  BEDemo
//
//  Created by YuliSun on 17/03/2017.
//  Copyright © 2017 SODEC. All rights reserved.
//

#include "BECVMarkers.hpp"

BECVMarkers::BECVMarkers(float length, PREDEFINED_DICTIONARY_NAME preDefine)
{
    markerLength = length;
    
    dict = getPredefinedDictionary(preDefine);
    params = DetectorParameters::create();    
    
    corners = vector<vector<Point2f>>();
    markerIds = vector<int>();
}

bool BECVMarkers::detect(Mat& image)
{
    corners.clear();
    markerIds.clear();
    
    int channels = ((InputArray)image).getMat().channels();
    if (channels != 1 && channels != 3) {
        cout << "channels:" << channels << '\n';
    }
    
    detectMarkers(image, dict, corners, markerIds, params);
    if (markerIds.size() > 0) {
        return true;
    }
    return false;
}

void BECVMarkers::draw(Mat& image)
{
    drawDetectedMarkers(image, corners, markerIds);
}

void BECVMarkers::axis(Mat& image, Mat cameraMatrix, Mat distCoeffs, Mat rvec, Mat tvec)
{
    drawAxis(image, cameraMatrix, distCoeffs, rvec, tvec, markerLength);
}

void BECVMarkers::estimate(Mat cameraMatrix, Mat distCoeffs, Mat& rvec, Mat& tvec)
{
    vector<Vec3d> rvecArray;
    vector<Vec3d> tvecArray;
    estimatePoseSingleMarkers(corners, markerLength, cameraMatrix, distCoeffs, rvecArray, tvecArray);
    rvec = rvecArray[0];
    tvec = tvecArray[0];
}
