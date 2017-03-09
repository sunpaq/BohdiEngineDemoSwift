//
//  GLView.m
//  BEDemo
//
//  Created by YuliSun on 02/03/2017.
//  Copyright © 2017 SODEC. All rights reserved.
//

#import "BEViewController.h"

#import <Foundation/Foundation.h>
#import <BohdiEngine/MCDirector.h>
#import <BohdiEngine/MCGLEngine.h>
#import <BEDemo-Swift.h>

@interface BEViewController()
{
    MCDirector* director;
    float pinch_scale;
}
@end

@implementation BEViewController

@dynamic deviceRotateMat3;

//-(instancetype)initWithFrame:(CGRect)frame
//{
//    if (self=[super initWithFrame:frame]) {
//        [self setupBE];
//        return self;
//    }
//    return nil;
//}
//
//-(instancetype)initWithCoder:(NSCoder *)aDecoder
//{
//    if (self=[super initWithCoder:aDecoder]) {
//        [self setupBE];
//        return self;
//    }
//    return nil;
//}

-(void)viewDidLoad
{
    [super viewDidLoad];
    [EAGLContext setCurrentContext:[[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES3]];

    CGRect frame = self.view.frame;
    director = new(MCDirector);
    pinch_scale = 10.0;
    
    _indicator = nil;
    GLKView* _glView = (GLKView*)self.view;
    _glView.context = [EAGLContext currentContext];
    //_glView = [[GLKView alloc] initWithFrame:frame context:[EAGLContext currentContext]];
    _glView.delegate = self;
    ((GLKView*)self.view).delegate = self;
    
    _glView.drawableColorFormat = GLKViewDrawableColorFormatRGBA8888;
    _glView.drawableDepthFormat = GLKViewDrawableDepthFormat24;
    _glView.drawableStencilFormat = GLKViewDrawableStencilFormat8;
    _glView.drawableMultisample = GLKViewDrawableMultisampleNone;
    
    self.preferredFramesPerSecond = 60;
    //[self.view addSubview:_glView];
    [self setupBE];
}

-(void)dealloc
{
    release(director);
}

-(void)glkView:(GLKView *)view drawInRect:(CGRect)rect
{
    ff(director, updateAll, 0);
    ff(director, drawAll, 0);
}

-(void)setupBE
{
    CGRect frame = self.view.frame;
    unsigned width = frame.size.width;
    unsigned height = frame.size.height;
    ff(director, setupMainScene, width, height);
    ff(director, setCameraRotateMode, MCCameraRotateAR);
}

-(void) setViewFrame:(CGRect)frame
{
    self.view.frame = frame;
    unsigned width = frame.size.width;
    unsigned height = frame.size.height;
    [self resizeGL:width height:height];
}

-(void) setTransparentBG
{
    self.view.opaque = NO;
    self.view.backgroundColor = [UIColor clearColor];
    self.modalPresentationStyle = UIModalPresentationOverCurrentContext;
    self.navigationController.modalPresentationStyle = UIModalPresentationOverCurrentContext;
    
    //MCGLEngine_enableTransparency(true);
    MCGLEngine_setClearScreenColor((MCColorf){0.0,0.0,0.0,0.0});
}

//-(void) insertBackgroundView:(UIView*)bgview
//{
//    [self.view insertSubview:bgview belowSubview:_glView];
//}
//
//-(void) insertOverlayView:(UIView*)overlay
//{
//    [self.view insertSubview:overlay aboveSubview:_glView];
//}

-(void) resizeGL:(unsigned)width height:(unsigned)height
{
    ff(director, resizeAllScene, width, height);
}

-(void) setDeviceRotateMat3:(CMRotationMatrix)mat3
{
    director->deviceRotationMat3.m00 = mat3.m11;
    director->deviceRotationMat3.m01 = mat3.m12;
    director->deviceRotationMat3.m02 = mat3.m13;
    
    director->deviceRotationMat3.m10 = mat3.m21;
    director->deviceRotationMat3.m11 = mat3.m22;
    director->deviceRotationMat3.m12 = mat3.m23;
    
    director->deviceRotationMat3.m20 = mat3.m31;
    director->deviceRotationMat3.m21 = mat3.m32;
    director->deviceRotationMat3.m22 = mat3.m33;
}

-(void) setRotateMat3:(GLKMatrix3)mat3
{
    
}

-(void) setTranslateMat3:(GLKMatrix3)mat3
{

}

-(void) startLoadingAnimation
{
    if (!_indicator) {
        _indicator = [[UIActivityIndicatorView alloc] initWithActivityIndicatorStyle:UIActivityIndicatorViewStyleWhiteLarge];
    }
    _indicator.center = self.view.center;
    [self.view addSubview:_indicator];
    [_indicator startAnimating];
}

-(void) stopLoadingAnimation
{
    dispatch_async(dispatch_get_main_queue(), ^{
        if (_indicator) {
            [_indicator stopAnimating];
            _indicator = nil;
        }
    });
}

-(void) addModelNamed:(NSString*)modelName
{
    [self startLoadingAnimation];
    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_HIGH, 0), ^{
        const char* name = [modelName cStringUsingEncoding:NSUTF8StringEncoding];
        ff(director, addModelNamed, name);
        
        [self stopLoadingAnimation];
    });
}

-(void) setRotateCamera:(BOOL)doesRotate
{
    computed(director, cameraHandler)->isLockRotation = doesRotate? false : true;
}

-(void) setDrawWireFrame:(BOOL)doesDrawWF
{
    computed(director, contextHandler)->drawMode = doesDrawWF ? MCLineStrip : MCTriAngles;
}

-(void) handlePanGesture:(CGPoint)offset
{
    float x = offset.x;
    float y = offset.y;
    computed(director, cameraHandler)->tht += (y/16.0);
    computed(director, cameraHandler)->fai += -(x/9.0);
}

-(void) handlePinchGesture:(float)scale
{
    pinch_scale *= scale;
    pinch_scale = MAX(10.0, MIN(pinch_scale, 100.0));
    
    MCCamera* camera = computed(director, cameraHandler);
    MCCamera_distanceScale(0, camera, MCFloatF(20.0/pinch_scale));
}

@end
