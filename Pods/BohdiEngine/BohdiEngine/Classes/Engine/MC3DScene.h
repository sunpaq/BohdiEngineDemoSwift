//
//  MC3DScene.h
//  monkcGame
//
//  Created by Sun YuLi on 16/2/20.
//  Copyright © 2016年 oreisoft. All rights reserved.
//

#ifndef MC3DScene_h
#define MC3DScene_h

#include <stdio.h>
#include "monkc.h"
#include "MC3DNode.h"
#include "MCGLRenderer.h"
#include "MCCamera.h"
#include "MCClock.h"
#include "MCLight.h"
#include "MCSkybox.h"

class(MC3DScene, MCObject,
      struct MC3DSceneStruct* next;
      struct MC3DSceneStruct* prev;
      
      unsigned      scenewidth;
      unsigned      sceneheight;
      
      MCBool        skyboxShow;
      MCSkybox*     skyboxRef;
      
      MCGLRenderer* renderer;
      MC3DNode*     rootnode;
      MCCamera*     mainCamera;
      MCLight*      light;
      MCBool        cameraLock;
      MCClock*      clock;
      
      computing(MCBool, isDrawSky);
);

method(MC3DScene, void, bye, voida);
method(MC3DScene, MC3DScene*, initWithWidthHeightVSourceFSource, unsigned width, unsigned height, const char* vsource, const char* fsource);
method(MC3DScene, MC3DScene*, initWithWidthHeightVNameFName, unsigned width, unsigned height, const char* vname, const char* fname);
method(MC3DScene, MC3DScene*, initWithWidthHeightDefaultShader, unsigned width, unsigned height);
method(MC3DScene, void, updateScene, voida);

//return -1 or positive fps number
//please update your view only when it is not -1
method(MC3DScene, int, drawScene, voida);

method(MC3DScene, void, lockCamera, MCBool lock);
method(MC3DScene, MCCamera*, getCamera, voida);
method(MC3DScene, void, moveCameraOneStep, MCFloat deltaFai, MCFloat deltaTht);
method(MC3DScene, void, moveSkyboxCamera, MCFloat deltaFai, MCFloat deltaTht);
method(MC3DScene, void, printDebugInfo, voida);

#endif /* MC3DScene_h */
