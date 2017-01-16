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
        var(transform) = MCMatrix4Identity();
        var(material) = null;
        var(texture) = null;
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
    //material
    if (obj->material != null) {
        obj->material->dataChanged = true;
        MCMatrial_prepareMatrial(0, obj->material, ctx);
    }
    
    //batch setup
    MCGLContext_activateShaderProgram(0, ctx, 0);
    MCGLContext_setUniforms(0, ctx, 0);
    
    //draw self meshes
    MCLinkedListForEach(var(meshes),
                        MCMesh* mesh = (MCMesh*)item;
                        if (mesh != null) {
                            MCMesh_prepareMesh(0, mesh, ctx);
                            MCMesh_drawMesh(0, mesh, ctx);
                        })
    //draw self texture
    if (obj->texture != null) {
        ff(obj->texture, drawTexture, ctx);
    }
    
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
        binding(MC3DNode, void, update, voida);
        binding(MC3DNode, void, draw, voida);
        binding(MC3DNode, void, hide, voida);
        binding(MC3DNode, void, show, voida);
        return cla;
    }else{
        return null;
    }
}
