//
//  MCGLShader.c
//  monkcGame
//
//  Created by SunYuLi on 15/11/22.
//  Copyright © 2015年 oreisoft. All rights reserved.
//

#include "MCGLRenderer.h"
#include "MCGLEngine.h"
#include "MC3DBase.h"
#include "MCIO.h"

static MCHash _draw;
static MCHash _update;

static void prehash()
{
    _draw = hash("draw");
    _update = hash("update");
}

oninit(MCGLRenderer)
{
    if(init(MCObject)){
        MCGLEngine_featureSwith(MCGLDepthTest, true);
        MCGLEngine_setClearScreenColor((MCColorf){0.05, 0.25, 0.35, 1.0});
        MCGLEngine_featureSwith(MCGLCullFace, true);
        MCGLEngine_cullFace(MCGLBack);
        MCGLEngine_setFrontCounterClockWise(true);//CCW

        obj->context = new(MCGLContext);

        return obj;
    }else{
        return null;
    }
}

method(MCGLRenderer, void, bye, voida)
{
    release(obj->context);
    superbye(MCObject);
}

method(MCGLRenderer, MCGLRenderer*, initWithShaderCodeString, const char* vcode, const char* fcode)
{
    MCGLContext_initWithShaderCode(0, obj->context, vcode, fcode,
        (const char* []){
            "position",
            "normal",
            "color",
            "texcoord"
        }, 4,
        (MCGLUniformType []){
            MCGLUniformMat4,
            MCGLUniformMat4,
            MCGLUniformVec3,
            
            MCGLUniformMat4,
            MCGLUniformMat3,
            
            MCGLUniformVec3,
            MCGLUniformVec3,
            MCGLUniformVec3,
            MCGLUniformVec3,
            MCGLUniformVec3,

            MCGLUniformVec3,
            MCGLUniformVec3,
            MCGLUniformVec3,
            MCGLUniformVec1,
            MCGLUniformVec1,
            
            MCGLUniformScalar,
        },
        (const char* []){
            view_view,
            view_projection,
            view_position,
            
            model_model,
            model_normal,
            
            light_ambient,
            light_diffuse,
            light_specular,
            light_color,
            light_position,
            
            material_ambient,
            material_diffuse,
            material_specular,
            material_dissolve,
            material_shininess,
            
            texsampler
        }, 16);
    return obj;
}

method(MCGLRenderer, MCGLRenderer*, initWithShaderFileName, const char* vshader, const char* fshader)
{
    char path[LINE_MAX];
    MCFileGetPath(vshader, "vsh", path);
    const char* vcode = MCFileCopyContentWithPath(path);
    
    MCFileGetPath(fshader, "fsh", path);
    const char* fcode = MCFileCopyContentWithPath(path);
    
    MCGLRenderer_initWithShaderCodeString(0, obj, vcode, fcode);
    
    free((void*)vcode);
    free((void*)fcode);
    return obj;
}

method(MCGLRenderer, void, updateNodes, MC3DNode* rootnode)
{
    //update nodes
    if (rootnode != null) {
        ff(rootnode, update, obj->context);
    }
}

method(MCGLRenderer, void, drawNodes, MC3DNode* rootnode)
{
    if (rootnode != null) {
        ff(rootnode, draw, obj->context);
        //make FPS stable motion more smooth
        MCGLEngine_flushCommandBlock(0);
        //MCGLEngine_flushCommandAsync(0);
    }
}

onload(MCGLRenderer)
{
    if (load(MCObject)) {
        prehash();
        binding(MCGLRenderer, void, bye, voida);
        binding(MCGLRenderer, MCGLRenderer*, initWithShaderCodeString, const char* vcode, const char* fcode);
        binding(MCGLRenderer, MCGLRenderer*, initWithShaderFileName, const char* vshader, const char* fshader);
        binding(MCGLRenderer, void, updateNodes, MC3DNode* rootnode);
        binding(MCGLRenderer, void, drawNodes, MC3DNode* rootnode);
        return cla;
    }else{
        return null;
    }
}



