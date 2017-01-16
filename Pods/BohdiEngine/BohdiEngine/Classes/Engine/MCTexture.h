//
//  MCTexture.h
//  monkcGame
//
//  Created by Sun YuLi on 16/2/20.
//  Copyright © 2016年 oreisoft. All rights reserved.
//

#ifndef MCTexture_h
#define MCTexture_h

#include <stdio.h>
#include "monkc.h"
#include "MCGLBase.h"
#include "MCGLContext.h"
#include "BEAssetsManager.h"

class(MCTexture, MCObject,
      GLuint Id;
      GLenum textureUnit;
      BE2DTextureData* rawdata;
      int width;
      int height;
);

method(MCTexture, MCTexture*, initWithFileName, const char* name);
method(MCTexture, void, drawTexture, MCGLContext* ctx);

#endif /* MCTexture_h */
