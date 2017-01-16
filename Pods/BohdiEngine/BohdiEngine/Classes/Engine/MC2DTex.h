//
//  MC2DTex.h
//  monkcGame
//
//  Created by Sun YuLi on 16/4/13.
//  Copyright © 2016年 oreisoft. All rights reserved.
//

#ifndef MC2DTex_h
#define MC2DTex_h

#include <stdio.h>
#include "monkc.h"
#include "MCTexture.h"

class(MC2DTex, MCTexture);

method(MC2DTex, MC2DTex*, initWithFileName, const char* name);
method(MC2DTex, void, drawTexture, MCGLContext* ctx);

#endif /* MC2DTex_h */
