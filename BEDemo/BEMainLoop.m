//
//  BEMainLoop.c
//  BEDemo
//
//  Created by YuliSun on 16/01/2017.
//  Copyright © 2017 SODEC. All rights reserved.
//

#import "BEMainLoop.h"
#import <BohdiEngine/BohdiEngine-umbrella.h>

static MCDirector* director = null;
void BESetupGL(unsigned width, unsigned height)
{
    if (!director) {
        director = new(MCDirector);
    }
    
    ff(director, setupMainScene, width, height);
    
    dispatch_async(dispatch_queue_create("BEEngineQueue", null), ^{
        ff(director, addModelNamed, "2");
    });
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


