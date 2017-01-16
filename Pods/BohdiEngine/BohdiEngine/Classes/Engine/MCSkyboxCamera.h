//
//  MCSkyboxCamera.h
//  Sapindus
//
//  Created by Sun YuLi on 16/5/1.
//  Copyright © 2016年 oreisoft. All rights reserved.
//

#ifndef MCSkyboxCamera_h
#define MCSkyboxCamera_h

#include "MCCamera.h"

class(MCSkyboxCamera, MCCamera,
      //matrix
      computing(MCMatrix4, viewMatrix);
      computing(MCMatrix4, projectionMatrix);
      //uniforms
      computing(MCGLUniform, viewUniform);
      computing(MCGLUniform, projectionUniform);
);

//override
method(MCSkyboxCamera, void, bye, voida);
method(MCSkyboxCamera, MCSkyboxCamera*, initWithWidthHeightRatio, MCFloat ratio);
method(MCSkyboxCamera, void, move, MCFloat deltaFai, MCFloat deltaTht);
method(MCSkyboxCamera, void, update, MCGLContext* ctx);
method(MCSkyboxCamera, void, setAttitude, MCFloat fai, MCFloat tht);

#endif /* MCSkyboxCamera_h */
