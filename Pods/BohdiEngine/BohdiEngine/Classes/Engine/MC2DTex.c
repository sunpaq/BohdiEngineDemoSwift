//
//  MC2DTex.c
//  monkcGame
//
//  Created by Sun YuLi on 16/4/13.
//  Copyright © 2016年 oreisoft. All rights reserved.
//

#include "MC2DTex.h"
#include "MCGLEngine.h"

oninit(MC2DTex)
{
    if (init(MCTexture)) {
        return obj;
    }else{
        return null;
    }
}

method(MC2DTex, MC2DTex*, initWithFileName, const char* name)
{
    return obj;
}

method(MC2DTex, void, drawTexture, MCGLContext* ctx)
{
    MCGLEngine_activeTextureUnit(svar(textureUnit));
    MCGLEngine_bind2DTexture(svar(Id));
}

onload(MC2DTex)
{
    if (load(MCTexture)) {
        
        binding(MC2DTex, MC2DTex*, initWithFileName, const char* name);
        binding(MC2DTex, void, drawTexture, MCGLContext* ctx);
        
        return cla;
    }else{
        return null;
    }
}
