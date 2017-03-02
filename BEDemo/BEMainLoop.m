//
//  BEMainLoop.c
//  BEDemo
//
//  Created by YuliSun on 16/01/2017.
//  Copyright © 2017 SODEC. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <BohdiEngine/MCDirector.h>
#import <BEDemo-Swift.h>
#import "BEMainLoop.h"

static MCDirector* director = null;
static float pinch_scale = 10.0;

void BESetupGL(unsigned width, unsigned height)
{
    if (!director) {
        director = new(MCDirector);
    }
    
    ff(director, setupMainScene, width, height);
    ff(director, setCameraRotateMode, MCCameraRotateAroundModelByGyroscope);
}

void BEResizeGL(unsigned width, unsigned height)
{
    if (director) {
        ff(director, resizeAllScene, width, height);
    }
}

void BEUpdateGL()
{
    if (director) {
        ff(director, updateAll, 0);
    }
}

void BEDrawGL()
{
    if (director) {
        ff(director, drawAll, 0);
    }
}

void BEPullRotateMatrix(double m11, double m12, double m13,
                        double m21, double m22, double m23,
                        double m31, double m32, double m33)
{
    if (director) {
        director->deviceRotationMat3.m00 = m11;
        director->deviceRotationMat3.m01 = m12;
        director->deviceRotationMat3.m02 = m13;

        director->deviceRotationMat3.m10 = m21;
        director->deviceRotationMat3.m11 = m22;
        director->deviceRotationMat3.m12 = m23;

        director->deviceRotationMat3.m20 = m31;
        director->deviceRotationMat3.m21 = m32;
        director->deviceRotationMat3.m22 = m33;
    }
}

void BEAddModelNamed(const char* name)
{
    ff(director, addModelNamed, name);
}

void BEPanGesture(float x, float y)
{
    if (director) {
        computed(director, cameraHandler)->tht += (y/16.0);
        computed(director, cameraHandler)->fai += -(x/9.0);
    }
}

void BEPinchGesture(float scale)
{
    pinch_scale *= scale;
    pinch_scale = MAX(10.0, MIN(pinch_scale, 100.0));
    
    MCCamera* camera = computed(director, cameraHandler);
    if (director) {
        MCCamera_distanceScale(0, camera, MCFloatF(20.0/pinch_scale));
    }
}

void BESetRotateCamera(_Bool doesRotate)
{
    if (director) {
        computed(director, cameraHandler)->isLockRotation = !doesRotate;
    }
}

void BESetWireFrameMode(_Bool wireMode)
{
    if (director) {
        computed(director, contextHandler)->drawMode = wireMode ? MCLineStrip : MCTriAngles;
    }
}

