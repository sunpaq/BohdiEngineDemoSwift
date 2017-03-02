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
using namespace cv;
#endif

@implementation CVViewController

//conform CvVideoCameraDelegate
- (void)processImage:(cv::Mat&)image
{
    
}

-(void)viewDidLoad
{
    [super viewDidLoad];

    CGRect frame = [[UIScreen mainScreen] bounds];
    self.cvView = [[UIView alloc] initWithFrame:frame];
    self.videoSource = [[CvVideoCamera alloc] initWithParentView:self.view];
    self.videoSource.defaultAVCaptureDevicePosition = AVCaptureDevicePositionBack;
    self.videoSource.defaultAVCaptureSessionPreset  = AVCaptureSessionPresetHigh;
    self.videoSource.defaultFPS = 60;
    self.videoSource.delegate = self;
    
    [self startOpenCV];
}

-(void)startOpenCV
{
    [self.view addSubview:self.cvView];
    [self.videoSource start];
}


@end

#endif

