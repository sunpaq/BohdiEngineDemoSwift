//
//  MCSkybox.h
//  monkcGame
//
//  Created by Sun YuLi on 16/4/13.
//  Copyright © 2016年 oreisoft. All rights reserved.
//

#ifndef MCSkybox_h
#define MCSkybox_h

#include <stdio.h>
#include "monkc.h"
#include "MC3DNode.h"
#include "BEAssetsManager.h"
#include "MCSkyboxCamera.h"


class(MCSkybox, MC3DNode,
      MCSkyboxCamera* camera;
      MCGLContext* ctx;
      
      //MCUInt pid;
      MCUInt vaoid;
      MCUInt vboid;
      MCUInt eboid;
      MCUInt texid);

method(MCSkybox, void, bye, voida);
method(MCSkybox, MCSkybox*, initWithCubeTexture, BECubeTextureData* cubetex, double widthHeightRatio);
method(MCSkybox, MCSkybox*, initWithFileNames, const char* namelist[], double widthHeightRatio);
method(MCSkybox, MCSkybox*, initWithDefaultFilesRatio, double widthHeightRatio);
method(MCSkybox, MCSkybox*, initWithDefaultFiles, voida);
method(MCSkybox, void, resizeWithWidthHeight, unsigned width, unsigned height);
//override
method(MCSkybox, void, update, MCGLContext* ctx);
method(MCSkybox, void, draw, MCGLContext* ctx);

#endif /* MCSkybox_h */
