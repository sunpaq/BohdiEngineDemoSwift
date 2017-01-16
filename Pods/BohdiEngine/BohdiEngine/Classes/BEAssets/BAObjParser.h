//
//  MC3DFileParser.h
//  monkcGame
//
//  Created by Sun YuLi on 16/3/17.
//  Copyright © 2016年 oreisoft. All rights reserved.
//

#ifndef MC3DFileParser_h
#define MC3DFileParser_h

#include <stdio.h>
#include "monkc.h"
#include "MCMath.h"
#include "MCGeometry.h"
#include "MCLexer.h"
#include "BEAssetsManager.h"
#include "BAMtlParser.h"

typedef struct {
    long*  big;
    long   small[18];
    long*  data;
    size_t vcount;
} BAFace;

MCInline void BAFaceInit(BAFace* face, long* buff, size_t vcount)
{
    face->big = null;
    memset(&face->small[0], 0, sizeof(face->small));
    size_t size = sizeof(long) * vcount;
    if (vcount <= 18) {
        memcpy(face->small, buff, size);
        face->data = face->small;
    }else{
        face->big = (long*)malloc(size);
        memcpy(face->big, buff, size);
        face->data = face->big;
    }
    face->vcount = vcount;
}

typedef union {
    struct {
        double xmax;
        double xmin;
        double ymax;
        double ymin;
        double zmax;
        double zmin;
    };
    double m[6];
} BACubeFrame;

typedef struct {
    size_t vertex_count;
    size_t texcoord_count;
    size_t normal_count;
    size_t face_count;
    
    size_t object_count;
    size_t group_count;
    size_t usemtl_count;
    
    size_t object_starts[256];
    size_t group_starts[1024];
    size_t usemtl_starts[1024];
    
    size_t mtllib_count;
} BAObjMeta;

MCInline void BAObjMetaInit(BAObjMeta* meta) {
    meta->vertex_count    = 0;
    meta->texcoord_count  = 0;
    meta->normal_count    = 0;
    meta->face_count      = 0;

    meta->object_count    = 0;
    meta->group_count     = 0;
    meta->usemtl_count    = 0;
    
    memset(meta->object_starts, 0, 256);
    memset(meta->group_starts, 0, 1024);
    memset(meta->usemtl_starts, 0, 1024);
    
    meta->mtllib_count    = 0;
}

typedef struct BAObjStruct {
    BACubeFrame Frame;
    //raw data
    MCVector3* vertexbuff;
    MCVector2* texcoorbuff;
    MCVector4* normalbuff;//use w record times
    size_t  normal_count;
    //faces
    BAFace* facebuff;
    size_t  facecount;
    //mtl
    //BAMtlLibrary* mlibbuff;
    BAMaterial* usemtlbuff;
    //size_t mlibcount;
    size_t usemtlcount;
    char name[256];
} BAObj;

MCInline void BAObjDumpInfo(BAObj* baobj)
{
    for (int i=0; i<baobj->usemtlcount; i++) {
        BAMaterial* mtl = &baobj->usemtlbuff[i];
        debug_log("BAMaterial:%s\n", mtl->name);
    }
//    BAFace* faces = &baobj->facebuff[0];
//    for (int i=0; i<baobj->facecount; i++) {
//        BAFace* f = &faces[i];
//        printf("BAFace(%ld): ", f->vcount);
//        for (int j=0; j<f->vcount; j++) {
//            printf("%ld,", f->data[j]);
//        }
//        printf("\n");
//    }
}

MCInline BAObj* BAObjAlloc(BAObjMeta* meta)
{
    BAObj* buff = (BAObj*)malloc(sizeof(BAObj));
    if (buff) {
        buff->Frame = (BACubeFrame){};
        buff->vertexbuff  = (MCVector3*)malloc(sizeof(MCVector3) * (meta->vertex_count));
        buff->texcoorbuff = (MCVector2*)malloc(sizeof(MCVector2) * (meta->texcoord_count));
        buff->normalbuff  = (MCVector4*)malloc(sizeof(MCVector4) * (meta->normal_count));
        buff->normal_count = meta->normal_count;
        buff->facebuff    = (BAFace*)malloc(sizeof(BAFace)       * (meta->face_count));
        buff->facecount   = meta->face_count;
        //buff->mlibbuff    = (BAMtlLibrary*)malloc(sizeof(BAMtlLibrary) * meta->mtllib_count);
        buff->usemtlbuff  = (BAMaterial*)malloc(sizeof(BAMaterial) * meta->usemtl_count);
        //buff->mlibcount   = meta->mtllib_count;
        buff->usemtlcount = meta->usemtl_count;
        
        if (buff->vertexbuff && buff->texcoorbuff && buff->normalbuff && buff->facebuff) {
            buff->name[0] = NUL;
            return buff;
        }
    }
    error_log("BAObjParser - BAObjAlloc failed. face count is %d\n", meta->face_count);
    return null;
}

MCInline void BAObjRelease(BAObj* buff)
{
    //recursively
    if (buff) {
        //clean up self
        for (int i=0; i<buff->facecount; i++) {
            BAFace* f = &buff->facebuff[i];
            if (f->big) {
                free(f->big);
            }
        }
        free(buff->facebuff);
        free(buff->vertexbuff);
        free(buff->texcoorbuff);
        free(buff->normalbuff);
        //free(buff->mlibbuff);
        free(buff->usemtlbuff);
        free(buff);
    }
}

BAObj* BAObjNew(const char* filename, BAObjMeta* meta);

#endif /* MC3DFileParser_h */
