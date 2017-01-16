#include "BAObjParser.h"
#include "BEAssetsManager.h"
#include "MCGeometry.h"
#include "MCIO.h"

static size_t epcount = 0;

void parseObjMeta(BAObjMeta* meta, const char* buff)
{
    BAObjMetaInit(meta);
    if (meta && buff) {
        char line[LINE_MAX]; char* c = (char*)buff;
        while (*c!=NUL) {
            //skip '\n' when '\r\n'
            if (*c==MCNewLineN || *c==MCNewLineR) {
                c++; continue;
            }
            for (int i=0; !isNewLine(c); c++) {
                line[i++] = *c;
                line[i] = NUL;
            }
            //process line
            const char* remain = line; char word[256];
            MCToken token = tokenize(nextWord(&remain, word));
            if (token.type == MCTokenIdentifier) {
                if (MCStringEqualN(word, "vt", 2)) {
                    meta->texcoord_count++;
                }
                else if (MCStringEqualN(word, "vn", 2)) {
                    meta->normal_count++;
                }
                else if (MCStringEqualN(word, "v", 1)) {
                    meta->vertex_count++;
                }
                else if (MCStringEqualN(word, "f", 1)) {
                    meta->face_count++;
                }
                else if (MCStringEqualN(word, "o", 1)) {
                    meta->object_starts[meta->object_count] = meta->face_count;
                    meta->object_count++;
                }
                else if (MCStringEqualN(word, "g", 1)) {
                    meta->group_starts[meta->group_count] = meta->face_count;
                    meta->group_count++;
                }
                else if (MCStringEqualN(word, "mtllib", 6)) {
                    meta->mtllib_count++;
                }
                else if (MCStringEqualN(word, "usemtl", 6)) {
                    MCToken token = tokenize(nextWord(&remain, word));
                    if (token.type == MCTokenIdentifier) {
                        meta->usemtl_starts[meta->usemtl_count] = meta->face_count;
                        meta->usemtl_count++;
                        //debug_log("usemtl %s\n", token.value.Word);
                    }
                }
            }
        }
    }
}

void parseObj(BAObj* object, const char* file)
{
    if (object && file) {
        size_t vcursor = 0;
        size_t tcursor = 0;
        size_t ncursor = 0;
        size_t fcursor = 0;
        //size_t mcursor = 0;
        size_t ucursor = 0;
        BAMtlLibrary* current_mtllib = null;
        
        char line[LINE_MAX]; char* c = (char*)file;
        while (*c != NUL && *c != EOF) {
            //skip '\n' when '\r\n'
            if (*c==MCNewLineN || *c==MCNewLineR) {
                c++; continue;
            }
            for (int i=0; !isNewLine(c); c++) {
                line[i++] = *c;
                line[i] = NUL;
            }
            //process line
            //debug_log("process line: %s\n", line);
            char word[256]; const char* remain = line;
            MCToken token = tokenize(nextWord(&remain, word));
            switch (token.type) {
                case MCTokenIdentifier:
                    //don't change the order!
                    if (MCStringEqualN(word, "vt", 2)) {
                        double fbuff[LINE_MAX];
                        size_t n = nextFloats(&remain, fbuff);
                        for (int i=0; i<n; i++)
                            object->texcoorbuff[tcursor].v[i] = (float)fbuff[i];
                        tcursor++;
                    }
                    else if (MCStringEqualN(word, "vn", 2)) {
                        double fbuff[LINE_MAX];
                        size_t n = nextFloats(&remain, fbuff);
                        for (int i=0; i<n; i++)
                            object->normalbuff[ncursor].v[i] = (float)fbuff[i];
                        ncursor++;
                    }
                    else if (MCStringEqualN(word, "vp", 2)) {
                        //parameter
                    }
                    else if (MCStringEqualN(word, "v", 1)) {
                        double fbuff[LINE_MAX];
                        size_t n = nextFloats(&remain, fbuff);
                        for (int i=0; i<n; i++)
                            object->vertexbuff[vcursor].v[i] = fbuff[i];
                        vcursor++;
                    }
                    else if (MCStringEqualN(word, "f", 1)) {
                        //peek next value
                        token = tokenize(peekNext(&remain, word));
                        if (token.type == MCTokenDate) {
                            long lbuff[LINE_MAX];
                            BAFace* f = &object->facebuff[fcursor];
                            f->vcount = nextDates(&remain, lbuff);
                            if (f->vcount < 6) {
                                error_log("[%s] -> detect a face have less then 3 vertex, ignore it\n", line);
                            } else {
                                BAFaceInit(f, lbuff, f->vcount);
                                fcursor++;
                            }
                        }
                        if (token.type == MCTokenInteger) {
                            long lbuff[LINE_MAX];
                            BAFace* f = &object->facebuff[fcursor];
                            f->vcount = nextIntegers(&remain, lbuff);
                            if (f->vcount < 3) {
                                error_log("[%s] -> detect a face have less then 3 vertex, ignore it\n", line);
                            } else {
                                BAFaceInit(f, lbuff, f->vcount);
                                fcursor++;
                            }
                        }
                    }
                    else if (MCStringEqualN(word, "g", 1)) {
                        nextWord(&remain, word);
                        continue;
                    }
                    else if (MCStringEqualN(word, "o", 1)) {
                        token = tokenize(nextWord(&remain, word));
                        if (token.type == MCTokenIdentifier) {
                            MCStringFill(object->name, token.value.Word);
                        }
                        continue;
                    }
                    else if (MCStringEqualN(word, "mtllib", 6)) {
                        token = tokenize(nextWord(&remain, word));
                        if (token.type == MCTokenFilename) {
                            if ((current_mtllib=BAMtlLibraryNew(token.value.Word)) != null) {
                                if (!MCStringEqual(current_mtllib->name, token.value.Word)) {
                                    free(current_mtllib);
                                    current_mtllib = BAMtlLibraryNew(token.value.Word);
                                }
                            } else {
                                error_log("can not create mtl library: %s\n", word);
                                current_mtllib = null;
                            }
                        }
                    }
                    else if (MCStringEqualN(word, "usemtl", 6)) {
                        token = tokenize(nextWord(&remain, word));
                        if (token.type == MCTokenIdentifier) {
                            BAMaterial* mtl = null;
                            mtl = BAFindMaterial(current_mtllib, token.value.Word);
                            if (mtl) {
                                if (ucursor < object->usemtlcount) {
                                    object->usemtlbuff[ucursor++] = (BAMaterial)(*mtl);
                                }else{
                                    break;
                                }
                            }
                        }
                    }
                    else {
                        
                    }
                    
                    break;
                case MCTokenComment:
                case MCTokenUnknown:
                default:
                    break;
            }
        }
        //clean up
        if (current_mtllib) {
            free(current_mtllib);
        }
    }
}

BAObj* BAObjNew(const char* filename, BAObjMeta* meta)
{
    epcount = 0;
    
    const char* assetbuff;
//    if (isFilename(filename)) {
//        char noext[256];
//        MCString_filenameTrimExtension(filename, &noext);
//        assetbuff = MCFileCopyContent(noext, "obj");
//    }else{
        assetbuff = MCFileCopyContentWithPath(filename);
//    }

    if (assetbuff) {
        parseObjMeta(meta, assetbuff);
        if (meta->face_count <= 0 || meta->vertex_count <= 0) {
            error_log("MC3DObjParser - object face count is ZERO\n");
            return null;
        }
        if (meta->normal_count == 0 || meta->normal_count < meta->vertex_count) {
            error_log("MC3DObjParser modle need calculate normal\n");
        }
        
        BAObj* buff = BAObjAlloc(meta);
        if (!buff) {
            return null;
        }
        
        debug_log("MC3DObjParser - before parse\n");
        parseObj(buff, assetbuff);
        debug_log("MC3DObjParser - after parse\n");
        
        free((void*)assetbuff);
        return buff;
    }else{
        error_log("MC3DObjParser - AAssetManager_open %s failed\n", filename);
        return null;
    }
}

