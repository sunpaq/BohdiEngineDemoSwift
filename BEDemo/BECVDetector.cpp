//#ifdef USE_OPENCV

#include "BECVDetector.hpp"

BECVDetector::BECVDetector(int width, int height, float unit, Pattern patternType, int flags)
{
    boardSize = Size_<int>(width, height);
    unitSize  = unit;
    pattern   = patternType;
    
    intrinsicMatCalculated = false;
    estimateFlags = flags;
    
    cameraMatrix = Mat::eye(3, 3, CV_64F);
    distCoeffs   = Mat::zeros(8, 1, CV_64F);
    
    R = Mat::zeros(3, 1, CV_64F);
    T = Mat::zeros(3, 1, CV_64F);
    
    points2D = vector<Point2f>();
    points3D = vector<Point3f>();
    
    //prepare marker points
    if (pattern == CHESSBOARD || pattern == CIRCLES_GRID) {
        for( int i = 0; i < boardSize.height; ++i )
            for( int j = 0; j < boardSize.width; ++j )
                points3D.push_back(Point3f(j*unitSize, i*unitSize, 0));
    }
    else if (pattern == ASYMMETRIC_CIRCLES_GRID) {
        for( int i = 0; i < boardSize.height; i++ )
            for( int j = 0; j < boardSize.width; j++ )
                points3D.push_back(Point3f((2*j + i % 2)*unitSize, i*unitSize, 0));
    }
}

bool BECVDetector::detect(Mat& image)
{
    bool found = false;
    if (pattern == CHESSBOARD) {
        found = findChessboardCorners(image, boardSize, points2D);
    }
    else if (pattern == CIRCLES_GRID) {
        found = findCirclesGrid(image, boardSize, points2D);
    }
    else if (pattern == ASYMMETRIC_CIRCLES_GRID) {
        found = findCirclesGrid(image, boardSize, points2D, CALIB_CB_ASYMMETRIC_GRID);
    }
    
    if (found) {
        //calculate subpixel
        cornerSubPix(image, points2D, boardSize, Size_<int>(-1,-1), TermCriteria(CV_TERMCRIT_ITER, 30, 0.1));
    }

    return found;
}

double BECVDetector::calibrate(Mat& image)
{
    vector<vector<Point2f>> points2DArray;
    vector<vector<Point3f>> points3DArray;
    points2DArray.push_back(points2D);
    points3DArray.push_back(points3D);
    points3DArray.resize(points2DArray.size(), points3DArray[0]);

    return calibrateCamera(points3DArray, points2DArray, image.size(), cameraMatrix, distCoeffs, noArray(), noArray());
}

bool BECVDetector::estimate(int flags)
{
    bool OK = solvePnP(points3D, points2D, cameraMatrix, distCoeffs, R, T, false, flags);
    if (OK) {
        Mat Rod(3,3,DataType<double>::type);
        Rodrigues(R, Rod);
        //cout << "Rodrigues = "<< endl << " "  << Rod << endl << endl;
        
        static double flip[] = {
            1, 0, 0,
            0,-1, 0,
            0, 0,-1
        };
        Mat_<double> flipX(3,3,flip);
        
        Rod = flipX * Rod;
        T   = flipX * T;
        float scale = 1;
        
        extrinsicMatColumnMajor[0] = Rod.at<double>(0, 0);
        extrinsicMatColumnMajor[1] = Rod.at<double>(1, 0);
        extrinsicMatColumnMajor[2] = Rod.at<double>(2, 0);
        extrinsicMatColumnMajor[3] = 0.0f;
        
        extrinsicMatColumnMajor[4] = Rod.at<double>(0, 1);
        extrinsicMatColumnMajor[5] = Rod.at<double>(1, 1);
        extrinsicMatColumnMajor[6] = Rod.at<double>(2, 1);
        extrinsicMatColumnMajor[7] = 0.0f;
        
        extrinsicMatColumnMajor[8]  = Rod.at<double>(0, 2);
        extrinsicMatColumnMajor[9]  = Rod.at<double>(1, 2);
        extrinsicMatColumnMajor[10] = Rod.at<double>(2, 2);
        extrinsicMatColumnMajor[11] = 0.0f;
        
        extrinsicMatColumnMajor[12] = scale * T.at<double>(0, 0);
        extrinsicMatColumnMajor[13] = scale * T.at<double>(1, 0);
        extrinsicMatColumnMajor[14] = scale * T.at<double>(2, 0);
        extrinsicMatColumnMajor[15] = 1.0f;
    }
    
    return OK;
}

bool BECVDetector::processImage(Mat& image) {
    try {
        Mat gray;
        cvtColor(image, gray, COLOR_BGR2GRAY);
        
        if (intrinsicMatCalculated == false) {
            if (!detect(gray)) {
                return false;
            }
            double RMS = calibrate(gray);
            if(RMS < 0.1 || RMS > 1.0 || !checkRange(cameraMatrix) || !checkRange(distCoeffs)){
                return false;
            }
            intrinsicMatCalculated = true;
        }
        
        if (detect(gray) && points2D.size() == 20) {
            drawChessboardCorners(image, boardSize, points2D, false);
            return estimate(estimateFlags);
        }
        
    } catch (exception& e) {
        cout << e.what() << '\n';
    }
    return false;
}

//#endif

