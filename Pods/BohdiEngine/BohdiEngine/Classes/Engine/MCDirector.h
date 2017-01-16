//
//  MCDirector.h
//  monkcGame
//
//  Created by Sun YuLi on 16/3/19.
//  Copyright © 2016年 oreisoft. All rights reserved.
//

#ifndef MCDirector_h
#define MCDirector_h

#include <stdio.h>
#include "monkc.h"
#include "MC3DScene.h"
#include "MC3DModel.h"
#include "MCThread.h"

class(MCDirector, MCObject,
      MC3DScene* lastScene;
      
      MCThread* skyboxThread;
      MCThread* modelThread;
      
      int currentWidth;
      int currentHeight;
      
      computing(MCCamera*, cameraHandler);
);

method(MCDirector, void, bye, voida);
method(MCDirector, void, updateAll, voida);

//return -1 or positive fps number
//please update your view only when it is not -1
method(MCDirector, int, drawAll, voida);

method(MCDirector, void, setupMainScene, unsigned width, unsigned height);
method(MCDirector, void, pushScene, MC3DScene* scene);
method(MCDirector, void, popScene, voida);
method(MCDirector, void, resizeAllScene, int width, int height);

method(MCDirector, void, addModel, MC3DModel* model);
method(MCDirector, void, addModelNamed, const char* name);

method(MCDirector, void, cameraFocusOn, MCVector3 vertex);
method(MCDirector, void, cameraFocusOnModel, MC3DModel* model);
method(MCDirector, void, printDebugInfo, voida);

//iOS callbacks
//void onRootViewLoad(void* rootview);
//void onOpenExternalFile(const char* filepath);
//void onReceiveMemoryWarning();
//void onSetupGL(unsigned windowWidth, unsigned windowHeight, const char** filePathArray);
//void onTearDownGL();
//void onUpdate();
//void onDraw();

#endif /* MCDirector_h */
