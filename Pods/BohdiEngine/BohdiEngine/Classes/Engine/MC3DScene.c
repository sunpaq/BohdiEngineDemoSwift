//
//  MC3DScene.c
//  monkcGame
//
//  Created by Sun YuLi on 16/2/20.
//  Copyright © 2016年 oreisoft. All rights reserved.
//

#include "MC3DScene.h"
#include "MCGLEngine.h"
#include "BEAssetsManager.h"

compute(MCBool, isDrawSky)
{
    as(MC3DScene);
    if (var(skyboxShow) && var(skyboxRef)!=null) {
        return true;
    }
    return false;
}

oninit(MC3DScene)
{
    if (init(MCObject)) {
        var(skyboxShow) = false;
        var(skyboxRef)  = null;
        
        var(renderer)   = new(MCGLRenderer);
        var(rootnode)   = new(MC3DNode);
        var(mainCamera) = new(MCCamera);
        var(clock)      = new(MCClock);
        var(light)      = new(MCLight);
        
        var(next) = null;
        var(prev) = null;
        var(scenewidth) = 0;
        var(sceneheight)= 0;
        
        var(cameraLock) = false;
        var(isDrawSky) = isDrawSky;
        
        return obj;
    }else{
        return null;
    }
}

method(MC3DScene, void, printDebugInfo, voida)
{
    debug_log("MC3DScene: skyboxShow=%d\n", obj->skyboxShow);
    ff(obj->light, printDebugInfo, 0);
}

method(MC3DScene, void, bye, voida)
{
    release(var(renderer));
    release(var(rootnode));
    release(var(mainCamera));
    release(var(clock));
    release(var(light));
    
    superbye(MCObject);
}

method(MC3DScene, MC3DScene*, initWithWidthHeightVSourceFSource, unsigned width, unsigned height,
       const char* vsource, const char* fsource)
{
    var(scenewidth)  = width;
    var(sceneheight) = height;
    MCCamera_initWithWidthHeight(0, var(mainCamera), width, height);
    MCGLRenderer_initWithShaderCodeString(0, var(renderer), vsource, fsource);
    var(skyboxRef) = null;
    var(skyboxShow) = false;
    debug_log("MC3DScene - init end\n");
    return obj;
}

method(MC3DScene, MC3DScene*, initWithWidthHeightVNameFName, unsigned width, unsigned height,
       const char* vname, const char* fname)
{
    char path[LINE_MAX];
    MCFileGetPath(vname, "vsh", path);
    const char* vsource = MCFileCopyContentWithPath(path);
    
    MCFileGetPath(vname, "fsh", path);
    const char* fsource = MCFileCopyContentWithPath(path);
    
    debug_log("MC3DScene vsource: %s", vsource);
    debug_log("MC3DScene fsource: %s", fsource);
    MC3DScene_initWithWidthHeightVSourceFSource(0, obj, width, height, vsource, fsource);
    
    free((void*)vsource);
    free((void*)fsource);
    
    return obj;
}

method(MC3DScene, MC3DScene*, initWithWidthHeightDefaultShader, unsigned width, unsigned height)
{
    debug_log("MC3DScene initWithWidthHeightDefaultShader %dx%d %s\n", width, height, "MCGLRenderer");
	return MC3DScene_initWithWidthHeightVNameFName(0, obj, width, height, "MCGLRenderer", "MCGLRenderer");
}

method(MC3DScene, void, lockCamera, MCBool lock)
{
    var(cameraLock) = lock;
    printf("lock=%d\n", lock);
}

method(MC3DScene, MCCamera*, getCamera, voida)
{
    return var(mainCamera);
}

method(MC3DScene, void, moveCameraOneStep, MCFloat deltaFai, MCFloat deltaTht)
{
    if (var(cameraLock) == false) {
//        if (var(skyboxRef) != null) {
//            MCSkyboxCamera_move(0, var(skyboxRef)->camera, deltaFai.d / 5, deltaTht.d / 5);
//        }
        MCCamera_move(0, var(mainCamera), deltaFai, deltaTht);
    }
}

method(MC3DScene, void, moveSkyboxCamera, MCFloat deltaFai, MCFloat deltaTht)
{
    if (cpt(isDrawSky)) {
        MCSkyboxCamera_move(0, var(skyboxRef)->camera, deltaFai, deltaTht);
    }
}

method(MC3DScene, void, updateScene, voida)
{
    MC3DScene_moveCameraOneStep(0, obj, (MCFloat)0.5, (MCFloat)0.0);
    
    if(cpt(isDrawSky)) {
        MCSkybox_update(0, var(skyboxRef), var(renderer)->context);
    }
    
    MCCamera_update(0, obj->mainCamera, obj->renderer->context);
    MCLight_update(0, obj->light, obj->renderer->context);
    
    MCGLRenderer_updateNodes(0, var(renderer), var(rootnode));
}

method(MC3DScene, int, drawScene, voida)
{
    MCGLEngine_clearScreen(0);
    
    if (cpt(isDrawSky)) {
        MCSkybox_draw(0, var(skyboxRef), var(renderer)->context);
    }
    
    MCGLRenderer_drawNodes(0, var(renderer), var(rootnode));
    
    //calculate FPS
    return MCGLEngine_tickFPS(var(clock));
}

onload(MC3DScene)
{
    if (load(MCObject)) {
        binding(MC3DScene, void, bye, voida);
        binding(MC3DScene, MC3DScene*, initWithWidthHeightVSourceFSource, unsigned width, unsigned height, const char* vsource, const char* fsource);
        binding(MC3DScene, MC3DScene*, initWithWidthHeightVNameFName, unsigned width, unsigned height, const char* vname, const char* fname);
        binding(MC3DScene, MC3DScene*, initWithWidthHeightDefaultShader, unsigned width, unsigned height);
        
        binding(MC3DScene, void, updateScene, voida);
        binding(MC3DScene, void, drawScene, voida);
        
        binding(MC3DScene, void, lockCamera, MCBool lock);
        binding(MC3DScene, MCCamera*, getCamera, voida);
        binding(MC3DScene, void, moveCameraOneStep, double deltaFai, double deltaTht);
        binding(MC3DScene, void, moveSkyboxCamera, MCDouble deltaFai, MCDouble deltaTht);
        binding(MC3DScene, void, printDebugInfo, voida);

        return cla;
    }else{
        return null;
    }
}
