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
    
    //create points
    objPoints.create(4, 1, CV_32FC3);
    // set coordinate system in the middle of the marker, with Z pointing out
    objPoints.ptr< Vec3f >(0)[0] = Vec3f(-markerLength / 2.f, markerLength / 2.f, 0);
    objPoints.ptr< Vec3f >(0)[1] = Vec3f(markerLength / 2.f, markerLength / 2.f, 0);
    objPoints.ptr< Vec3f >(0)[2] = Vec3f(markerLength / 2.f, -markerLength / 2.f, 0);
    objPoints.ptr< Vec3f >(0)[3] = Vec3f(-markerLength / 2.f, -markerLength / 2.f, 0);
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
//    vector<Vec3d> rvecArray;
//    vector<Vec3d> tvecArray;
//    
//    Vec3d tvec3d = Vec<double, 3>(tvec.at<double>(0, 0),
//                                  tvec.at<double>(1, 0),
//                                  tvec.at<double>(2, 0));
//    
//    Vec3d rvec3d = Vec<double, 3>(rvec.at<double>(0, 0),
//                                  rvec.at<double>(1, 0),
//                                  rvec.at<double>(2, 0));
//
//    rvecArray.push_back(rvec3d);
//    tvecArray.push_back(tvec3d);
    
    drawAxis(image, cameraMatrix, distCoeffs, rvec, tvec, markerLength / 0.5);
}

//static void _getSingleMarkerObjectPoints(float markerLength, OutputArray _objPoints) {
//    
//    CV_Assert(markerLength > 0);
//    
//
//}

/*
 markerLength:
 
 marker side in meters or in any other unit. 
 Note that the translation vectors of the estimated poses 
 will be in the same unit
 */
void BECVMarkers::estimate(const Mat cameraMatrix, const Mat distCoeffs, Mat& rvec, Mat& tvec)
{
    vector<Vec3d> rvecArray;
    vector<Vec3d> tvecArray;
    
//    Mat markerObjPoints;
//    _getSingleMarkerObjectPoints(markerLength, markerObjPoints);
//    int nMarkers = (int)corners.total();
//    rvecArray.create(nMarkers, 1, CV_64FC3);
//    tvecArray.create(nMarkers, 1, CV_64FC3);
//    
//    Mat rvecs = _rvecs.getMat(), tvecs = _tvecs.getMat();
    
    //// for each marker, calculate its pose
    //for (int i = 0; i < nMarkers; i++) {
        solvePnPRansac(objPoints, corners[0], cameraMatrix, distCoeffs, rvec, tvec);
    //}
    
    //estimatePoseSingleMarkers(corners, markerLength, cameraMatrix, distCoeffs, rvecArray, tvecArray);

//    tvec.at<double>(0, 0) = tvecArray[0][0];
//    tvec.at<double>(1, 0) = tvecArray[0][1];
//    tvec.at<double>(2, 0) = tvecArray[0][2];
//
//    rvec.at<double>(0, 0) = rvecArray[0][0];
//    rvec.at<double>(1, 0) = rvecArray[0][1];
//    rvec.at<double>(2, 0) = rvecArray[0][2];
//
//    cout << "rvecArray:" << rvecArray[0] << '\n';
//    cout << "tvecArray:" << tvecArray[0] << '\n';
//    cout << "rvec:" << rvec << '\n';
//    cout << "tvec:" << tvec << '\n';
}

int BECVMarkers::getId()
{
    return markerIds[0];
}

