//
//  MCSkyboxCamera.c
//  Sapindus
//
//  Created by Sun YuLi on 16/5/1.
//  Copyright © 2016年 oreisoft. All rights reserved.
//

#include "MCSkyboxCamera.h"

compute(MCMatrix4, boxViewMatrix);
compute(MCMatrix4, boxProjectionMatrix);
compute(MCGLUniform, viewUniform);
compute(MCGLUniform, projectionUniform);

//current positon always 0,0,0
oninit(MCSkyboxCamera)
{
    if (init(MCCamera)) {
        //super
        sobj->ratio = MCRatioHDTV16x9;
        sobj->R_value = 1.0;
        sobj->R_percent = 1.0;
        sobj->tht = 0;
        sobj->fai = 90;
        
        //world coordinate
        sobj->lookat = MCVector3Make(0, -1, 0);
        sobj->eye    = MCVector3Make(0, 0, 0);
        sobj->up     = MCVector3Make(0, 0, 1);
        
        //uniforms
        obj->viewMatrix        = boxViewMatrix;
        obj->projectionMatrix  = boxProjectionMatrix;
        obj->viewUniform       = viewUniform;
        obj->projectionUniform = projectionUniform;
        
        return obj;
    }else{
        return null;
    }
}

compute(MCMatrix4, boxViewMatrix)
{
    as(MCSkyboxCamera);
    MCMatrix4 m = MCMatrix4MakeLookAt(0, 0,0,
                                      0, 0,-1,
                                      0, 1,0);
    MCMatrix4 imat4 = MCMatrix4Invert(obj->Super.Super.transform, null);
    
    return MCMatrix4Multiply(m, imat4);
}

compute(MCMatrix4, boxProjectionMatrix)
{
    as(MCSkyboxCamera);
    return MCMatrix4MakePerspective(MCDegreesToRadians(sobj->view_angle),
                                    sobj->ratio,
                                    0.1,
                                    2);
}

compute(MCGLUniform, viewUniform)
{
    as(MCSkyboxCamera);
    MCGLUniform uniform;
    uniform.type = MCGLUniformMat4;
    uniform.data.mat4 = cpt(viewMatrix);
    return uniform;
}

compute(MCGLUniform, projectionUniform)
{
    as(MCSkyboxCamera);
    MCGLUniform uniform;
    uniform.type = MCGLUniformMat4;
    uniform.data.mat4 = cpt(projectionMatrix);
    return uniform;
}

//override
method(MCSkyboxCamera, MCSkyboxCamera*, initWithWidthHeightRatio, MCFloat ratio)
{
    //setting camera
    sobj->ratio = ratio.f;
    return obj;
}

method(MCSkyboxCamera, void, move, MCFloat deltaFai, MCFloat deltaTht)
{
    MCCamera_move(0, sobj, deltaFai, deltaTht);
}

method(MCSkyboxCamera, void, update, MCGLContext* ctx)
{
    //change value
    MCGLContext_activateShaderProgram(0, ctx, 0);
    
    MCGLUniform uv = computed(obj, viewUniform);
    MCGLUniform up = computed(obj, projectionUniform);

    MCGLContext_updateUniform(0, ctx, "boxViewMatrix", uv.data);
    MCGLContext_updateUniform(0, ctx, "boxProjectionMatrix", up.data);
    
    MCGLContext_setUniforms(0, ctx, 0);
}

method(MCSkyboxCamera, void, setRotationMat3, float mat3[9])
{
    MCCamera_setRotationMat3(0, sobj, mat3);
}

onload(MCSkyboxCamera)
{
    if (load(MCCamera)) {
        binding(MCSkyboxCamera, MCSkyboxCamera*, initWithWidthHeightRatio, double ratio);
        binding(MCSkyboxCamera, void, move, double deltaFai, double deltaTht);
        binding(MCSkyboxCamera, void, update, MCGLContext* ctx);
        binding(MCSkyboxCamera, void, setRotationMat3, float mat3[9]);
        return cla;
    }else{
        return null;
    }
}

