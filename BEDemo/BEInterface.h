//
//  BEMainLoop.h
//  BEDemo
//
//  Created by YuliSun on 16/01/2017.
//  Copyright © 2017 SODEC. All rights reserved.
//

#ifndef BEMainLoop_h
#define BEMainLoop_h

void BETeardownGL();
void BESetupGL(unsigned width, unsigned height);
void BEResizeGL(unsigned width, unsigned height);
void BEUpdateGL();
void BEDrawGL();

void BEPanGesture(float x, float y);
void BEPinchGesture(float scale);

void BEAddModelNamed(const char* fname);

#endif /* BEMainLoop_h */
