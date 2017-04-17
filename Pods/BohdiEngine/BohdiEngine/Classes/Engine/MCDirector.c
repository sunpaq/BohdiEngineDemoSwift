//
//  MCDirector.c
//  monkcGame
//
//  Created by Sun YuLi on 16/3/19.
//  Copyright © 2016年 oreisoft. All rights reserved.
//

#include "MCDirector.h"
#include "MCThread.h"

compute(MCLight*, lightHandler)
{
    as(MCDirector);
    if (var(lastScene) != null && var(lastScene)->light != null) {
        return var(lastScene)->light;
    }
    return null;
}

compute(MCCamera*, cameraHandler)
{
    as(MCDirector);
    if (var(lastScene) != null && var(lastScene)->mainCamera != null) {
        return var(lastScene)->mainCamera;
    }
    return null;
}

compute(MCGLContext*, contextHandler)
{
    as(MCDirector);
    return var(lastScene)->renderer->context;
}

oninit(MCDirector)
{
    if (init(MCObject)) {
        var(lastScene) = null;
        var(currentWidth) = 0;
        var(currentHeight) = 0;
        
        var(gyroscopeMode) = true;
        var(lightFollowCamera) = true;
        var(deviceRotationMat3) = MCMatrix3Identity;
        
        var(lightHandler) = lightHandler;
        var(cameraHandler) = cameraHandler;
        var(contextHandler) = contextHandler;
        
        //var(skybox) = null;
        //var(skysph) = null;
        
        var(skyboxThread) = new(MCThread);
        var(modelThread) = new(MCThread);
        return obj;
    }else{
        return null;
    }
}

function(void, releaseScenes, MC3DScene* scene)
{
    as(MCDirector);
    if (scene!= null) {
        if (scene->prev != null) {
            releaseScenes(0, obj, scene->prev);
        }else{
            release(scene);
        }
    }
}

method(MCDirector, void, bye, voida)
{
    if (obj->lastScene != null) {
        releaseScenes(0, obj, obj->lastScene);
        obj->lastScene = null;
    }
    //release(var(skybox));
    //release(var(skysph));
    release(var(skyboxThread));
    release(var(modelThread));

    superbye(MCObject);
}

method(MCDirector, void, updateAll, voida)
{
    if (obj && var(lastScene) != null) {
        if (var(gyroscopeMode)) {
            MCCamera_setRotationMat3(0, cpt(cameraHandler), obj->deviceRotationMat3.m);
            MC3DScene_setRotationMat3(0, var(lastScene), obj->deviceRotationMat3.m);
        }
        if (var(lightFollowCamera) && cpt(lightHandler) && cpt(cameraHandler)) {
            cpt(lightHandler)->lightPosition = computed(cpt(cameraHandler), currentPosition);
            cpt(lightHandler)->dataChanged = true;
        }
        MC3DScene_updateScene(0, var(lastScene), 0);
    }
}

method(MCDirector, int, drawAll, voida)
{
    int fps = -1;
    if (obj && var(lastScene) != null) {
        fps = MC3DScene_drawScene(0, var(lastScene), 0);
    }
    return fps;
}

method(MCDirector, void, setupMainScene, unsigned width, unsigned height)
{
    MC3DScene* scene = ff(new(MC3DScene), initWithWidthHeightDefaultShader, width, height);
    if (scene) {
        releaseScenes(0, obj, obj->lastScene);
        MCDirector_pushScene(0, obj, scene);
        release(scene);
    }
}

method(MCDirector, void, pushScene, MC3DScene* scene)
{
    if (var(lastScene) == null) {
        scene->next = null;
        scene->prev = null;
        var(lastScene) = scene;
        retain(scene);
    }else{
        scene->next = null;
        scene->prev = var(lastScene);
        var(lastScene)->next = scene;
        var(lastScene) = scene;
        retain(scene);
    }
}

method(MCDirector, void, popScene, voida)
{
    if (var(lastScene)) {
        ff(var(lastScene), lockCamera, true);
        MC3DScene* current = var(lastScene);
        //first scene
        if (current->prev == null) {
            var(lastScene) = null;
            release(current);
        }
        else {
            var(lastScene) = current->next;
            release(current);
        }
        ff(var(lastScene), lockCamera, false);
    }
}

method(MCDirector, void, resizeAllScene, int width, int height)
{
    if (var(currentWidth) == width && var(currentHeight) == height) {
        //no need to update
        return;
    }
    MC3DScene* iter = null;
    for (iter=var(lastScene); iter!=null; iter=iter->prev) {
        MC3DScene_resizeScene(0, iter, width, height);
    }
    var(currentWidth) = width;
    var(currentHeight) = height;
}

method(MCDirector, void, addNode, MC3DNode* node)
{
    if(node && obj->lastScene && obj->lastScene->rootnode) {
        MC3DNode_addChild(0, obj->lastScene->rootnode, (MC3DNode*)node);
    }else{
        error_log("MCDirector add node(%p) failed [lastScene=%p rootnode=%p]\n",
                  node, obj->lastScene, obj->lastScene->rootnode);
    }
}

method(MCDirector, void, addModel, MC3DModel* model)
{
    if(model && obj->lastScene && obj->lastScene->rootnode) {
        MC3DNode_addChild(0, obj->lastScene->rootnode, (MC3DNode*)model);
        double maxl  = computed(model, maxlength);
        double scale = 10 / maxl;
        MCVector3 scaleVec = MCVector3Make(scale, scale, scale);
        MC3DNode_scaleVec3(0, &model->Super, &scaleVec, false);
        debug_log("MCDirector - model maxlength=%lf scale=%lf\n", maxl, scale);
    }else{
        error_log("MCDirector add model(%p) failed [lastScene=%p rootnode=%p]\n",
                  model, obj->lastScene, obj->lastScene->rootnode);
    }
}

method(MCDirector, void, addModelNamed, const char* name)
{
    MC3DModel* model = new(MC3DModel);
    MC3DModel_initWithFileName(0, model, name);
    MCDirector_addModel(0, obj, model);
}

method(MCDirector, void, removeCurrentModel, voida)
{
    if (obj->lastScene) {
        MCLinkedList* list = obj->lastScene->rootnode->children;
        MCLinkedList_popItem(0, list, 0);
    }
}

method(MCDirector, void, addSkyboxNamed, const char* names[6])
{
    if (obj->lastScene) {
        MCSkybox* box = ff(new(MCSkybox), initWithFileNames, names);
        if (box) {
            MC3DScene_addSkybox(0, obj->lastScene, box);
            release(box);
        }
    }
}

method(MCDirector, void, addSkysphereNamed, const char* name)
{
    if (obj->lastScene) {
        MCSkysphere* sph = ff(new(MCSkysphere), initWithFileName, name);
        if (sph) {
            MC3DScene_addSkysph(0, obj->lastScene, sph);
            release(sph);
        }
    }
}

method(MCDirector, void, removeCurrentSkybox, voida)
{
    if (obj->lastScene) {
        MC3DScene_removeSkybox(0, obj->lastScene, 0);
    }
}

method(MCDirector, void, removeCurrentSkysph, voida)
{
    if (obj->lastScene) {
        MC3DScene_removeSkysph(0, obj->lastScene, 0);
    }
}

method(MCDirector, void, cameraFocusOn, MCVector4 vertex)
{
    MCCamera* c = computed(obj, cameraHandler);
    if (c != null) {
        c->lookat.x = vertex.x;
        c->lookat.y = vertex.y;
        c->lookat.z = vertex.z;
        c->R_value  = vertex.w;
        c->R_percent= 1.0;
    }
}

method(MCDirector, void, cameraFocusOnModel, MC3DModel* model)
{
    MC3DFrame frame = model->lastSavedFrame;
    double mheight = frame.ymax - frame.ymin;
    double mwidth  = frame.xmax - frame.xmin;
    double mdepth  = frame.zmax - frame.zmin;
    
    double _max = (mheight > mwidth) ? mheight : mwidth;
    double max = (mdepth > _max) ? mdepth : _max;
    
    cpt(cameraHandler)->lookat.y = mheight / 2.0f;
    cpt(cameraHandler)->R_value = max * 2.0f;
}

method(MCDirector, void, moveModelToOrigin, MC3DModel* model)
{
    MC3DModel_translateToOrigin(0, model, 0);
}

method(MCDirector, void, setDeviceRotationMat3, float mat3[9])
{
    if (mat3) {
        for (int i=0; i<9; i++) {
            obj->deviceRotationMat3.m[i] = mat3[i];
        }
    }
}

method(MCDirector, void, setCameraRotateMode, MCCameraRotateMode mode)
{
    if (cpt(cameraHandler)) {
        cpt(cameraHandler)->rotateMode = mode;
    }
}

method(MCDirector, void, printDebugInfo, voida)
{
    debug_log("MCDirector currentWidth=%d currentHeight=%d\n", obj->currentWidth, obj->currentHeight);
    MCCamera* cam = cpt(cameraHandler);
    if (cam) {
        ff(cam, printDebugInfo, 0);
    }
    if (obj->lastScene) {
        ff(obj->lastScene, printDebugInfo, 0);
    }
}

onload(MCDirector)
{
    if (load(MCObject)) {
        mixing(void, releaseScenes, MC3DScene* scene);
        
        binding(MCDirector, void, bye, voida);
        binding(MCDirector, void, updateAll, voida);
        binding(MCDirector, void, drawAll, voida);
        binding(MCDirector, void, setupMainScene, unsigned width, unsigned height);
        binding(MCDirector, void, pushScene, MC3DScene* scene);
        binding(MCDirector, void, popScene, voida);
        binding(MCDirector, void, resizeAllScene, int width, int height);
        binding(MCDirector, void, addNode, MC3DNode* node);
        binding(MCDirector, void, addModel, MC3DModel* model);
        binding(MCDirector, void, addModelNamed, const char* name);
        binding(MCDirector, void, removeCurrentModel, voida);
        binding(MCDirector, void, addSkyboxNamed, const char* names[6]);
        binding(MCDirector, void, addSkysphereNamed, const char* name);
        binding(MCDirector, void, removeCurrentSkybox, voida);
        binding(MCDirector, void, removeCurrentSkysph, voida);
        binding(MCDirector, void, cameraFocusOn, MCVector3 vertex);
        binding(MCDirector, void, cameraFocusOnModel, MC3DModel* model);
        binding(MCDirector, void, moveModelToOrigin, MC3DModel* model);
        binding(MCDirector, void, setDeviceRotationMat3, float mat3[9]);
        binding(MCDirector, void, setCameraRotateMode, MCCameraRotateMode mode);
        binding(MCDirector, void, printDebugInfo, voida);

        return cla;
    }else{
        return null;
    }
}
