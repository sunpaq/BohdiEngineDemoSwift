//
//  VRRenderer.swift
//  BEDemo
//
//  Created by 孙御礼 on 2/24/30 H.
//  Copyright © 30 Heisei SODEC. All rights reserved.
//

import UIKit
import GVRKit

class VRRenderer: GVRRenderer {

    var renderer: BERenderer? = nil
    var controller: BEGameController? = nil
    var currentModelIndex = 0
    var models: [String]!
    
    override func initializeGl() {
        super.initializeGl()
        models = BEResource.shared().objModelNames as! [String]
        
        renderer = BERenderer.init(frame: CGRect.init())
        renderer?.addModelNamed(models[currentModelIndex], scale: 1.0, rotateX: 0, tag: 11)
        
        if let controller = BEGameController.shared().gameController {
            controller.extendedGamepad?.buttonA.pressedChangedHandler = {
                button, value, pressed in
                if pressed {
                    self.renderer?.removeCurrentModel()
                    if self.currentModelIndex >= 0 && self.currentModelIndex < self.models.count {
                        self.renderer?.addModelNamed(self.models[self.currentModelIndex], scale: 1.0, rotateX: 0, tag: 12)
                        self.currentModelIndex += 1
                    } else {
                        self.currentModelIndex = 0
                    }
                }
            }
            controller.extendedGamepad?.buttonB.pressedChangedHandler = {
                button, value, pressed in
                if pressed {
                    self.renderer?.removeCurrentModel()
                    if self.currentModelIndex >= 0 && self.currentModelIndex < self.models.count {
                        self.renderer?.addModelNamed(self.models[self.currentModelIndex], scale: 1.0, rotateX: 0, tag: 12)
                        self.currentModelIndex -= 1
                    } else {
                        self.currentModelIndex = 0
                    }
                }
            }
            controller.extendedGamepad?.buttonX.pressedChangedHandler = {
                button, value, pressed in
                if pressed {
                    self.renderer?.removeCurrentModel()
                    self.renderer?.removeCurrentSkysph()
                }
            }
            controller.extendedGamepad?.buttonY.pressedChangedHandler = {
                button, value, pressed in
                if pressed {
                    self.renderer?.addSkysphNamed("panorama360.jpg")
                }
            }
        }
    }
    
    override func setSize(_ size: CGSize, andOrientation orientation: UIInterfaceOrientation) {
        if vrModeEnabled == false {
            renderer = renderer?.resizeAllScene(size)
        }
    }
    
    override func update(_ headPose: GVRHeadPose!) {
        if vrModeEnabled == true {
            renderer?.doesAutoRotateCamera = false
        } else {
            renderer?.doesAutoRotateCamera = true
        }
    }
    
    override func draw(_ headPose: GVRHeadPose!) {
        if let ren = renderer {
            if vrModeEnabled == true {
                ren.drawFrame(headPose.viewport,
                              vrHeadTransform: headPose.headTransform,
                              vrEyeTransform: headPose.eyeTransform,
                              vrFOV: headPose.fieldOfView.top + headPose.fieldOfView.bottom)
            } else {
                ren.drawFrame()
            }
            handleGameController(ren)
        }
    }
    
    func handleGameController(_ ren: BERenderer!) {
        if let controller = BEGameController.shared().gameController {
            ren.rotateModel(byPanGesture: CGPoint(x:BEGameController.shared().leftStick.x, y:0))
            ren.rotateModel(byPanGesture: CGPoint(x:0, y:BEGameController.shared().rightStick.y))

            if let trigger = controller.extendedGamepad?.leftTrigger {
                if trigger.isPressed {
                    ren.doesDrawWireFrame = true
                } else {
                    ren.doesDrawWireFrame = false
                }
            }
            if let trigger = controller.extendedGamepad?.rightTrigger {
                if trigger.isPressed {
                    ren.cameraTranslate(GLKVector3.init(v: (0,0,(1-trigger.value)*3+1)), incremental: true)
                } else {
                    ren.cameraTranslate(GLKVector3.init(v: (0,0,3)), incremental: true)
                }
            }
        }
    }
}
