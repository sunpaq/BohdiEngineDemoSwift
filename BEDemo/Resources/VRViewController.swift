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
    
    @objc func drawFrame() {
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
        renderer.doesAutoRotateCamera = true
        renderer.addModelNamed("arcanegolem.obj", scale: 1.0, rotateX: 0, tag: 11)
    }
    
    func cardboardView(_ cardboardView: GVRCardboardView!, prepareDrawFrame headTransform: GVRHeadTransform!) {
        
    }
    
    func cardboardView(_ cardboardView: GVRCardboardView!, draw eye: GVREye, with headTransform: GVRHeadTransform!) {
        
        let fov = headTransform.fieldOfView(for: eye)
        renderer.cameraFOVReset(Float(fov.top + fov.bottom))
        
        let headmat = headTransform.headPoseInStartSpace()
        let eyemat  = headTransform.eye(fromHeadMatrix: eye)
        let mat4 = GLKMatrix4Multiply(eyemat, headmat)
        
        renderer.cameraTransformWorld(mat4)
        let viewport = headTransform.viewport(for: eye)
        renderer.scissorAllScene(viewport)
        renderer.drawFrame()

    }
}
