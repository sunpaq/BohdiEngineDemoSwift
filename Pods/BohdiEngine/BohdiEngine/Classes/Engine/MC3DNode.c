//
//  MC3DNode.c
//  monkcGame
//
//  Created by Sun YuLi on 16/2/20.
//  Copyright © 2016年 oreisoft. All rights reserved.
//

#include "MC3DNode.h"
#include "MCGLEngine.h"
#include "MCLinkedList.h"
#include "MCGLRenderer.h"

oninit(MC3DNode)
{
    if (init(MCItem)) {
        var(visible) = true;
        var(center) = MCVector3Make(0, 0, 0);
        var(transform) = MCMatrix4Identity;
        var(material) = null;
        var(diffuseTexture) = null;
        var(specularTexture)= null;
        var(zorder) = -1;
        
        var(children) = new(MCLinkedList);
        var(meshes) = new(MCLinkedList);
        
        return obj;
    }else{
        return null;
    }
}

method(MC3DNode, void, bye, voida)
{
    release(var(material));
    release(var(diffuseTexture));
    release(var(specularTexture));
    release(var(children));
    release(var(meshes));
}

method(MC3DNode, MC3DErrCode, addChild, MC3DNode* child)
{
    child->visible = false;
    MCLinkedList_addItem(0, var(children), (MCItem*)child);
    child->visible = true;
    return MC3DSuccess;
}

method(MC3DNode, MC3DErrCode, removeChild, MC3DNode* child)
{
    child->visible = false;
    MCLinkedList_delItem(0, var(children), (MCItem*)child);
    return MC3DSuccess;
}

method(MC3DNode, void, copyChildrenFrom, MC3DNode* node)
{
    MCLinkedList_connectList(0, var(children), node->children);
}

method(MC3DNode, void, cleanUnvisibleChild, voida)
{
    MCLinkedListForEach(var(children),
                        MC3DNode* node = (MC3DNode*)item;
                        if (node != null && node->visible == false) {
                            MCLinkedList_delItem(0, var(children), (MCItem*)node);
                        })
}

method(MC3DNode, int, childCount, voida)
{
    return computed(obj->children, count);
}

method(MC3DNode, void, setAllVisible, MCBool visible)
{
    if (var(children)) {
        MCLinkedListForEach(var(children),
                            MC3DNode* node = (MC3DNode*)item;
                            if (node != null) {
                                node->visible = visible;
                            })
    }
}

method(MC3DNode, void, changeMatrial, MCMaterial* material)
{
    release(obj->material);
    obj->material = material;
}

method(MC3DNode, void, changeTexture, MCTexture* texture)
{
    release(obj->diffuseTexture);
    obj->diffuseTexture = texture;
}

method(MC3DNode, void, translate, MCVector3* position)
{
    obj->transform = MCMatrix4Multiply(MCMatrix4MakeTranslation(position->x, position->y, position->z), obj->transform);
}

method(MC3DNode, void, rotateX, double degree)
{
    //obj->transform = MCMatrix4Multiply(MCMatrix4MakeXAxisRotation(degree), obj->transform);
}

method(MC3DNode, void, rotateY, double degree)
{
    //obj->transform = MCMatrix4Multiply(MCMatrix4MakeYAxisRotation(degree), obj->transform);
}

method(MC3DNode, void, rotateZ, double degree)
{
    //obj->transform = MCMatrix4Multiply(MCMatrix4MakeZAxisRotation(degree), obj->transform);
}

method(MC3DNode, void, scale, MCVector3* factors)
{
    obj->transform = MCMatrix4Multiply(MCMatrix4MakeScale(factors->x, factors->y, factors->z), obj->transform);
}

method(MC3DNode, void, update, MCGLContext* ctx)
{
    MCGLUniform f;
    f.data.mat4 = var(transform);
    MCGLContext_updateUniform(0, ctx, model_model, f.data);
    
    MCMatrix3 nor = MCMatrix3InvertAndTranspose(MCMatrix4GetMatrix3(var(transform)), NULL);
    f.data.mat3 = nor;
    MCGLContext_updateUniform(0, ctx, model_normal, f.data);
    
    //update children
    MCLinkedListForEach(var(children),
                        MC3DNode* node = (MC3DNode*)item;
                        if (node != null && node->visible != false) {
                            ff(node, update, ctx);
                            //ff(node, update, ctx);
                        })
}

method(MC3DNode, void, draw, MCGLContext* ctx)
{
    MCGLContext_activateShaderProgram(0, ctx, 0);

    //material
    if (obj->material != null) {
        if (obj->material->hidden == 1) {
            return;
        }
        obj->material->dataChanged = true;
        MCMaterial_prepareMatrial(0, obj->material, ctx);
    }
    
    //draw self texture
    if (obj->diffuseTexture != null) {
        //ctx->diffuseTextureRef = obj->diffuseTexture;
        glUniform1i(glGetUniformLocation(ctx->pid, "usetexture"), true);
    } else {
        //ctx->diffuseTextureRef = null;
        glUniform1i(glGetUniformLocation(ctx->pid, "usetexture"), false);
    }
    
//    if (obj->specularTexture != null) {
//        ctx->specularTextureRef = obj->specularTexture;
//    } else {
//        ctx->specularTextureRef = null;
//    }
    
    //batch setup
    MCGLContext_setUniforms(0, ctx, 0);
    
    //draw self meshes
    MCLinkedListForEach(var(meshes),
                        MCMesh* mesh = (MCMesh*)item;
                        if (mesh != null) {
                            mesh->diffuseTextureRef  = obj->diffuseTexture;
                            mesh->specularTextureRef = obj->specularTexture;
                            MCMesh_prepareMesh(0, mesh, ctx);
                            MCMesh_drawMesh(0, mesh, ctx);
                        })
    
    //draw children
    MCLinkedListForEach(var(children),
                        MC3DNode* node = (MC3DNode*)item;
                        if (node != null && node->visible != false) {
                            ff(node, draw, ctx);
                        })
    
    //ff(ctx, printUniforms, 0);
}

method(MC3DNode, void, hide, voida)
{
    var(visible) = false;
}

method(MC3DNode, void, show, voida)
{
    var(visible) = true;
}

onload(MC3DNode)
{
    if (load(MCItem)) {
        binding(MC3DNode, void, bye, voida);
        binding(MC3DNode, void, addChild, MC3DNode* child);
        binding(MC3DNode, void, removeChild, MC3DNode* child);
        binding(MC3DNode, void, copyChildrenFrom, MC3DNode* node);
        binding(MC3DNode, void, cleanUnvisibleChild, voida);
        binding(MC3DNode, int, childCount, voida);
        binding(MC3DNode, void, setAllVisible, MCBool visible);
        binding(MC3DNode, void, changeMatrial, MCMaterial* material);
        binding(MC3DNode, void, changeTexture, MCTexture* texture);
        binding(MC3DNode, void, translate, MCVector3* position);
        binding(MC3DNode, void, rotateX, double degree);
        binding(MC3DNode, void, rotateY, double degree);
        binding(MC3DNode, void, rotateZ, double degree);
        binding(MC3DNode, void, scale, MCVector3* factors);
        binding(MC3DNode, void, update, voida);
        binding(MC3DNode, void, draw, voida);
        binding(MC3DNode, void, hide, voida);
        binding(MC3DNode, void, show, voida);
        return cla;
    }else{
        return null;
    }
}
