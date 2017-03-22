//
//  BEMainLoop.c
//  BEDemo
//
//  Created by YuliSun on 16/01/2017.
//  Copyright © 2017 SODEC. All rights reserved.
//

#import "BEInterface.h"
#import "MCDirector.h"

static MCDirector* director = null;
static float pinch_scale = 10.0;

void BETeardownGL()
{
    if (director) {
        release(director);
        director = null;
    }
}

void BESetupGL(unsigned width, unsigned height)
{
    if (!director) {
        director = new(MCDirector);
    }
    
    ff(director, setupMainScene, width, height);
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

void BEAddModelNamed(const char* fname)
{
    ff(director, addModelNamed, fname);
    ff(director, cameraFocusOnModel, director->lastScene->rootnode->children->headItem);
}

