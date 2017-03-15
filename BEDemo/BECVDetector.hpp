//#ifdef USE_OPENCV

#ifndef BECVDetector_hpp
#define BECVDetector_hpp

#import <iostream>
#import <sstream>
#import <string>
#import <ctime>
#import <cstdio>
#import <opencv2/core.hpp>
#import <opencv2/core/utility.hpp>
#import <opencv2/imgproc.hpp>
#import <opencv2/calib3d.hpp>
#import <opencv2/imgcodecs.hpp>
#import <opencv2/videoio.hpp>

using namespace std;
using namespace cv;

class BECVDetector {

public:
    Mat cameraMatrix;
    Mat distCoeffs;

    Mat R;
    Mat T;
    
    float intrinsicMatColumnMajor[16];
    float extrinsicMatColumnMajor[16];
    
    enum Pattern { NOT_EXISTING, CHESSBOARD, CIRCLES_GRID, ASYMMETRIC_CIRCLES_GRID };
    
    BECVDetector(int width, int height, float unit, Pattern patternType);
    void processImage(Mat& image);
    
private:
    float unitSize;
    Pattern pattern;
    Size_<int> boardSize;
    vector<Point2f> cornerPoints;
    vector<vector<Point3f>> points3D;

    bool intrinsicMatCalculated;
    
    bool findPattern(Mat& image);
    void calcBoardCornerPositions(Size_<int> boardSize, float squareSize, vector<Point3f>& corners, Pattern patternType);
    void calcBoardCornerSubpixels(Mat& image, Size_<int> boardSize, vector<Point2f> cornerPoints);
};

#endif
//#endif
