//
//  MCDirector.c
//  monkcGame
//
//  Created by Sun YuLi on 16/3/19.
//  Copyright © 2016年 oreisoft. All rights reserved.
//

#include "MCDirector.h"
#include "MCThread.h"

compute(MCCamera*, cameraHandler)
{
    as(MCDirector);
    if (var(lastScene) != null && var(lastScene)->mainCamera != null) {
        return var(lastScene)->mainCamera;
    }
    return null;
}

oninit(MCDirector)
{
    if (init(MCObject)) {
        var(lastScene) = null;
        var(currentWidth) = 0;
        var(currentHeight) = 0;
        
        var(cameraHandler) = cameraHandler;
        
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
    }
    release(var(skyboxThread));
    release(var(modelThread));

    superbye(MCObject);
}

method(MCDirector, void, updateAll, voida)
{
    if (var(lastScene) != null) {
        MC3DScene_updateScene(0, var(lastScene), 0);
    }
}

method(MCDirector, int, drawAll, voida)
{
    int fps = -1;
    if (var(lastScene) != null) {
        fps = MC3DScene_drawScene(0, var(lastScene), 0);
    }
    return fps;
}

method(MCDirector, void, setupMainScene, unsigned width, unsigned height)
{
    MC3DScene* scene = ff(new(MC3DScene), initWithWidthHeightDefaultShader, width, height);
    MCDirector_pushScene(0, obj, scene);
}

method(MCDirector, void, pushScene, MC3DScene* scene)
{
    if (var(lastScene) == null) {
        var(lastScene) = scene;
    }else{
        scene->prev = var(lastScene);
        var(lastScene)->next = scene;
        
        var(lastScene) = scene;
    }
}

method(MCDirector, void, popScene, voida)
{
    ff(var(lastScene), lockCamera, true);
    //first scene
    if (var(lastScene) != null && var(lastScene)->prev != null) {
        var(lastScene) = var(lastScene)->prev;
    }
    //last scene
    else if (var(lastScene) != null && var(lastScene)->next != null) {
        var(lastScene) = var(lastScene)->next;
    }
    ff(var(lastScene), lockCamera, false);
}

method(MCDirector, void, resizeAllScene, int width, int height)
{
    if (var(currentWidth) == width && var(currentHeight) == height) {
        //no need to update
        return;
    }
    MC3DScene* iter;
    for (iter=var(lastScene); iter!=null; iter=iter->prev) {
        if (iter->skyboxRef != null) {
            superof(iter->skyboxRef->camera)->ratio = MCRatioMake(width, height);
        }
        if (iter->mainCamera != null) {
            iter->mainCamera->ratio = MCRatioMake(width, height);
        }
    }
    var(currentWidth) = width;
    var(currentHeight) = height;
}

method(MCDirector, void, addModel, MC3DModel* model)
{
    if(model && obj->lastScene && obj->lastScene->rootnode) {
        MC3DNode_addChild(0, obj->lastScene->rootnode, (MC3DNode*)model);
        
    }else{
        error_log("MCDirector add model(%p) failed [lastScene=%p rootnode=%p]\n",
                  model, obj->lastScene, obj->lastScene->rootnode);
    }
}

method(MCDirector, void, addModelNamed, const char* name)
{
    MC3DModel* model = new(MC3DModel);
    MC3DModel_initWithFileName(0, model, name);
    MCDirector_cameraFocusOnModel(0, obj, model);
    MCDirector_addModel(0, obj, model);
}

method(MCDirector, void, cameraFocusOn, MCVector3 vertex)
{
    MCCamera* c = computed(obj, cameraHandler);
    if (c != null) {
        c->lookat.x = vertex.x;
        c->lookat.y = vertex.y;
        c->lookat.z = vertex.z;
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
        binding(MCDirector, void, addModel, MC3DModel* model);
        binding(MCDirector, void, addModelNamed, const char* name);
        binding(MCDirector, void, cameraFocusOn, MCVector3 vertex);
        binding(MCDirector, void, cameraFocusOnModel, MC3DModel* model);
        binding(MCDirector, void, printDebugInfo, voida);

        return cla;
    }else{
        return null;
    }
}
