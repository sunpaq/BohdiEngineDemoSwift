#include "MCCamera.h"
#include "MCGLRenderer.h"

compute(double, Radius);
compute(MCMatrix3, normal);
compute(MCMatrix4, viewMatrix);
compute(MCMatrix4, projectionMatrix);
compute(MCVector3, currentPosition);
compute(MCMatrix3, rotationMat3);

oninit(MCCamera)
{
    if (init(MC3DNode)) {
        var(ratio) = MCRatioHDTV16x9;//MCRatioCameraFilm3x2;
        var(view_angle) = MCLensStandard50mmViewAngle;
        var(depth_of_field) = 10;

        //local spherical coordinate
        var(R_value) = 100;
        var(R_percent) = 1.0;
        var(tht) = 90;
        var(fai) = 0;

        var(lookat) = MCVector3Make(0,0,0);
        var(eye) = MCVector3Make(0.0,obj->R_value,0.0);
        var(up)  = MCVector3Make(0.0,0.0,-1.0);
        
        var(Radius) = Radius;
        var(normal) = normal;
        //var(modelViewMatrix) = modelViewMatrix;
        var(viewMatrix) = viewMatrix;
        var(projectionMatrix) = projectionMatrix;
        var(currentPosition) = currentPosition;
        //transform
        var(rotationMat3) = rotationMat3;
        
        var(isReverseMovement) = false;
        var(isLockRotation) = false;
        
        var(rotateMode)= MCCameraRotateAroundModelManual;
        
        return obj;
    }else{
        return null;
    }
}

method(MCCamera, void, bye, voida)
{
    MC3DNode_bye(0, sobj, 0);
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

/*
 Initialize Status:
 
 view space coordinate orientation equal to device attitude (iphone on the table, x-right z-up)
 world space coordinate orientation equal monitor attitude (LCD on the desk, x-right -z forward)
 */
compute(MCMatrix4, viewMatrix)
{
    as(MCCamera);
    double r = cpt(Radius);
    
    if (obj->rotateMode == MCCameraRotateAroundModelByGyroscope) {
        MCMatrix4 R  = sobj->transform;
        MCMatrix4 Ri = MCMatrix4Invert(R, null);
        MCMatrix4 world = Ri;
        
        MCMatrix4 m = MCMatrix4MakeLookAt(0, 0, r,
                                          0, 0, 0,
                                          0, 1, 0);
        
        obj->eye = MCGetEyeFromRotationMat4(world, r);
        return MCMatrix4Multiply(m, world);
    }
    else if (obj->rotateMode == MCCameraRotateAR) {
        MCMatrix4 R  = sobj->transform;
        MCMatrix4 Ri = MCMatrix4Invert(R, null);
        MCVector3 e  = MCGetEyeFromRotationMat4(R, r);
        MCMatrix4 T  = MCMatrix4MakeTranslation(-e.x, -e.y, -e.z);
        obj->eye = e;
        return MCMatrix4Multiply(Ri, T);
    }
    //default is MCCameraRotateAroundModelManual
    else {
        return MCMatrix4MakeLookAtByEulerAngle_EyeUp(obj->lookat, cpt(Radius),
                                                     obj->fai, obj->tht,
                                                     &obj->eye, &obj->up);
    }
}

compute(MCMatrix4, projectionMatrix)
{
    as(MCCamera);
    double near = cpt(Radius) - var(depth_of_field);
    double far  = cpt(Radius) + var(depth_of_field);
    
    if (near <= 0) {
        near = MCLensStandard50mm;
    }
    
    return MCMatrix4MakePerspective(MCDegreesToRadians(obj->view_angle),
                                    var(ratio),
                                    near,
                                    far);
}

compute(MCVector3, currentPosition)
{
    as(MCCamera);
    return obj->eye;
}

compute(MCMatrix3, rotationMat3)
{
    as(MCCamera);
    return MCMatrix4GetMatrix3(sobj->transform);
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
    
    data.mat4 = cpt(projectionMatrix);
    MCGLContext_updateUniform(0, ctx, view_projection, data);
}

method(MCCamera, void, move, MCFloat deltaFai, MCFloat deltaTht)
{
    if (var(isLockRotation) == true) {
        return;
    }
    //angle with x+
    if (var(isReverseMovement)) {
        obj->fai += deltaFai.f;   //Left
        obj->tht += deltaTht.f;   //Up
    }else{
        obj->fai -= deltaFai.f;   //Left
        obj->tht -= deltaTht.f;   //Up
    }
}

method(MCCamera, void, fucus, MCFloat deltaX, MCFloat deltaY)
{
    //angle with x+
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

method(MCCamera, void, setRotationMat3, float mat3[9])
{
    if (mat3) {
        MCMatrix3 m3 = {0};
        for (int i=0; i<9; i++) {
            m3.m[i] = mat3[i];
        }
        sobj->transform = MCMatrix4FromMatrix3(m3);
    }
}

onload(MCCamera)
{
    if (load(MC3DNode)) {
        binding(MCCamera, void, bye, voida);
        binding(MCCamera, MCCamera*, initWithWidthHeight, unsigned width, unsigned height);
        binding(MCCamera, void, move, MCFloat deltaFai, MCFloat deltaTht);
        binding(MCCamera, void, fucus, MCFloat deltaX, MCFloat deltaY);
        binding(MCCamera, void, pull, MCFloat deltaR);
        binding(MCCamera, void, reset, MCBool updateOrNot);
        binding(MCCamera, void, update);
        binding(MCCamera, void, distanceScale, MCFloat scale);
        binding(MCCamera, void, setRotationMat3, float mat3[9]);
        binding(MCCamera, void, printDebugInfo, voida);
        return cla;
    }else{
        return null;
    }
}

