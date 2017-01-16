//
//  MC3DModel.h
//  monkcGame
//
//  Created by Sun YuLi on 16/3/14.
//  Copyright © 2016年 oreisoft. All rights reserved.
//

#ifndef MC3DModel_h
#define MC3DModel_h

#include <stdio.h>
#include "monkc.h"
#include "MCGLBase.h"
#include "MCMesh.h"
#include "MCTexture.h"
#include "MCMatrial.h"
#include "MCIO.h"
#include "MC3DNode.h"
#include "MCMath.h"

class(MC3DModel, MC3DNode,
      char name[256];
      //char mtl[256];

      MCColorf defaultColor;
      const char* defaultExtension;
      MCBool textureOnOff;
      
      MC3DFrame lastSavedFrame;
      computing(MC3DFrame, frame);
);

method(MC3DModel, void, bye, voida);
method(MC3DModel, MC3DModel*, initWithFilePath, const char* path);
method(MC3DModel, MC3DModel*, initWithFileName, const char* name);
method(MC3DModel, MC3DModel*, initWithFilePathColor, const char* path, MCColorf color);
method(MC3DModel, MC3DModel*, initWithFileNameColor, const char* name, MCColorf color);

#endif /* MC3DModel_h */
