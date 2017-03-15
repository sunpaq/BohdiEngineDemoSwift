//#ifdef USE_OPENCV

#include "BECVDetector.hpp"

BECVDetector::BECVDetector(int width, int height, float unit, Pattern patternType)
{
    boardSize = Size_<int>(width, height);
    unitSize  = unit;
    pattern   = patternType;
    
    intrinsicMatCalculated = false;
    
    cameraMatrix = Mat::eye(3, 3, CV_64F);
    distCoeffs   = Mat::zeros(8, 1, CV_64F);
    
    R = Mat::zeros(3, 1, CV_64F);
    T = Mat::zeros(3, 1, CV_64F);
    
    boardSize = Size_<int>(5, 4);
    vector<vector<Point3f>> _points3D(1);
    calcBoardCornerPositions(boardSize, unitSize, _points3D[0], pattern);
    points3D = _points3D;
}

void BECVDetector::calcBoardCornerPositions(Size_<int> boardSize, float squareSize, vector<Point3f>& corners, Pattern patternType)
{
    corners.clear();
    
    switch(patternType)
    {
        case CHESSBOARD:
        case CIRCLES_GRID:
            for( int i = 0; i < boardSize.height; ++i )
                for( int j = 0; j < boardSize.width; ++j )
                    corners.push_back(Point3f(j*squareSize, i*squareSize, 0));
            break;
            
        case ASYMMETRIC_CIRCLES_GRID:
            for( int i = 0; i < boardSize.height; i++ )
                for( int j = 0; j < boardSize.width; j++ )
                    corners.push_back(Point3f((2*j + i % 2)*squareSize, i*squareSize, 0));
            break;
        default:
            break;
    }
}

void BECVDetector::calcBoardCornerSubpixels(Mat& image, Size_<int> boardSize, vector<Point2f> cornerPoints)
{
    Mat viewGray;
    cvtColor(image, viewGray, COLOR_BGR2GRAY);
    cornerSubPix(viewGray, cornerPoints, boardSize, Size_<int>(-1,-1), TermCriteria( CV_TERMCRIT_ITER, 30, 0.1));
}

bool BECVDetector::findPattern(Mat& image)
{
    switch (pattern) {
        default:
        case CHESSBOARD:
            return findChessboardCorners(image, boardSize, cornerPoints);
            break;
        case CIRCLES_GRID:
            return findCirclesGrid(image, boardSize, cornerPoints);
            break;
        case ASYMMETRIC_CIRCLES_GRID:
            return findCirclesGrid(image, boardSize, cornerPoints, CALIB_CB_ASYMMETRIC_GRID);
            break;
    }
}

void BECVDetector::processImage(Mat& image) {
    try {
        vector<vector<Point2f>> points2D;
        Mat Rod(3,3,DataType<double>::type);
        
        if (intrinsicMatCalculated == false) {
            int chessBoardFlags = CALIB_CB_ADAPTIVE_THRESH | CALIB_CB_NORMALIZE_IMAGE;
            bool found = findChessboardCorners(image, boardSize, cornerPoints, chessBoardFlags);
            if (!found) {
                return;
            }
            
            calcBoardCornerSubpixels(image, boardSize, cornerPoints);
            points2D.push_back(cornerPoints);
            points3D.resize(points2D.size(), points3D[0]);
            
            double RMS = 0.0;
            RMS = calibrateCamera(points3D, points2D, image.size(), cameraMatrix, distCoeffs, noArray(), noArray());
            if(RMS < 0.1 || RMS > 1.0 || !checkRange(cameraMatrix) || !checkRange(distCoeffs)){
                return;
            }
            intrinsicMatCalculated = true;
        }
        
        int chessBoardFlags = CALIB_CB_ADAPTIVE_THRESH | CALIB_CB_NORMALIZE_IMAGE | CALIB_CB_FAST_CHECK;
        bool found = findChessboardCorners(image, boardSize, cornerPoints, chessBoardFlags);
        if (found && cornerPoints.size() == 20) {
            
            calcBoardCornerSubpixels(image, boardSize, cornerPoints);
            points2D.push_back(cornerPoints);
            points3D.resize(points2D.size(), points3D[0]);
            
            drawChessboardCorners(image, boardSize, cornerPoints, !found);
            
            bool OK = solvePnP(points3D.front(), points2D.front(), cameraMatrix, distCoeffs, R, T, false, CV_ITERATIVE);
            if (OK) {
                //cout << "R = "<< endl << " "  << R << endl << endl;
                //cout << "T = "<< endl << " "  << T << endl << endl;
                
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
        }
        
    } catch (exception& e) {
        cout << e.what() << '\n';
    }
}

//#endif

