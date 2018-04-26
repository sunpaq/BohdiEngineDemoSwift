//
//  BEAssetsManager.c
//  Sapindus
//
//  Created by Sun YuLi on 16/4/30.
//  Copyright © 2016年 oreisoft. All rights reserved.
//

#ifdef __APPLE__
#include "TargetConditionals.h"
#include <CoreFoundation/CoreFoundation.h>
#include <pthread.h>
static CFStringRef BundlePath = NULL;
#endif
#include "BEAssetsManager.h"
#include "MCString.h"
#ifdef __ANDROID__
static AAssetManager* assetManager_ = null;
static ANativeWindow* window_ = null;

//File
void MCFileSetAssetManager(AAssetManager* assetManager)
{
	assetManager_ = assetManager;
}

AAssetManager* MCFileGetAssetManager()
{
    return assetManager_;
}
#endif

int MCFileGetPath(const char* filename, char* buffer)
{
    return MCFileGetPathFromBundle(NULL, filename, buffer);
}

int MCFileGetPathFromBundle(const char* bundlename, const char* filename, char* buffer)
{
    if (isFilename(filename) == false) {
        printf("MCFileGetPath - filename malformed: %s\n", filename);
        return -1;
    }
    
    char basename[256] = {0};
    char extension[64] = {0};
    
    if (MCString_extensionFromFilename(filename, basename, extension) > 0) {
        debug_log("MCFileGetPath - filename/basename/extension -> %s/%s/%s\n", filename, basename, extension);
    } else {
        debug_log("MCFileGetPath - filename/basename/no extension -> %s/%s\n", filename, basename);
    }
    
#ifdef __ANDROID__
    if (assetManager_ != null) {
        const char* subpath;
        if (strcmp(extension, "fsh") == 0) {
            subpath = "shaders";
        } else if (strcmp(extension, "vsh") == 0) {
            subpath = "shaders";
        } else if (strcmp(extension, "obj") == 0 || strcmp(extension, "mtl") == 0) {
            subpath = "raw";
        } else if (strcmp(extension, "png") == 0) {
            subpath = "textures";
        } else if (strcmp(extension, "jpg") == 0) {
            subpath = "textures";
        } else if (strcmp(extension, "tga") == 0) {
            subpath = "textures";
        } else if (strcmp(extension, "dds") == 0) {
            subpath = "textures";
        } else {
            subpath = "raw";
            error_log("can not detect(%s) use raw folder\n", extension);
        }
        
        char fullname[PATH_MAX] = {};
		sprintf(fullname, "%s.%s", basename, extension);
        AAssetDir* rootdir = AAssetManager_openDir(assetManager_, subpath);
        if (rootdir) {
            const char* name;
            char fullpath[PATH_MAX] = {0};
            while ((name=AAssetDir_getNextFileName(rootdir)) != NULL) {
                if (strcmp(fullname, name) == 0) {
                    sprintf(fullpath, "%s/%s", subpath, name);
                    strcpy(buffer, fullpath);
                }
            }
        }else{
            error_log("can not find rootdir\n");
            return -1;
        }
    }
    return 0;
#else

    if (BundlePath == NULL) {
        //static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
        //pthread_mutex_lock(&lock);
        CFBundleRef bundle = NULL;
        if (bundlename) {
            CFStringRef bid = CFStringCreateWithCString(kCFAllocatorDefault, bundlename, kCFStringEncodingUTF8);
            bundle = CFBundleGetBundleWithIdentifier(bid);
            CFRelease(bid);
        } else {
            bundle = CFBundleGetMainBundle();
        }
        if (!bundle) {
            error_log("BEAssetManager can not find bundle (%s)\n", bundlename);
            //pthread_mutex_unlock(&lock);
            return -1;
        }
        CFURLRef url = CFBundleCopyBundleURL(bundle);
        if (!url) {
            error_log("BEAssetManager can not find path of (%s).(%s)\n", basename, extension);
            //pthread_mutex_unlock(&lock);
            return -1;
        }
        BundlePath = CFURLCopyPath(url);
        CFRelease(url);
        //pthread_mutex_unlock(&lock);
    }
    
    char rootpath[PATH_MAX] = {0};
    CFStringGetCString(BundlePath, rootpath, PATH_MAX, kCFStringEncodingUTF8);
    
#if TARGET_OS_OSX
    strcat(rootpath, "Contents/Resources/");
    strcat(rootpath, filename);
#else
    strcat(rootpath, filename);
#endif
    MCStringFillLimited(buffer, rootpath, strlen(rootpath));
    return 0;

#endif
}

const char* MCFileCopyContentWithPathGetBufferSize(const char* filepath, off_t* buffsize)
{
#ifdef __ANDROID__
    if (assetManager_ != null) {
        debug_log("MCFileCopyContentWithPath %s\n", filepath);
        AAsset* f = AAssetManager_open(assetManager_, filepath, AASSET_MODE_BUFFER);
        if (f) {
            const char* abuff = AAsset_getBuffer(f);
            if (abuff) {
                off_t size = AAsset_getLength(f);
                char* buff = (char*)malloc((size + 1) * sizeof(char));
                memcpy(buff, abuff, size);
                buff[size] = NUL;
                AAsset_close(f);
                if (buffsize) {
                    *buffsize = size;
                }
                return buff;
            }else{
                error_log("MCFileCopyContentWithPath(%s) AAsset_getBuffer() failed\n", filepath);
            }
        }else{
            error_log("MCFileCopyContentWithPath(%s) Android assetManager_ can not open\n", filepath);
        }
    }
    error_log("MCFileCopyContent(%s) Android assetManager_ is null\n", filepath);
    return null;
#else
    char decodepath[PATH_MAX] = {0};
    FILE* f = fopen(MCString_percentDecode(filepath, decodepath), "r");
    if (f) {
        fseek(f, 0, SEEK_END);
        off_t size = ftell(f) + 1;
        fseek(f, 0, SEEK_SET);
        char* buffer = (char*)malloc(size);
        if (!buffer) {
            error_log("MCFileCopyContent(%s) can not alloc buffer\n", filepath);
            return null;
        }
        memset(buffer, 0, size);
        //copy
        fread(buffer, 1, size, f);
        fclose(f);
        if (buffsize) {
            *buffsize = size;
        }
        return buffer;
    }else{
        error_log("MCFileCopyContent(%s) fopen return null\n", filepath);
        return null;
    }

#endif
}

const char* MCFileCopyContentWithPath(const char* filepath)
{
    return MCFileCopyContentWithPathGetBufferSize(filepath, null);
}

void MCFileReleaseContent(const char* buff)
{
	free(buff);
}
