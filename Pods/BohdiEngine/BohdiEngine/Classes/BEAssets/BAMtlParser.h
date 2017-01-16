//
//  MC3DMtlParser.h
//  monkcGame
//
//  Created by Sun YuLi on 16/4/5.
//  Copyright © 2016年 oreisoft. All rights reserved.
//

#ifndef MC3DMtlParser_h
#define MC3DMtlParser_h

#include <stdio.h>
#include "monkc.h"
#include "MCMath.h"
#include "MCGeometry.h"
#include "MCString.h"
#include "MCLexer.h"

/* illum_0 -> illum_10
0   Color on and Ambient off 
1   Color on and Ambient on 
2   Highlight on 
3   Reflection on and Ray trace on 
4   Transparency: Glass on 
Reflection: Ray trace on 
5   Reflection: Fresnel on and Ray trace on 
6   Transparency: Refraction on 
Reflection: Fresnel off and Ray trace on 
7   Transparency: Refraction on 
Reflection: Fresnel on and Ray trace on 
8   Reflection on and Ray trace off 
9   Transparency: Glass on 
Reflection: Ray trace off 
10  Casts shadows onto invisible surfaces
*/

typedef enum {
    Ambient = 0,
    Diffuse,
    Specular,
    //transmission filter
    TFilter
} BALightType;

typedef enum {
    RGB,         //Ka r g b
    XYZ,         //Ka x y z (CIEXYZ color space)
    SpectralFile //Ka spectral file.rfl factor
} BAColorType;

typedef enum {
    SpecularExponent, //Ns
    DissolveFactor,   //d
    Disp,             //disp (surface roughness)
    Decal,            //decal
    Bump              //bump
} BAScalarType;

typedef struct {
    BAColorType Ctype;
    double factor;//default=1.0
    union {
        double rgbxyz[3];
        char spectral[256];
    } data;
} BALightColor;

/* texture map options
-blendu on | off 
-blendv on | off 
-cc on | off 
-clamp on | off 
-mm base gain 
-o u v w 
-s u v w 
-t u v w 
-texres value
*/

//map_Ka, map_Kd, map_Ks
typedef struct {
    BALightType Ltype;
    BAColorType Ctype;
    char filename[256];
    char options[256];
} BALightColorMap;

/* scalar map options
-blendu on | off 
-blendv on | off 
-clamp on | off 
-imfchan r | g | b | m | l | z 
-mm base gain 
-o u v w 
-s u v w 
-t u v w 
-texres value 
*/

//map_Ns, map_d, disp, decal, bump
typedef struct {
    BAScalarType type;
    char filename[256];
    char options[256];
} BAScalarMap;

/* reflection map options
-blendu on | off 
-blendv on | off 
-cc on | off 
-clamp on | off 
-mm base gain 
-o u v w 
-s u v w 
-t u v w 
-texres value 
*/

//refl -type
typedef struct {
    char filename[256];
    char options[256];
} BAReflectionMap;

typedef struct {
    //newmtl
    char name[256];
    //light color
    BALightColor lightColors[4];
    //illumination model 0->10
    int illumModelNum;
    //dissolve (d/d -halo)
    double dissolveFactor;
    double dissolveHaloFactor;
    //specular exponent (Ns) 0->1000
    double specularExponent;
    //sharpness value 0->1000 default 60
    int sharpnessValue;
    //index of refraction (Ni) 0.001->10
    int indexOfRefraction;    
    //texture map
    BALightColorMap lightColorMaps[10];
    BAScalarMap     scalarMaps[10];
    BAReflectionMap reflectionMaps[10];
} BAMaterial;

MCInline MCVector3 BAMaterialLightColor(BAMaterial* mat, BALightType type) {
    double R = mat->lightColors[type].data.rgbxyz[0];
    double G = mat->lightColors[type].data.rgbxyz[1];
    double B = mat->lightColors[type].data.rgbxyz[2];
    return MCVector3Make(R, G, B);
}

typedef struct BAMtlLibraryStruct {
    BAMaterial materials[256];
    //cursors
    int materialCursor;
    int lightColorMapCursor;
    int scalarMapCursor;
    int reflectionMapCursor;
    char name[256];
} BAMtlLibrary;

MCInline BAMaterial* BAFindMaterial(BAMtlLibrary* lib, const char* name) {
    if (lib && name) {
        for (int i=0; i<256; i++) {
            BAMaterial* mtl = &(lib->materials[i]);
            if (MCStringEqual(mtl->name, name)) {
                return mtl;
            }
        }
    }
    return null;
}

MCInline BAMtlLibrary* BAMtlLibraryAlloc() {
    BAMtlLibrary* lib = (BAMtlLibrary*)malloc(sizeof(BAMtlLibrary));
    if (lib) {
        //cursors
        lib->materialCursor = -1;
        lib->lightColorMapCursor = -1;
        lib->scalarMapCursor = -1;
        lib->reflectionMapCursor = -1;
        lib->name[0] = NUL;
        return lib;
    }
    return null;
}

MCInline void BAMtlLibraryResetCursor(BAMtlLibrary* lib) {
    if (lib->materialCursor != -1) {
        lib->materialCursor = 0;
    }
    if (lib->lightColorMapCursor != -1) {
        lib->lightColorMapCursor = 0;
    }
    if (lib->scalarMapCursor != -1) {
        lib->scalarMapCursor = 0;
    }
    if (lib->reflectionMapCursor != -1) {
        lib->reflectionMapCursor = 0;
    }
}

MCInline BAMaterial* currentMaterial(BAMtlLibrary* lib) {
    if (lib->materialCursor != -1) {
        return &(lib->materials[lib->materialCursor]);
    }else{
        return null;
    }
}

BAMtlLibrary* BAMtlLibraryNew(const char* filename);



#endif /* MC3DMtlParser_h */
