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
#import <opencv2/highgui.hpp>
#endif

using namespace cv;
using namespace std;

@interface CVViewController()
{
    int n_boards;
    int board_n;
    int board_dt;
    CvSize board_sz;
    
    int corner_count;
    int successes;
    int step;
    int frame;

    CvMat* image_points;
    CvMat* object_points;
    CvMat* point_counts;
    CvMat* intrinsic_mat;
    CvMat* distortion_coe;
    
    CvMat* object_points2;
    CvMat* image_points2;
    CvMat* point_counts2;
    
    CvMat* rotation_vec;
    CvMat* translation_vec;
    
    CvPoint2D32f* corners;
    
    IplImage* image;
    IplImage* grayimg;
    
    BOOL intrinsicMatCalculated;
    
    enum Pattern { NOT_EXISTING, CHESSBOARD, CIRCLES_GRID, ASYMMETRIC_CIRCLES_GRID };
    
    Mat cameraMatrix;
    Mat distCoeffs;
    
    //vector<vector<Point3f>> points3D;
}

@end


@implementation CVViewController

int getElementPosition(int Ncols, int Nchannels, int row, int col, int channel)
{
    return row * Ncols * Nchannels + col * Nchannels + channel;
}

-(void)prepareOpenCV_CAPI
{
    n_boards = 1;
    board_n  = 9 * 6;
    board_dt = 1;
    board_sz = cvSize(9, 6);
    
    corner_count = 0;
    successes = 0;
    step  = 0;
    frame = 0;
    
    //alloc
    image_points   = cvCreateMat(n_boards * board_n, 2, CV_32FC1);
    object_points  = cvCreateMat(n_boards * board_n, 3, CV_32FC1);
    point_counts   = cvCreateMat(n_boards, 1, CV_32SC1);//Mx1
    intrinsic_mat  = cvCreateMat(3, 3, CV_32FC1);
    distortion_coe = cvCreateMat(5, 1, CV_32FC1);
    
    //output buffer
    rotation_vec    = cvCreateMat(1, 3, CV_32FC1);
    translation_vec = cvCreateMat(1, 3, CV_32FC1);
    
    corners = new CvPoint2D32f[ board_n ];
    intrinsicMatCalculated = NO;
}



- (void)processImageUseCAPI:(cv::Mat&)mat
{
    image = new IplImage(mat);
    grayimg = cvCreateImage(cvGetSize(image), 8, 1);
    
    successes = 0;
    //while (successes < n_boards) {
    if (frame++ % board_dt == 0) {
        int found = cvFindChessboardCorners(image, board_sz, corners, &corner_count,
                                            CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FILTER_QUADS);
        
        //get sub pixel
        cvCvtColor(image, grayimg, CV_BGR2GRAY);
        cvFindCornerSubPix(grayimg, corners, corner_count,
                           cvSize(11, 11),
                           cvSize(-1, -1),
                           cvTermCriteria(CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 30, 0.1));
        
        cvDrawChessboardCorners(image, board_sz, corners, corner_count, found);
        
        //board found
        if (found) {
            if (corner_count == board_n) {
                step = successes * board_n;
                //step = board_n;
                for (int i=step, j=0; j<board_n; ++i, ++j) {
                    //2D
                    CV_MAT_ELEM(*image_points,  float, i, 0) = corners[j].x;
                    CV_MAT_ELEM(*image_points,  float, i, 1) = corners[j].y;
                    //3D
                    CV_MAT_ELEM(*object_points, float, i, 0) = j / board_sz.width;
                    CV_MAT_ELEM(*object_points, float, i, 1) = j % board_sz.width;
                    CV_MAT_ELEM(*object_points, float, i, 2) = 0.0f;
                }
                CV_MAT_ELEM(*point_counts, int, successes, 0) = board_n;
                successes++;
                
                //new alloc
                object_points2  = cvCreateMat(successes * board_n, 3, CV_32FC1);
                image_points2   = cvCreateMat(successes * board_n, 2, CV_32FC1);
                point_counts2   = cvCreateMat(successes, 1, CV_32SC1);//Mx1
                
                //transfer into correct size matrices
                for (int i=0; i<successes * board_n; ++i) {
                    //2D
                    CV_MAT_ELEM(*image_points2, float, i, 0) = CV_MAT_ELEM(*image_points, float, i, 0);
                    CV_MAT_ELEM(*image_points2, float, i, 1) = CV_MAT_ELEM(*image_points, float, i, 1);
                    //3D
                    CV_MAT_ELEM(*object_points2, float, i, 0) = CV_MAT_ELEM(*object_points, float, i, 0);
                    CV_MAT_ELEM(*object_points2, float, i, 1) = CV_MAT_ELEM(*object_points, float, i, 1);
                    CV_MAT_ELEM(*object_points2, float, i, 2) = CV_MAT_ELEM(*object_points, float, i, 2);
                }
                for (int i=0; i<successes; ++i) {
                    CV_MAT_ELEM(*point_counts2, int, i, 0) = CV_MAT_ELEM(*point_counts, int, i, 0);
                }
                cvReleaseMat(&object_points);
                cvReleaseMat(&image_points);
                cvReleaseMat(&point_counts);
                
                //init intrinsic matrix
                CV_MAT_ELEM(*intrinsic_mat, float, 0, 0) = 1.0f;
                CV_MAT_ELEM(*intrinsic_mat, float, 1, 1) = 1.0f;
                
                //camera calibrate (intrinsic)
                if (intrinsicMatCalculated == NO) {
                    try {
                        cvCalibrateCamera2(object_points2, image_points2,
                                           point_counts2, cvGetSize(image),
                                           intrinsic_mat, distortion_coe,
                                           rotation_vec, translation_vec, 0);
                        
                        
                    }
                    catch (cv::Exception& e) {
                        
                    }
                    
                    intrinsicMatCalculated = YES;
                }
                
                //camera calibrate (ex)
                //                    cvFindExtrinsicCameraParams2(object_points, image_points,
                //                                                 intrinsic_mat, distortion_coe,
                //                                                 rotation_vec, translation_vec);
                
            }
        }
        
        
        
        
    }
    //}
    
    //update rotate & translate matrix
}

static void calcBoardCornerPositions(Size_<int> boardSize, float squareSize, vector<Point3f>& corners, Pattern patternType)
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

-(void)prepareOpenCV
{
    intrinsicMatCalculated = NO;

    cameraMatrix = Mat::eye(3, 3, CV_64F);
    distCoeffs = Mat::zeros(8, 1, CV_64F);
    
    Size_<int> boardSize = Size_<int>(4, 5);
    
//    vector<vector<Point3f>> _points3D(1);
//    calcBoardCornerPositions(boardSize, 1.0, _points3D[0], CHESSBOARD);
//    points3D = _points3D;
    
    frame = 0;
}

void cameraPoseFromHomography(const Mat& H, Mat& pose)
{
    pose = Mat::eye(3, 4, CV_32FC1);      // 3x4 matrix, the camera pose
    float norm1 = (float)norm(H.col(0));
    float norm2 = (float)norm(H.col(1));
    float tnorm = (norm1 + norm2) / 2.0f; // Normalization value
    
    Mat p1 = H.col(0);       // Pointer to first column of H
    Mat p2 = pose.col(0);    // Pointer to first column of pose (empty)
    
    cv::normalize(p1, p2);   // Normalize the rotation, and copies the column to pose
    
    p1 = H.col(1);           // Pointer to second column of H
    p2 = pose.col(1);        // Pointer to second column of pose (empty)
    
    cv::normalize(p1, p2);   // Normalize the rotation and copies the column to pose
    
    p1 = pose.col(0);
    p2 = pose.col(1);
    
    Mat p3 = p1.cross(p2);   // Computes the cross-product of p1 and p2
    Mat c2 = pose.col(2);    // Pointer to third column of pose
    p3.copyTo(c2);       // Third column is the crossproduct of columns one and two
    
    pose.col(3) = H.col(2) / tnorm;  //vector t [R|t] is the last column of pose
}

//conform CvVideoCameraDelegate
- (void)processImage:(cv::Mat&)mat
{
//    if (frame++ % 20 != 0) {
//        return;
//    }
    
    Size_<int> imageSize = mat.size();
    Size_<int> boardSize = Size_<int>(4, 5);
    
    vector<Point2f> markerPoints;
    vector<Point2f> cornerPoints;
    vector<vector<Point2f>> points2D;
    
    vector<vector<Point3f>> points3D(1);
    calcBoardCornerPositions(boardSize, 0.025, points3D[0], CHESSBOARD);

    //int chessBoardFlags = CALIB_CB_ADAPTIVE_THRESH | CALIB_CB_NORMALIZE_IMAGE | CALIB_CB_FAST_CHECK;
    //int chessBoardFlags = CALIB_CB_ADAPTIVE_THRESH | CALIB_CB_NORMALIZE_IMAGE;
    bool found = findChessboardCorners(mat, boardSize, cornerPoints);
    if (found && cornerPoints.size() == 20) {

        Mat viewGray;
        cvtColor(mat, viewGray, COLOR_BGR2GRAY);
        cornerSubPix( viewGray, cornerPoints, boardSize,
        Size_<int>(-1,-1), TermCriteria( CV_TERMCRIT_ITER, 30, 0.1 ));
        
        points2D.push_back(cornerPoints);
        points3D.resize(points2D.size(), points3D[0]);
        
        drawChessboardCorners(mat, boardSize, cornerPoints, found);

        
        //Mat rvec(3,1,DataType<float>::type);
        //Mat tvec(3,1,DataType<float>::type);
        
        vector<Mat> rvecs;
        vector<Mat> tvecs;
        try {
            if (intrinsicMatCalculated == NO) {
                double RMS = 0.0;
                RMS = calibrateCamera(points3D, points2D, imageSize, cameraMatrix, distCoeffs, noArray(), noArray());
                if(RMS < 0.1 || RMS > 1.0){
                    return;
                }
                intrinsicMatCalculated = YES;
            }
            
            
//            float squareSize = 0.025;
//            for( int i = 0; i < boardSize.height; ++i )
//                for( int j = 0; j < boardSize.width; ++j )
//                    markerPoints.push_back(Point2f(j*squareSize, i*squareSize));
//            
//            Mat H = findHomography(markerPoints, cornerPoints);
//            if (H.empty()) {
//                return;
//            }
//            
//            //Mat pose;//3x4
//            //cameraPoseFromHomography(H, pose);
//            
//            
//            vector<Mat> rotations;
//            vector<Mat> translations;
//            vector<Mat> normals;
//            decomposeHomographyMat(H, cameraMatrix, rotations, translations, normals);
            
//            Mat R = rotations[2];
//            Mat T = translations.front();
            
            Mat rvec(3,1,DataType<double>::type);
            Mat tvec(3,1,DataType<double>::type);
            bool OK = solvePnP(points3D.front(), points2D.front(), cameraMatrix, distCoeffs, rvec, tvec, true, CV_EPNP);

            if (OK) {
                Mat R = rvec;
                Mat T = tvec;
                
                cout << "R = "<< endl << " "  << R << endl << endl;
                cout << "T = "<< endl << " "  << T << endl << endl;
                
                Mat r(3,3,DataType<double>::type);
                Rodrigues(R, r);
                cout << "Rodrigues = "<< endl << " "  << r << endl << endl;
            
            
                GLKMatrix3 mat3;
                int i=0;
                for(int col=0; col<r.cols; col++) {
                    for(int row=0; row<r.rows; row++) {
                        mat3.m[i++] = r.at<double>(row, col);
                    }
                }
            
//                if(mat3.m00 == 1 && mat3.m11 == 1 && mat3.m22 == 1) {
//                    return;
//                }
            
                GLKMatrix4 camTransform;
                camTransform.m00 = mat3.m00;
                camTransform.m01 = mat3.m01;
                camTransform.m02 = mat3.m02;
                camTransform.m03 = 0;
    
                camTransform.m10 = mat3.m10;
                camTransform.m11 = mat3.m11;
                camTransform.m12 = mat3.m12;
                camTransform.m13 = 0;
    
                camTransform.m20 = mat3.m20;
                camTransform.m21 = mat3.m21;
                camTransform.m22 = mat3.m22;
                camTransform.m23 = 0;
    
                camTransform.m30 = 0;
                camTransform.m31 = 0;
                camTransform.m32 = 0;
                camTransform.m33 = 1;
                
//                float x = *(R.ptr(0, 0));
//                float y = *(R.ptr(1, 0));
//                float z = *(R.ptr(2, 0));
//                float tht = GLKVector3Length(GLKVector3Make(x, y, z));
//                printf("x/y/z/tht -> %f/%f/%f/%f\n", x, y, z, tht);
//                
//                float tx = *(T.ptr(0, 0));
//                float ty = *(T.ptr(1, 0));
//                float tz = *(T.ptr(2, 0));
//                printf("tx/ty/tz -> %f/%f/%f\n", tx, ty, tz);
                
                //GLKMatrix4 camTransform = GLKMatrix4Identity;
                //camTransform = GLKMatrix4Translate(camTransform, -tx, -ty, -tz);
                //camTransform = GLKMatrix4Rotate(camTransform, tht, x, y, z);
            
            
            
//            GLKMatrix3 mat3;
//            int i=0;
//            for(int col=0; col<R.cols; col++) {
//                for(int row=0; row<R.rows; row++) {
//                    mat3.m[i++] = R.at<float>(row, col);
//                }
//            }
//
//
//            
//            GLKMatrix4 camTransform;
//            camTransform.m00 = mat3.m00;
//            camTransform.m01 = mat3.m01;
//            camTransform.m02 = mat3.m02;
//            camTransform.m03 = 0;
//
//            camTransform.m10 = mat3.m10;
//            camTransform.m11 = mat3.m11;
//            camTransform.m12 = mat3.m12;
//            camTransform.m13 = 0;
//
//            camTransform.m20 = mat3.m20;
//            camTransform.m21 = mat3.m21;
//            camTransform.m22 = mat3.m22;
//            camTransform.m23 = 0;
//
//            camTransform.m30 = 0;
//            camTransform.m31 = 0;
//            camTransform.m32 = 0;
//            camTransform.m33 = 1;
            
            
                [_beViewCtl setCameraTransfrom:camTransform];
            }
            //else {
            
            //}

        }
        catch (exception e) {
        
        }
        
    }
    
}

-(void)viewDidLoad
{
    [super viewDidLoad];

    CGRect frame = [[UIScreen mainScreen] bounds];
    _cvView = [[UIView alloc] initWithFrame:frame];

    _videoSource = [[CvVideoCamera alloc] initWithParentView:_cvView];
    _videoSource.defaultAVCaptureDevicePosition = AVCaptureDevicePositionBack;
    _videoSource.defaultAVCaptureSessionPreset  = AVCaptureSessionPresetLow;
    _videoSource.defaultFPS = 30;
    _videoSource.delegate = self;
    
    [self.view addSubview:_cvView];

    _beViewCtl = [[BEViewController alloc] init];
    [_beViewCtl setTransparentBG];

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

