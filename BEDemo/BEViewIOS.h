//
//  GLView.h
//  BEDemo
//
//  Created by YuliSun on 02/03/2017.
//  Copyright © 2017 SODEC. All rights reserved.
//

#ifndef BEViewIOS_h
#define BEViewIOS_h

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#import <GLKit/GLKit.h>
#import <CoreMotion/CoreMotion.h>

@interface BEViewIOS : GLKViewController <GLKViewDelegate>

@property (atomic) CMRotationMatrix deviceRotateMat3;
@property (nonatomic, strong) UIActivityIndicatorView* indicator;

-(void) addModelNamed:(NSString*)modelName;
-(void) setRotateCamera:(BOOL)doesRotate;
-(void) setDrawWireFrame:(BOOL)doesDrawWF;

-(void) setViewFrame:(CGRect)frame;
-(void) setTransparentBG;
-(void) insertBackgroundView:(UIView*)bgview;
-(void) insertOverlayView:(UIView*)overlay;
@end


#endif /* BEViewIOS_h */
