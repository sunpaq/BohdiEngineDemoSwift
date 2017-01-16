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
        sobj->ratio = MCRatioHDTV16x9;

        //world coordinate
        sobj->lookat = MCVector3Make(0,0,-1);
        
        //local spherical coordinate
        sobj->R_value = 1.0;
        sobj->R_percent = 1.0;
        sobj->tht = 0.0;
        sobj->fai = 90.0;
        
        obj->viewMatrix        = boxViewMatrix;
        obj->projectionMatrix  = boxProjectionMatrix;
        obj->viewUniform       = viewUniform;
        obj->projectionUniform = projectionUniform;
        
        return obj;
    }else{
        return null;
    }
}

method(MCSkyboxCamera, void, bye, voida)
{
    //MCCamera_bye(0, sobj, 0);
}

compute(MCMatrix4, boxViewMatrix)
{
    as(MCSkyboxCamera);
    MCVector3 lookat = MCVector3Make(MCSinDegrees(sobj->fai), MCSinDegrees(sobj->tht), MCCosDegrees(sobj->tht));
    MCVector3 up     = MCVector3Make(MCTanDegrees(sobj->fai) * MCSinDegrees(sobj->tht),
                                     MCCosDegrees(sobj->tht), MCSinDegrees(sobj->tht));
    MCVector3 eye = MCVector3Make(0, 0, 0);

    return MCMatrix4MakeLookAt(eye.x, eye.y, eye.z,
                               lookat.x, lookat.y, lookat.z,
                               up.x, up.y, up.z);
}

compute(MCMatrix4, boxProjectionMatrix)
{
    as(MCSkyboxCamera);
    return MCMatrix4MakePerspective(MCDegreesToRadians(sobj->view_angle),
                                    sobj->ratio,
                                    sobj->focal_length,
                                    sobj->max_distance);
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
    if (sobj->isLockRotation == true) {
        return;
    }
    if (sobj->isReverseMovement) {
        sobj->fai += deltaFai.f;   //Left
        sobj->tht += deltaTht.f;   //Up
    }else{
        sobj->fai -= deltaFai.f;   //Left
        sobj->tht -= deltaTht.f;   //Up
    }
    
    //keep the tht -180 ~ 180
    if (sobj->tht < -179.99) {
        sobj->tht = -179.99;
    }
    if (sobj->tht > 179.99) {
        sobj->tht = 179.99;
    }
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

method(MCSkyboxCamera, void, setAttitude, MCFloat fai, MCFloat tht)
{
    sobj->fai = fai.f;
    sobj->tht = tht.f;
}

onload(MCSkyboxCamera)
{
    if (load(MCCamera)) {
        binding(MCSkyboxCamera, void, bye, voida);
        binding(MCSkyboxCamera, MCSkyboxCamera*, initWithWidthHeightRatio, double ratio);
        binding(MCSkyboxCamera, void, move, double deltaFai, double deltaTht);
        binding(MCSkyboxCamera, void, update, MCGLContext* ctx);
        binding(MCSkyboxCamera, void, setAttitude, double fai, double tht);
        
        return cla;
    }else{
        return null;
    }
}

