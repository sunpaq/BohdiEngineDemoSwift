#include "MCCamera.h"
#include "MCGLRenderer.h"

compute(double, Radius);
compute(MCMatrix3, normal);
compute(MCMatrix4, viewMatrix);
compute(MCMatrix4, projectionMatrix);
compute(MCVector3, currentPosition);

oninit(MCCamera)
{
    if (init(MC3DNode)) {
        var(ratio) = MCRatioOldTV4x3;//MCRatioCameraFilm3x2;
        var(focal_length) = MCLensStandard50mm;//MCLensWide24mm;
        var(view_angle) = MCLensStandard50mmViewAngle;
        var(max_distance) = 10000;//100 metres
        var(lookat) = MCVector3Make(0,0,0);
        
        //local spherical coordinate
        var(R_value) = 100;
        var(R_percent) = 1.0;
        var(tht) = 60.0;
        var(fai) = 45.0;
        
        var(Radius) = Radius;
        var(normal) = normal;
        //var(modelViewMatrix) = modelViewMatrix;
        var(viewMatrix) = viewMatrix;
        var(projectionMatrix) = projectionMatrix;
        var(currentPosition) = currentPosition;
        
        var(isReverseMovement) = true;
        var(isLockRotation) = false;        
        return obj;
    }else{
        return null;
    }
}

method(MCCamera, void, printDebugInfo, voida)
{
    debug_log("MCCamera: lookat=%.2f/%.2f/%.2f R_value=%.2f R_percent=%.3f\n",
              obj->lookat.x, obj->lookat.y, obj->lookat.z, obj->R_value, obj->R_percent);
}

compute(double, Radius)
{
    as(MCCamera);
    return (obj->R_value * obj->R_percent);
}

compute(MCMatrix3, normal)
{
    as(MCCamera);
    MCMatrix3 nor = MCMatrix3InvertAndTranspose((MCMatrix3)MCMatrix4GetMatrix3(cpt(viewMatrix)), NULL);
    return nor;
}

compute(MCMatrix4, viewMatrix)
{
    as(MCCamera);
    MCVector3 modelpos = var(lookat);
    MCVector3 eyelocal = MCVertexFromSpherical(obj->Radius(obj), var(tht), var(fai));
    MCVector3 eye = MCWorldCoorFromLocal(eyelocal, modelpos);
    
    MCVector3 up = (MCVector3){0.0, 1.0, 0.0};
    if (var(tht) > 0.0 && var(tht) < 90.0) {
        MCVector3 Npole = MCVector3Make(0, cpt(Radius)/MCCosDegrees(var(tht)), 0);
        up = (MCVector3){Npole.x-eye.x, Npole.y-eye.y, Npole.z-eye.z};
    }
    else if (var(tht) > 90.0 && var(tht) < 180.0) {
        MCVector3 Spole = MCVector3Make(0, -cpt(Radius)/MCCosDegrees(180.0-var(tht)), 0);
        up = (MCVector3){eye.x-Spole.x, eye.y-Spole.y, eye.z-Spole.z};
    }
    return MCMatrix4MakeLookAt(eye.x, eye.y, eye.z,
                               modelpos.x, modelpos.y, modelpos.z,
                               up.x, up.y, up.z);
}

compute(MCMatrix4, projectionMatrix)
{
    as(MCCamera);
    return MCMatrix4MakePerspective(MCDegreesToRadians(obj->view_angle),
                                    var(ratio),
                                    var(focal_length),
                                    var(max_distance));
}

compute(MCVector3, currentPosition)
{
    as(MCCamera);
    return MCWorldCoorFromLocal(MCVertexFromSpherical(cpt(Radius), var(tht), var(fai)), var(lookat));
}

method(MCCamera, MCCamera*, initWithWidthHeight, unsigned width, unsigned height)
{
    //setting camera
    obj->ratio = MCRatioMake(width, height);
    obj->R_value = 5;
    return obj;
}

method(MCCamera, void, reset, voida)
{
    init(MCCamera);
}

//override
method(MCCamera, void, update, MCGLContext* ctx)
{
    MCGLUniformData data;
    
    data.mat4 = cpt(viewMatrix);
    MCGLContext_updateUniform(0, ctx, view_view, data);
    
    if (ctx->cameraRatio != obj->ratio) {
        data.mat4 = cpt(projectionMatrix);
        MCGLContext_updateUniform(0, ctx, view_projection, data);
    }
    
    data.vec3 = cpt(currentPosition);
    MCGLContext_updateUniform(0, ctx, view_position, data);
    MCVector3 lightpos = MCVector3Add(cpt(currentPosition), (MCVector3){0, 0, 0});
    data.vec3 = lightpos;
    MCGLContext_updateUniform(0, ctx, light_position, data);
}

method(MCCamera, void, move, MCFloat deltaFai, MCFloat deltaTht)
{
    if (var(isLockRotation) == true) {
        return;
    }
    if (var(isReverseMovement)) {
        obj->fai += deltaFai.f;   //Left
        obj->tht += deltaTht.f;   //Up
    }else{
        obj->fai -= deltaFai.f;   //Left
        obj->tht -= deltaTht.f;   //Up
    }
    
    //keep the tht -180 ~ 180
    if (obj->tht < -179.99) {
        obj->tht = -179.99;
    }
    if (obj->tht > 179.99) {
        obj->tht = 179.99;
    }
}

method(MCCamera, void, fucus, MCFloat deltaX, MCFloat deltaY)
{
    if (var(isReverseMovement)) {
        obj->lookat.x += deltaX.f;
        obj->lookat.y += deltaY.f;
        
    }else{
        obj->lookat.x -= deltaX.f;
        obj->lookat.y -= deltaY.f;
    }
}

method(MCCamera, void, pull, MCFloat deltaR)
{
    obj->R_value += deltaR.f;
}

method(MCCamera, void, distanceScale, MCFloat scale)
{
    obj->R_percent = scale.f;
}

onload(MCCamera)
{
    if (load(MC3DNode)) {
        binding(MCCamera, MCCamera*, initWithWidthHeight, unsigned width, unsigned height);
        binding(MCCamera, void, move, MCFloat deltaFai, MCFloat deltaTht);
        binding(MCCamera, void, fucus, MCFloat deltaX, MCFloat deltaY);
        binding(MCCamera, void, pull, MCFloat deltaR);
        binding(MCCamera, void, reset, MCBool updateOrNot);
        binding(MCCamera, void, update);
        binding(MCCamera, void, distanceScale, MCFloat scale);
        binding(MCCamera, void, printDebugInfo, voida);

        return cla;
    }else{
        return null;
    }
}

