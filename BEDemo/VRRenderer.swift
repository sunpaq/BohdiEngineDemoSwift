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
    var models: [String] = []
    
    override func initializeGl() {
        super.initializeGl()
        
        if let paths = Bundle.main.urls(forResourcesWithExtension: "obj", subdirectory: nil) {
            for path in paths {
                let name = path.lastPathComponent
                models.append(name)
            }
        } else {
            return
        }
        
        renderer = BERenderer.init(frame: CGRect.init())
        renderer?.addModelNamed(models[currentModelIndex], scale: 1.0, rotateX: 0, tag: 11)
        
        if let controller = BEGameController.shared().gameController {
            controller.extendedGamepad?.buttonA.pressedChangedHandler = {
                button, value, pressed in
                if pressed {
                    self.renderer?.removeCurrentModel()
                    self.renderer?.removeCurrentSkysph()
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
                    self.renderer?.removeCurrentSkysph()
                    if self.currentModelIndex >= 0 && self.currentModelIndex < self.models.count {
                        self.renderer?.addModelNamed(self.models[self.currentModelIndex], scale: 1.0, rotateX: 0, tag: 12)
                        self.currentModelIndex -= 1
                    } else {
                        self.currentModelIndex = 0
                    }
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
            ren.rotateModel(byPanGesture: BEGameController.shared().leftStick)
            ren.rotateSkysph(byPanGesture: BEGameController.shared().leftStick)
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
            if let X = controller.extendedGamepad?.buttonX {
                if X.isPressed {
                    ren.removeCurrentModel()
                    ren.removeCurrentSkysph()
                }
            }
            if let Y = controller.extendedGamepad?.buttonY {
                if Y.isPressed {
                    ren.addSkysphNamed("panorama360.jpg")
                }
            }
        }
    }
}