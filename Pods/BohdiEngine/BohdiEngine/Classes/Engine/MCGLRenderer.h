//
//  MCGLShader.h
//  monkcGame
//
//  Created by SunYuLi on 15/11/22.
//  Copyright © 2015年 oreisoft. All rights reserved.
//

/*
 Notice:
 
 You must set the window system on both platform
 as below: (RGBA=8888 Depth=24bit)
 
 //iOS
 <GLKViewController>
 view.drawableColorFormat = GLKViewDrawableColorFormatRGBA8888;
 view.drawableDepthFormat = GLKViewDrawableDepthFormat24;
 
 //Android
 <GLSurfaceView>
 setEGLConfigChooser(8, 8, 8, 8, 24, 0);
 setEGLContextClientVersion(3);
 */

#ifndef MCGLRenderer_h
#define MCGLRenderer_h

#include <stdio.h>
#include "monkc.h"
#include "MCGLBase.h"
#include "MC3DNode.h"
#include "MCGLContext.h"

//uniform mat4  view.view;
//uniform mat4  view.projection;
//uniform vec3  view.position;

//uniform mat4  model.model;
//uniform mat3  model.normal;

//uniform vec3 light.ambient;
//uniform vec3 light.diffuse;
//uniform vec3 light.specular;
//uniform vec3 light.color;
//uniform vec3 light.position;

//uniform vec3 material.ambient;
//uniform vec3 material.diffuse;
//uniform vec3 material.specular;
//uniform int  material.dissolve;
//uniform int  material.shininess;

//uniform sampler2D texsampler;

MCGlobalKey view_view       = "view_view";
MCGlobalKey view_projection = "view_projection";
MCGlobalKey view_position   = "view_position";

MCGlobalKey model_model     = "model_model";
MCGlobalKey model_normal    = "model_normal";

MCGlobalKey light_ambient   = "light_ambient";
MCGlobalKey light_diffuse   = "light_diffuse";
MCGlobalKey light_specular  = "light_specular";
MCGlobalKey light_color     = "light_color";
MCGlobalKey light_position  = "light_position";

MCGlobalKey material_ambient   = "material_ambient";
MCGlobalKey material_diffuse   = "material_diffuse";
MCGlobalKey material_specular  = "material_specular";
MCGlobalKey material_dissolve  = "material_dissolve";
MCGlobalKey material_shininess = "material_shininess";

MCGlobalKey texsampler = "texsampler";

class(MCGLRenderer, MCObject,
      MCGLContext* context;
);
//life cycle
method(MCGLRenderer, void, bye, voida);
method(MCGLRenderer, MCGLRenderer*, initWithShaderCodeString, const char* vcode, const char* fcode);
method(MCGLRenderer, MCGLRenderer*, initWithShaderFileName, const char* vshader, const char* fshader);
//draw
method(MCGLRenderer, void, updateNodes, MC3DNode* rootnode);
method(MCGLRenderer, void, drawNodes, MC3DNode* rootnode);


#endif /* MCGLRenderer_h */
