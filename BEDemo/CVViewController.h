//
//  CVBridge.h
//  BEDemo
//
//  Created by YuliSun on 01/03/2017.
//  Copyright © 2017 SODEC. All rights reserved.
//

#ifndef CVBridge_h
#define CVBridge_h

#import <UIKit/UIKit.h>
#import <Foundation/Foundation.h>
#import <opencv2/videoio/cap_ios.h>

#import "BEViewController.h"

@interface CVViewController : UIViewController <CvVideoCameraDelegate>
@property (nonatomic, strong) CvVideoCamera* videoSource;
@property (nonatomic, strong) UIView* cvView;
@property (nonatomic, strong) BEViewController* beViewCtl;

@end

#endif /* CVBridge_h */
