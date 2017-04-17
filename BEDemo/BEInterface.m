//
//  BEMainLoop.c
//  BEDemo
//
//  Created by YuliSun on 16/01/2017.
//  Copyright © 2017 SODEC. All rights reserved.
//

#import "BEInterface.h"
#import "MCDirector.h"
#import <BohdiEngine/MC3DAxis.h>
#import <BohdiEngine/MCCube.h>

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
    //MC3DAxis* axis = new(MC3DAxis);
    //MCDirector_addNode(0, director, (MC3DNode*)axis);
    MCCube* cube = new(MCCube);
    MCDirector_addNode(0, director, (MC3DNode*)cube);
    
    ff(director, addModelNamed, fname);
    MC3DModel* model = (MC3DModel*)director->lastScene->rootnode->children->headItem->nextItem;
    
    //ff(director, moveModelToOrigin, model);
    //ff(director, cameraFocusOnModel, model);
    ff(director, cameraZoomToFitModel, model);
    computed(director, cameraHandler)->lookat.y -= 5;
}

void BESetDoesRotateCamera(_Bool doesRotate)
{
    computed(director, cameraHandler)->isLockRotation = doesRotate? false : true;
}

void BESetDoesDrawWireFrame(_Bool doesDrawWF)
{
    computed(director, contextHandler)->drawMode = doesDrawWF ? GL_LINE_STRIP : GL_TRIANGLES;
}
