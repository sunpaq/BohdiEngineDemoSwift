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
    
    Mat cameraMatrix;
    Mat distCoeffs;
    
    Size_<int> boardSize;
    vector<vector<Point3f>> points3D;
}
@end

@implementation CVViewController

-(void)prepareOpenCV
{
    intrinsicMatCalculated = NO;

    boardSize = Size_<int>(4, 5);
    
    cameraMatrix = Mat::eye(3, 3, CV_64F);
    distCoeffs = Mat::zeros(8, 1, CV_64F);
    
    vector<vector<Point3f>> _points3D(1);
    calcBoardCornerPositions(boardSize, 1.0, _points3D[0], CHESSBOARD);
    points3D = _points3D;
}

//conform CvVideoCameraDelegate
- (void)processImage:(cv::Mat&)mat
{
    try {
        vector<Point2f> cornerPoints;
        vector<vector<Point2f>> points2D;
        
        //int chessBoardFlags = CALIB_CB_ADAPTIVE_THRESH | CALIB_CB_NORMALIZE_IMAGE | CALIB_CB_FAST_CHECK;
        bool found = findChessboardCorners(mat, boardSize, cornerPoints);
        if (found && cornerPoints.size() == 20) {
            
            Mat viewGray;
            cvtColor(mat, viewGray, COLOR_BGR2GRAY);
            cornerSubPix(viewGray, cornerPoints, boardSize, Size_<int>(-1,-1), TermCriteria( CV_TERMCRIT_ITER, 30, 0.1));
            
            points2D.push_back(cornerPoints);
            points3D.resize(points2D.size(), points3D[0]);
            
            drawChessboardCorners(mat, boardSize, cornerPoints, found);
            if (intrinsicMatCalculated == NO) {
                double RMS = 0.0;
                RMS = calibrateCamera(points3D, points2D, mat.size(), cameraMatrix, distCoeffs, noArray(), noArray());
                if(RMS < 0.1 || RMS > 1.0 || !checkRange(cameraMatrix) || !checkRange(distCoeffs)){
                    return;
                }
                intrinsicMatCalculated = YES;
            }
            
            Mat R(3,1,DataType<double>::type);
            Mat T(3,1,DataType<double>::type);
            Mat Rod(3,3,DataType<double>::type);

            bool OK = solvePnP(points3D.front(), points2D.front(), cameraMatrix, distCoeffs, R, T, true, CV_EPNP);
                
            if (OK) {
                if(!checkRange(R) || !checkRange(T)) {
                    return;
                }

                //cout << "R = "<< endl << " "  << R << endl << endl;
                cout << "T = "<< endl << " "  << T << endl << endl;
                
                Rodrigues(R, Rod);
                //cout << "Rodrigues = "<< endl << " "  << r << endl << endl;

                GLKMatrix3 mat3;
                int i=0;
                for(int col=0; col<Rod.cols; col++) {
                    for(int row=0; row<Rod.rows; row++) {
                        mat3.m[i++] = (float)Rod.at<double>(row, col);
                    }
                }
                
                float scale = -2;
                GLKVector3 vec3 = GLKVector3Make(scale * (float)T.at<double>(0),
                                                 scale * (float)T.at<double>(1),
                                                 scale * (float)T.at<double>(2));
                cout << vec3.x << " " << vec3.y << " " << vec3.z << '\n';
                
                [_beViewCtl cameraReset];
                //[_beViewCtl cameraTranslate:vec3 Incremental:YES];
                [_beViewCtl cameraRotate:mat3 Incremental:YES];

                
                
            }
        }
            
    } catch (exception& e) {
        cout << e.what() << '\n';
    }
}

-(void)viewDidLoad
{
    [super viewDidLoad];

    CGRect frame = [[UIScreen mainScreen] bounds];
    _cvView = [[UIView alloc] initWithFrame:frame];

    _videoSource = [[CvVideoCamera alloc] initWithParentView:_cvView];
    _videoSource.defaultAVCaptureDevicePosition = AVCaptureDevicePositionBack;
    _videoSource.defaultAVCaptureSessionPreset  = AVCaptureSessionPresetLow; //AVCaptureSessionPresetLow;
    _videoSource.defaultFPS = 60;
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
        
        
        [_beViewCtl addModelNamed:@"2.obj"];
        
        //[be insertSubview:_cvView belowSubview:be.view];
        
        //[be insertBackgroundView:_cvView];
        
    }];
}

@end

#endif

