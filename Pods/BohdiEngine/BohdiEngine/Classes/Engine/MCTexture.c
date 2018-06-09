//
//  MCTexture.c
//  monkcGame
//
//  Created by Sun YuLi on 16/2/20.
//  Copyright © 2016年 oreisoft. All rights reserved.
//

#include "MCTexture.h"
#include "MCGLEngine.h"
#include "BEAssetsManager.h"

oninit(MCTexture)
{
    if (init(MCObject)) {
        var(Id) = -1;
        var(width) = 512;
        var(height)= 512;
        var(textureUnit) = MCGLEngine_getIdleTextureUnit(0);
        var(data) = null;
        var(displayMode) = MCTextureRepeat;
        var(loadedToGL) = false;
        return obj;
    }else{
        return null;
    }
}

function(unsigned char*, loadImageRawdata, const char* path)
{
    as(MCTexture);
    var(data) = BE2DTextureData_newWithPathname(path);
    if (var(data)) {
        obj->width  = obj->data->width;
        obj->height = obj->data->height;
        return obj->data->raw;
    }
    else {
        error_log("MCTexture - can not load image: %s\n", path);
        return null;
    }
}

function(void, freeRawdata, voida)
{
    as(MCTexture);
    if (obj->data) {
        release(obj->data);
        obj->data = null;
    }
}

method(MCTexture, void, bye, voida)
{
    superbye(MCObject);
    freeRawdata(obj, 0);
}

method(MCTexture, MCTexture*, initWithFileNameMode, const char* name, MCTextureDisplayMode mode)
{
    char pathbuff[PATH_MAX] = {0};
    if (MCFileGetPath(name, pathbuff)) {
        return null;
    }
    obj->displayMode = mode;
    loadImageRawdata(obj, pathbuff);
    return obj;
}

method(MCTexture, MCTexture*, initWithFileName, const char* name)
{
    return MCTexture_initWithFileNameMode(obj, name, MCTextureRepeat);
}

method(MCTexture, MCTexture*, initWith2DTexture, BE2DTextureData* tex)
{
    var(data) = tex;
    if (var(data)) {
        obj->width  = obj->data->width;
        obj->height = obj->data->height;
        return obj;
    }
    return null;
}

onload(MCTexture)
{
    if (load(MCObject)) {
        mixing(unsigned char*, loadImageRawdata, const char* name);
        //mixing(void, rawdataToTexbuffer, voida);
        //mixing(void, setupTexParameter, GLenum textype);
        mixing(void, freeRawdata, voida);
        
        binding(MCTexture, void, bye, voida);
        binding(MCTexture, MCTexture*, initWithFileNameMode, const char* name, MCTextureDisplayMode mode);
        binding(MCTexture, MCTexture*, initWithFileName, const char* name);
        binding(MCTexture, MCTexture*, initWith2DTexture, BE2DTextureData* tex);
        //binding(MCTexture, void, loadToGLBuffer, voida);
        //binding(MCTexture, void, active, GLuint pid, const char* uniformName);

        return cla;
    }else{
        return null;
    }
}
