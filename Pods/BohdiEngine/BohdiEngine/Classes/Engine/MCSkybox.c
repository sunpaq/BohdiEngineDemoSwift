//
//  MCSkybox.c
//  monkcGame
//
//  Created by Sun YuLi on 16/4/13.
//  Copyright © 2016年 oreisoft. All rights reserved.
//

#include "MCSkybox.h"
#include "MCGLEngine.h"
#include "MCGLContext.h"
#include "MCCamera.h"

static GLfloat skyboxVertices[] = {
    -1.0f, -1.0f, -1.0f, //000 0
    -1.0f, -1.0f,  1.0f, //001 1
    -1.0f,  1.0f, -1.0f, //010 2
    -1.0f,  1.0f,  1.0f, //011 3
    1.0f, -1.0f, -1.0f,  //100 4
    1.0f, -1.0f,  1.0f,  //101 5
    1.0f,  1.0f, -1.0f,  //110 6
    1.0f,  1.0f,  1.0f,  //111 7
};

static GLuint indexs[] = {
    2,0,4,4,6,2,
    1,0,2,2,3,1,
    4,5,7,7,6,4,
    1,3,7,7,5,1,
    2,6,7,7,3,2,
    0,1,4,4,1,5
};

oninit(MCSkybox)
{
    if (init(MC3DNode)) {
        var(vaoid) = 0;
        var(vboid) = 0;
        var(texid) = 0;
        
        var(camera) = new(MCSkyboxCamera);
        var(ctx) = new(MCGLContext);
        
        MCGLEngine_setClearScreenColor((MCColorf){0.05, 0.25, 0.35, 1.0});
//        MCGLEngine_featureSwith(MCGLCullFace, true);
//        MCGLEngine_cullFace(MCGLBack);
//        MCGLEngine_setFrontCounterClockWise(true);//CCW
        return obj;
    }else{
        return null;
    }
}

method(MCSkybox, void, bye, voida)
{
    release(var(camera));
    MC3DNode_bye(0, sobj, 0);
}

method(MCSkybox, MCSkybox*, initWithCubeTexture, BECubeTextureData* cubetex, double widthHeightRatio)
{
    //Shader
    MCGLContext_initWithShaderName(0, var(ctx), "MCSkyboxShader.vsh", "MCSkyboxShader.fsh",
                                   (const char* []){
                                       "position"
                                   }, 1,
                                   (MCGLUniformType []){
                                       MCGLUniformMat4,
                                       MCGLUniformMat4,
                                       MCGLUniformScalar
                                   },
                                   (const char* []){
                                       "boxViewMatrix",
                                       "boxProjectionMatrix",
                                       "cubeSampler"
                                   }, 3);
    
    //Camera
    MCSkyboxCamera_initWithWidthHeightRatio(0, var(camera), (MCFloat)widthHeightRatio);
    
    //Mesh & Texture
    MCUInt buffers[3];
    glGenVertexArrays(1, &var(vaoid));
    glGenBuffers(3, buffers);
    var(vboid) = buffers[0];
    var(eboid) = buffers[1];
    var(texid) = buffers[2];
    //VAO
    glBindVertexArray(var(vaoid));
    //VBO
    glBindBuffer(GL_ARRAY_BUFFER, var(vboid));
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);
    //EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, var(eboid));
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexs), indexs, GL_STATIC_DRAW);
    //VAttributes
    MCVertexAttribute attr = (MCVertexAttribute){MCVertexAttribPosition, 3, GL_FLOAT, GL_FALSE,
        sizeof(GLfloat) * 3, MCBUFFER_OFFSET(0)};
    MCVertexAttributeLoad(&attr);
    //Texture
    MCGLEngine_activeTextureUnit(0);
    MCGLEngine_bindCubeTexture(var(texid));
    for (int i=0; i<6; i++) {
        BE2DTextureData* face = cubetex->faces[i];
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
                     GL_RGB, face->width, face->height, 0,
                     GL_RGB, GL_UNSIGNED_BYTE, face->raw);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    //Unbind
    glBindVertexArray(0);
    
    return obj;
}

method(MCSkybox, MCSkybox*, initWithFileNames, const char* namelist[], double widthHeightRatio)
{
    BECubeTextureData* data = BECubeTextureData_newWithFaces(namelist);
    MCSkybox* skybox = MCSkybox_initWithCubeTexture(0, obj, data, widthHeightRatio);
    release(data);
    return skybox;
}

method(MCSkybox, MCSkybox*, initWithDefaultFilesRatio, double widthHeightRatio)
{
    const char* names[6] = {"right.jpg","left.jpg","top.jpg","bottom.jpg","back.jpg","front.jpg"};
    return MCSkybox_initWithFileNames(0, obj, names, widthHeightRatio);
}

method(MCSkybox, MCSkybox*, initWithDefaultFiles, voida)
{
    const char* names[6] = {"right.jpg","left.jpg","top.jpg","bottom.jpg","back.jpg","front.jpg"};
    return MCSkybox_initWithFileNames(0, obj, names, MCRatioHDTV16x9);
}

method(MCSkybox, void, resizeWithWidthHeight, unsigned width, unsigned height)
{
    superof(var(camera))->ratio = MCRatioMake(width, height);
}

method(MCSkybox, void, update, MCGLContext* ctx)
{
    obj->boxViewMatrix = var(camera)->viewMatrix(var(camera));
    obj->boxProjectionMatrix = var(camera)->projectionMatrix(var(camera));
    
    if (obj->boxCameraRatio != superof(obj->camera)->ratio) {
        MCGLContext_activateShaderProgram(0, var(ctx), 0);
        
        MCGLUniformData data;
        data.mat4 = obj->boxProjectionMatrix;
        MCGLContext_updateUniform(0, var(ctx), "boxProjectionMatrix", data);
        obj->boxCameraRatio = superof(var(camera))->ratio;
    }
}

method(MCSkybox, void, draw, MCGLContext* ctx)
{
    glDepthMask(GL_FALSE);
    MCGLContext_activateShaderProgram(0, var(ctx), 0);
    MCGLUniformData data;
    data.mat4 = obj->boxViewMatrix;
    MCGLContext_updateUniform(0, var(ctx), "boxViewMatrix", data);
    MCGLContext_setUniforms(0, var(ctx), 0);
    
    glBindVertexArray(obj->vaoid);
    MCGLEngine_activeTextureUnit(0);
    //MCGLEngine_bindCubeTexture(obj->texid);
    
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, MCBUFFER_OFFSET(0));
    
    glBindVertexArray(0);
    glDepthMask(GL_TRUE);
}

onload(MCSkybox)
{
    if (load(MC3DNode)) {
        binding(MCSkybox, void, bye, voida);
        binding(MCSkybox, MCSkybox*, initWithCubeTexture, BECubeTextureData* cubetex, double widthHeightRatio);
        binding(MCSkybox, MCSkybox*, initWithFileNames, const char* namelist[], double widthHeightRatio);
        binding(MCSkybox, MCSkybox*, initWithDefaultFilesRatio, double widthHeightRatio);
        binding(MCSkybox, MCSkybox*, initWithDefaultFiles, voida);
        binding(MCSkybox, void, resizeWithWidthHeight, unsigned width, unsigned height);
        binding(MCSkybox, void, update, MCGLContext* ctx);
        binding(MCSkybox, void, draw, MCGLContext* ctx);
        return cla;
    }else{
        return null;
    }
}
