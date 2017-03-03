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
#import <opencv2/calib3d/calib3d_c.h>
using namespace cv;
#endif

@interface CVViewController()
{
    CvPoint2D32f* conrners;
}

@end


@implementation CVViewController

//conform CvVideoCameraDelegate
- (void)processImage:(cv::Mat&)image
{
    
    
    //if(cvFindChessboardCorners(image.data[0], cvSize(9, 6), conrners)) {
        //cvDrawChessboardCorners(&image, cvSize(9, 6), conrners, 54, 1);

    //}
    
    
}

-(void)viewDidLoad
{
    [super viewDidLoad];

    CGRect frame = [[UIScreen mainScreen] bounds];
    _cvView = [[UIView alloc] initWithFrame:frame];

    _videoSource = [[CvVideoCamera alloc] initWithParentView:_cvView];
    _videoSource.defaultAVCaptureDevicePosition = AVCaptureDevicePositionBack;
    _videoSource.defaultAVCaptureSessionPreset  = AVCaptureSessionPresetHigh;
    _videoSource.defaultFPS = 60;
    _videoSource.delegate = self;
    
    [self.view addSubview:_cvView];

    _beViewCtl = [[BEViewIOS alloc] init];
    [_beViewCtl setTransparentBG];

    
    conrners = (CvPoint2D32f*)malloc(sizeof(CvPoint2D32f) * 70);

    

}

-(void)viewDidAppear:(BOOL)animated
{
    //start openCV
    [self.videoSource start];
    
    [self presentViewController:_beViewCtl animated:NO completion:^{
        
        
        [_beViewCtl addModelNamed:@"2.obj"];
        
        //[be insertSubview:_cvView belowSubview:be.view];
        
        //[be insertBackgroundView:_cvView];
        
    }];
}

@end

#endif

