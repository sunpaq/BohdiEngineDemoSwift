//
//  BEMainLoop.h
//  BEDemo
//
//  Created by YuliSun on 16/01/2017.
//  Copyright © 2017 SODEC. All rights reserved.
//

#ifndef BEMainLoop_h
#define BEMainLoop_h

void BESetupGL(unsigned width, unsigned height);
void BEResizeGL(unsigned width, unsigned height);
void BEUpdateGL();
void BEDrawGL();

void BEPullRotateMatrix(double m11, double m12, double m13,
                        double m21, double m22, double m23,
                        double m31, double m32, double m33);

void BEAddModelNamed(const char* name);

void BEPanGesture(float x, float y);
void BEPinchGesture(float scale);

void BESetRotateCamera(_Bool doesRotate);
void BESetWireFrameMode(_Bool wireMode);

#endif /* BEMainLoop_h */
