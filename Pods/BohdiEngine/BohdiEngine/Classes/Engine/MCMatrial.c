//
//  MCMatrial.c
//  monkcGame
//
//  Created by SunYuLi on 16/2/24.
//  Copyright © 2016年 oreisoft. All rights reserved.
//

#include "MCMatrial.h"
#include "MC3DBase.h"
#include "MCGLRenderer.h"

oninit(MCMatrial)
{
    if (init(MCObject)) {
        obj->ambientLightColor  = MCVector3Make(1.0, 1.0, 1.0);
        obj->diffuseLightColor  = MCVector3Make(1.0, 1.0, 1.0);
        obj->specularLightColor = MCVector3Make(1.0, 1.0, 1.0);
        obj->specularLightPower = 32.0;
        obj->dissolve = 1.0;
        obj->dataChanged = true;
        
        obj->tag[0] = NUL;
        return obj;
    }else{
        return null;
    }
}

method(MCMatrial, void, prepareMatrial, MCGLContext* ctx)
{
    if (obj->dataChanged == true) {
        MCGLContext_activateShaderProgram(0, ctx, 0);
        
        MCGLUniform f;
        f.type = MCGLUniformVec3;
        f.data.vec3 = obj->ambientLightColor;
        MCGLContext_updateUniform(0, ctx, material_ambient, f.data);

        f.data.vec3 = obj->diffuseLightColor;
        f.type = MCGLUniformVec3;
        MCGLContext_updateUniform(0, ctx, material_diffuse, f.data);
    
        f.type = MCGLUniformVec3;
        f.data.vec3 = obj->specularLightColor;
        MCGLContext_updateUniform(0, ctx, material_specular, f.data);
        
        f.data.vec1 = obj->specularLightPower;
        f.type = MCGLUniformVec1;
        MCGLContext_updateUniform(0, ctx, material_shininess, f.data);
    
        f.data.vec1 = obj->dissolve;
        f.type = MCGLUniformVec1;
        MCGLContext_updateUniform(0, ctx, material_dissolve, f.data);

        obj->dataChanged = false;
    }
}

onload(MCMatrial)
{
    if (load(MCObject)) {
        binding(MCMatrial, void, prepareMatrial, MCGLContext* ctx);
        return cla;
    }else{
        return null;
    }
}
