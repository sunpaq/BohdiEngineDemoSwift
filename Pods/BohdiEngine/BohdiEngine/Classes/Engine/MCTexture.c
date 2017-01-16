//
//  MCTexture.c
//  monkcGame
//
//  Created by Sun YuLi on 16/2/20.
//  Copyright © 2016年 oreisoft. All rights reserved.
//

#include "MCTexture.h"
#include "MCGLEngine.h"

oninit(MCTexture)
{
    if (init(MCObject)) {
        obj->textureUnit = GL_TEXTURE1;
        return obj;
    }else{
        return null;
    }
}

function(unsigned char*, loadImageRawdata, const char* name)
{
    as(MCTexture);
    var(rawdata) = BE2DTextureData_newWithPath(name);
    return obj->rawdata->raw;
}

function(void, rawdataToTexbuffer, GLenum textype)
{
    as(MCTexture);
    glTexImage2D(textype, 0, GL_RGB, obj->width, obj->height, 0, GL_RGB, GL_UNSIGNED_BYTE, obj->rawdata);
    glGenerateMipmap(textype);
}

//GL_TEXTURE_2D
function(void, setupTexParameter, GLenum textype)
{
    glTexParameteri(textype, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(textype, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(textype, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(textype, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

function(void, freeRawdata, voida)
{
    as(MCTexture);
    release(obj->rawdata);
}

method(MCTexture, MCTexture*, initWithFileName, const char* name)
{
    glGenBuffers(1, &obj->Id);
    MCGLEngine_activeTextureUnit(obj->textureUnit);
    MCGLEngine_bind2DTexture(obj->Id);
    
    loadImageRawdata(0, obj, name);
    rawdataToTexbuffer(0, obj, GL_TEXTURE_2D);
    setupTexParameter(0, obj, GL_TEXTURE_2D);
    freeRawdata(0, obj, 0);
    
    return obj;
}

method(MCTexture, void, drawTexture, MCGLContext* ctx)
{
    MCGLEngine_activeTextureUnit(obj->textureUnit);
    MCGLEngine_bind2DTexture(obj->Id);
    //MCGLContext_setUniformVector1(0, ctx, "texsampler", obj->textureUnit-GL_TEXTURE0);
}

onload(MCTexture)
{
    if (load(MCObject)) {
        mixing(unsigned char*, loadImageRawdata, const char* name);
        mixing(void, rawdataToTexbuffer, voida);
        mixing(void, setupTexParameter, GLenum textype);
        mixing(void, freeRawdata, voida);
        
        binding(MCTexture, MCTexture*, initWithFileName, const char* name);
        binding(MCTexture, void, drawTexture, MCGLContext* ctx);

        return cla;
    }else{
        return null;
    }
}
