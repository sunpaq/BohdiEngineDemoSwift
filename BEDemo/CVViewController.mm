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
#import <opencv2/calib3d.hpp>
using namespace cv;
#endif

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
    
    CvMat* rotation_vec;
    CvMat* translation_vec;
    
    CvPoint2D32f* corners;
    
    IplImage* image;
    IplImage* grayimg;
    
    BOOL intrinsicMatCalculated;
}

@end


@implementation CVViewController

int getElementPosition(int Ncols, int Nchannels, int row, int col, int channel)
{
    return row * Ncols * Nchannels + col * Nchannels + channel;
}

-(void)prepareOpenCV
{
    n_boards = 1;
    board_n  = 9 * 6;
    board_dt = 20;
    board_sz = cvSize(9, 6);
    
    corner_count = 0;
    successes = 0;
    step  = 0;
    frame = 0;
    
    //alloc
    image_points   = cvCreateMat(n_boards * board_n, 2, CV_32FC1);
    object_points  = cvCreateMat(n_boards * board_n, 3, CV_32FC1);
    point_counts   = cvCreateMat(n_boards, 1, CV_32FC1);
    intrinsic_mat  = cvCreateMat(3, 3, CV_32FC1);
    distortion_coe = cvCreateMat(5, 1, CV_32FC1);
    
    //output buffer
    rotation_vec    = cvCreateMat(3, 3, CV_32FC1);
    translation_vec = cvCreateMat(3, 1, CV_32FC1);
    
    corners = new CvPoint2D32f[ board_n ];
    intrinsicMatCalculated = NO;
}


//conform CvVideoCameraDelegate
- (void)processImage:(cv::Mat&)mat
{
    image = new IplImage(mat);
    //grayimg = cvCreateImage(cvGetSize(image), 8, 1);
    
    successes = 0;
    while (successes < n_boards) {
        if (frame++ % board_dt == 0) {
            int found = cvFindChessboardCorners(image, board_sz, corners, &corner_count, CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FILTER_QUADS);
            cvDrawChessboardCorners(image, board_sz, corners, corner_count, found);
            
            //board found
            if (found) {
                if (corner_count == board_n) {
                    step = successes * board_n;
                    //step = board_n;
                    for (int i=step, j=0; j<board_n; ++i, ++j) {
                        CV_MAT_ELEM(*image_points,  float, i, 0) = corners[j].x;
                        CV_MAT_ELEM(*image_points,  float, i, 1) = corners[j].y;
                        CV_MAT_ELEM(*object_points, float, i, 0) = j / board_sz.width;
                        CV_MAT_ELEM(*object_points, float, i, 1) = j % board_sz.width;
                        CV_MAT_ELEM(*object_points, float, i, 2) = 0.0f;
                    }
                    CV_MAT_ELEM(*point_counts, int, successes, 0) = board_n;
                    successes++;
                }
            }
            
            //camera calibrate (intrinsic)
            if (!intrinsicMatCalculated) {
                cvCalibrateCamera2(object_points, image_points, point_counts, cvGetSize(image), intrinsic_mat, distortion_coe);
                intrinsicMatCalculated = YES;
            }
            
            //camera calibrate (ex)
            cvFindExtrinsicCameraParams2(object_points, image_points, intrinsic_mat, distortion_coe, rotation_vec, translation_vec);
        }
    }
    
    //update rotate & translate matrix
    
    
    
    

    
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

