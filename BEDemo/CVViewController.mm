//
//  CVBridge.m
//  BEDemo
//
//  Created by YuliSun on 01/03/2017.
//  Copyright © 2017 SODEC. All rights reserved.
//

#ifdef __OBJC__
#import "CVViewController.h"

#ifdef __cplusplus
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
//#import <opencv2/highgui.hpp>
#endif

using namespace cv;
using namespace std;

enum Pattern { NOT_EXISTING, CHESSBOARD, CIRCLES_GRID, ASYMMETRIC_CIRCLES_GRID };

void calcBoardCornerPositions(Size_<int> boardSize, float squareSize, vector<Point3f>& corners, Pattern patternType)
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

@interface CVViewController()
{
    BOOL intrinsicMatCalculated;
    BOOL modelLoaded;
    
    Mat cameraMatrix;
    Mat distCoeffs;
    
    Mat R;
    Mat T;
    
    Size_<int> boardSize;
    vector<vector<Point3f>> points3D;
}
@end

@implementation CVViewController

-(void)prepareOpenCV
{
    intrinsicMatCalculated = NO;
    modelLoaded = NO;
    
    boardSize = Size_<int>(5, 4);
    
    cameraMatrix = Mat::eye(3, 3, CV_64F);
    distCoeffs   = Mat::zeros(8, 1, CV_64F);
    R = Mat::zeros(3, 1, CV_64F);
    T = Mat::zeros(3, 1, CV_64F);
    
    vector<vector<Point3f>> _points3D(1);
    calcBoardCornerPositions(boardSize, 0.25, _points3D[0], CHESSBOARD);
    points3D = _points3D;
}

//conform CvVideoCameraDelegate
- (void)processImage:(cv::Mat&)mat
{
    //dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_BACKGROUND, 0), ^{
        
        try {
            vector<Point2f> cornerPoints;
            vector<vector<Point2f>> points2D;
            if (intrinsicMatCalculated == NO) {
                int chessBoardFlags = CALIB_CB_ADAPTIVE_THRESH | CALIB_CB_NORMALIZE_IMAGE;
                bool found = findChessboardCorners(mat, boardSize, cornerPoints, chessBoardFlags);
                if (!found) {
                    return;
                }
                
                Mat viewGray;
                cvtColor(mat, viewGray, COLOR_BGR2GRAY);
                cornerSubPix(viewGray, cornerPoints, boardSize, Size_<int>(-1,-1), TermCriteria( CV_TERMCRIT_ITER, 30, 0.1));
                
                points2D.push_back(cornerPoints);
                points3D.resize(points2D.size(), points3D[0]);
                
                double RMS = 0.0;
                RMS = calibrateCamera(points3D, points2D, mat.size(), cameraMatrix, distCoeffs, noArray(), noArray());
                if(RMS < 0.1 || RMS > 1.0 || !checkRange(cameraMatrix) || !checkRange(distCoeffs)){
                    return;
                }
                intrinsicMatCalculated = YES;
            }
            
            int chessBoardFlags = CALIB_CB_ADAPTIVE_THRESH | CALIB_CB_NORMALIZE_IMAGE | CALIB_CB_FAST_CHECK;
            bool found = findChessboardCorners(mat, boardSize, cornerPoints, chessBoardFlags);
            if (found && cornerPoints.size() == 20) {
                
                Mat viewGray;
                cvtColor(mat, viewGray, COLOR_BGR2GRAY);
                cornerSubPix(viewGray, cornerPoints, boardSize, Size_<int>(-1,-1), TermCriteria( CV_TERMCRIT_ITER, 30, 0.1));
                
                points2D.push_back(cornerPoints);
                points3D.resize(points2D.size(), points3D[0]);
                
                drawChessboardCorners(mat, boardSize, cornerPoints, !found);

                
                //Mat R(3,1,DataType<double>::type);
                //Mat T(3,1,DataType<double>::type);
                Mat Rod(3,3,DataType<double>::type);
                
                bool OK = solvePnP(points3D.front(), points2D.front(), cameraMatrix, distCoeffs, R, T, false, CV_ITERATIVE);
                
                if (OK) {
                    
                    if (!modelLoaded) {
                        GLKVector3 lpos = {0,1000,-1000};
                        [_beViewCtl lightReset:&lpos];
                        [_beViewCtl addModelNamed:@"2.obj"];
                        modelLoaded = YES;
                    }

                    
                    //                if(!checkRange(R) || !checkRange(T)) {
                    //                    return;
                    //                }
                    
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
                    
                    //                if(!checkRange(Rod) || !checkRange(T)) {
                    //                    return;
                    //                }
                    
                    float scale = 1;
                    
                    GLKMatrix4 mat4;
                    mat4.m[0] = Rod.at<double>(0, 0);
                    mat4.m[1] = Rod.at<double>(1, 0);
                    mat4.m[2] = Rod.at<double>(2, 0);
                    mat4.m[3] = 0.0f;
                    
                    mat4.m[4] = Rod.at<double>(0, 1);
                    mat4.m[5] = Rod.at<double>(1, 1);
                    mat4.m[6] = Rod.at<double>(2, 1);
                    mat4.m[7] = 0.0f;
                    
                    mat4.m[8]  = Rod.at<double>(0, 2);
                    mat4.m[9]  = Rod.at<double>(1, 2);
                    mat4.m[10] = Rod.at<double>(2, 2);
                    mat4.m[11] = 0.0f;
                    
                    mat4.m[12] = scale * T.at<double>(0, 0);
                    mat4.m[13] = scale * T.at<double>(1, 0);
                    mat4.m[14] = scale * T.at<double>(2, 0);
                    mat4.m[15] = 1.0f;
                    
                    [_beViewCtl cameraReset:&mat4];
                }
            }
            
        } catch (exception& e) {
            cout << e.what() << '\n';
        }
        
        
    //});
    
    

}

-(void)viewDidLoad
{
    [super viewDidLoad];

    CGRect frame = [[UIScreen mainScreen] bounds];
    _cvView = [[UIView alloc] initWithFrame:frame];

    _videoSource = [[CvVideoCamera alloc] initWithParentView:_cvView];
    _videoSource.defaultAVCaptureVideoOrientation = AVCaptureVideoOrientationPortrait;
    _videoSource.defaultAVCaptureDevicePosition = AVCaptureDevicePositionBack;
    _videoSource.defaultAVCaptureSessionPreset  = AVCaptureSessionPreset640x480; //AVCaptureSessionPresetLow;
    _videoSource.imageWidth  = frame.size.width;
    _videoSource.imageHeight = frame.size.height;
    _videoSource.defaultFPS = 30;
    _videoSource.delegate = self;
    
    [self.view addSubview:_cvView];

    _beViewCtl = [[BEViewController alloc] init];
    _beViewCtl.useTransparentBackground = YES;
    

    
    [self prepareOpenCV];
}

-(void)viewDidAppear:(BOOL)animated
{
    //start openCV
    //self.videoSource.grayscaleMode = YES;
    [self.videoSource start];
    
    [self presentViewController:_beViewCtl animated:NO completion:^{
        
        
        //[_beViewCtl addModelNamed:@"2.obj"];
        
        //[be insertSubview:_cvView belowSubview:be.view];
        
        //[be insertBackgroundView:_cvView];
        
    }];
}

@end

#endif

