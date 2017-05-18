//
//  VRViewController.swift
//  BEDemo
//
//  Created by Sun YuLi on 2017/5/18.
//  Copyright © 2017年 SODEC. All rights reserved.
//

import UIKit

class VRViewController: UIViewController, GVRCardboardViewDelegate {

    var cardboardView: GVRCardboardView!
    var renderer: BERenderer!
    var runloop: BERunLoop!
    
    override func viewDidLoad() {
        super.viewDidLoad()
        //set viewer
        //let url = Bundle.main.url(forResource: "baofengXD", withExtension: "txt")

        //runloop
        runloop = BERunLoop.init(target: self, selector: #selector(drawFrame))
        //view
        cardboardView = GVRCardboardView.init(frame: self.view.frame)
        cardboardView.hidesTransitionView = true
        cardboardView.vrModeEnabled = true
        cardboardView.delegate = self
        self.view.addSubview(cardboardView)
    }
    
    override func viewDidAppear(_ animated: Bool) {
        super.viewDidAppear(animated)
        runloop.startRunloop()
    }
    
    func drawFrame() {
        cardboardView.render()
    }
    
    //MARK: GVRSDK
    func cardboardView(_ cardboardView: GVRCardboardView!, didFire event: GVRUserEvent) {
        if event == GVRUserEvent.backButton {
            self.dismiss(animated: true, completion: {
                //done
            })
        }
    }
    
    func cardboardView(_ cardboardView: GVRCardboardView!, shouldPauseDrawing pause: Bool) {
        
    }
    
    func cardboardView(_ cardboardView: GVRCardboardView!, willStartDrawing headTransform: GVRHeadTransform!) {
        renderer = BERenderer.init(frame: CGRect.init(x: self.view.frame.origin.x,
                                                      y: self.view.frame.origin.y,
                                                      width: self.view.frame.width,
                                                      height: self.view.frame.height / 2.0))
        //renderer.setCameraRotateMode(BECameraFixedAtOrigin)
        renderer.setCameraRotateMode(BECameraRotateAroundModelManual)
        renderer.doesAutoRotateCamera = true
        renderer.addModelNamed("2.obj", scale: 1.0, rotateX: 0, tag: 11)
        //renderer.setCameraRotateMode(BECameraRotateAroundModelByGyroscope)
        //renderer.addSkysphNamed("panorama360.jpg")
    }
    
    func cardboardView(_ cardboardView: GVRCardboardView!, prepareDrawFrame headTransform: GVRHeadTransform!) {
        
    }
    
    func cardboardView(_ cardboardView: GVRCardboardView!, draw eye: GVREye, with headTransform: GVRHeadTransform!) {
        
        // Set the viewport.
        let viewport = headTransform.viewport(for: eye)

        glEnable(GLenum(GL_DEPTH_TEST));
        glEnable(GLenum(GL_SCISSOR_TEST));
        glViewport(GLint(viewport.origin.x),
                   GLint(viewport.origin.y),
                   GLsizei(viewport.width),
                   GLsizei(viewport.height))
        glScissor(GLint(viewport.origin.x),
                  GLint(viewport.origin.y),
                  GLsizei(viewport.width),
                  GLsizei(viewport.height))
        
        let fov = headTransform.fieldOfView(for: eye)
        renderer.cameraFOVReset(Float(fov.top + fov.bottom))
        
        //let mat4 = headTransform.headPoseInStartSpace()
        //renderer.deviceRotate(mat4)
        //renderer.cameraTransformWorld(mat4)
        renderer.resizeAllScene(viewport.size)
        //renderer.updateModelTag(11, poseMat4F: )
        renderer.drawFrame()
    }
}
