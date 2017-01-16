//
//  MCGLEngine.h
//  monkcGame
//
//  Created by SunYuLi on 15/12/26.
//  Copyright © 2015年 oreisoft. All rights reserved.
//

#ifndef MCGLEngine_h
#define MCGLEngine_h

#include <stdio.h>
#include "monkc.h"
#include "MCGLBase.h"
#include "MC3DBase.h"
#include "MC3DShapeBase.h"
#include "MCClock.h"
#include "MCGLContext.h"

typedef enum {
    MCGLErrNone = GL_NO_ERROR,
    MCGLErrInvalidEnum = GL_INVALID_ENUM,
    MCGLErrInvalidValue = GL_INVALID_VALUE,
    MCGLErrInvalidOperation = GL_INVALID_OPERATION,
    MCGLErrOutOfMemory = GL_OUT_OF_MEMORY,
} MCGLEngineErr;

typedef struct {
    MCBool success;
    MCUInt vaoId;
    MCUInt vboId;
} MCGLEngineResponse;

//Global
utility(MCGLEngine, MCBool, isFeatureOn, MCGLFeature feature);
utility(MCGLEngine, void, featureSwith, MCGLFeature feature, MCBool onOrOff);
utility(MCGLEngine, void, flushCommandAsync, voida);
utility(MCGLEngine, void, flushCommandBlock, voida);
utility(MCGLEngine, void, clearScreen, voida);
utility(MCGLEngine, void, clearDepthBuffer, voida);
utility(MCGLEngine, void, clearStencilBuffer, voida);
utility(MCGLEngine, void, setClearScreenColor, MCColorf color);
utility(MCGLEngine, void, setPointSize, double pointsize);
utility(MCGLEngine, void, setLineWidth, double linewidth);
utility(MCGLEngine, void, setFrontCounterClockWise, MCBool isCCW);
utility(MCGLEngine, void, cullFace, MCGLFace face);
utility(MCGLEngine, void, cullBackFace, voida);
//Texture
utility(MCGLEngine, MCUInt, getMaxTextureUnits, voida);
utility(MCGLEngine, void, activeTextureUnit, MCUInt index);
utility(MCGLEngine, void, bindCubeTexture, MCUInt tid);
utility(MCGLEngine, void, bind2DTexture, MCUInt tid);
//Shader
utility(MCGLEngine, GLuint, createShader, voida);
utility(MCGLEngine, GLuint, prepareShader, GLuint Id, const char* vcode, const char* fcode);
utility(MCGLEngine, GLuint, prepareShaderName, GLuint Id, const char* vname, const char* fname);
utility(MCGLEngine, void, tryUseShaderProgram, GLuint Id);

//Frame Rate (FPS)
MCInline
utility(MCGLEngine, int, tickFPS, MCClock* clock)
{
    static unsigned fcount = 0;
    static clock_t elapse = 0;
    static clock_t time, lastime;
    
    MCClock_getCPUClocksSinceStart(0, clock, &time);
    if (elapse >= CLOCKS_PER_SEC ) {
        unsigned result = fcount;
        //reset
        elapse = 0;
        fcount = 0;
        lastime = time;
        
        return result;
    }else{
        elapse += (time - lastime);
        fcount++;
        return -1;
    }
}

//Shader
MCInline
utility(MCGLEngine, int, compileShader, GLuint* shader, GLenum type, const GLchar *source)
{
    if (!source) {
        return 0;
    }
    GLint status;
    
    *shader = glCreateShader(type);
    glShaderSource(*shader, 1, &source, NULL);
    glCompileShader(*shader);
    
    GLint logLength;
    glGetShaderiv(*shader, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0) {
        GLchar *log = (GLchar *)malloc(logLength);
        glGetShaderInfoLog(*shader, logLength, &logLength, log);
        printf("Shader compile log:\n%s", log);
        free(log);
    }
    
    glGetShaderiv(*shader, GL_COMPILE_STATUS, &status);
    if (status == 0) {
        glDeleteShader(*shader);
        return 0;
    }
    
    return 1;
}

MCInline
utility(MCGLEngine, int, linkProgram, GLuint prog)
{
    GLint status;
    glLinkProgram(prog);
    
    GLint logLength;
    glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0) {
        GLchar *log = (GLchar *)malloc(logLength);
        glGetProgramInfoLog(prog, logLength, &logLength, log);
        printf("Program link log:\n%s", log);
        free(log);
    }
    
    glGetProgramiv(prog, GL_LINK_STATUS, &status);
    if (status == 0) {
        return 0;
    }
    
    return 1;
}

MCInline
utility(MCGLEngine, int, validateProgram, GLuint prog)
{
    GLint logLength, status;
    
    glValidateProgram(prog);
    glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0) {
        GLchar *log = (GLchar *)malloc(logLength);
        glGetProgramInfoLog(prog, logLength, &logLength, log);
        printf("Program validate log:\n%s", log);
        free(log);
    }
    
    glGetProgramiv(prog, GL_VALIDATE_STATUS, &status);
    if (status == 0) {
        return 0;
    }
    
    return 1;
}

#endif /* MCGLEngine_h */
