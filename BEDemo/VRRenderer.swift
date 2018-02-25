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
    
    override func initializeGl() {
        super.initializeGl()
        renderer = BERenderer.init(frame: CGRect.init())
        renderer?.addModelNamed("arcanegolem.obj", scale: 1.0, rotateX: 0, tag: 11)
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
        if let controller = BEGameController.shared() {
            ren.rotateModel(byPanGesture: controller.leftStick)
            if let trigger = controller.gameController.extendedGamepad?.rightTrigger {
                if trigger.isPressed {
                    ren.cameraTranslate(GLKVector3.init(v: (0,0,(1-trigger.value)*3+1)), incremental: true)
                } else {
                    ren.cameraTranslate(GLKVector3.init(v: (0,0,3)), incremental: true)
                }
            }
            if let X = controller.gameController.extendedGamepad?.buttonX {
                if X.isPressed {
                    ren.removeCurrentModel()
                }
            }
            if let A = controller.gameController.extendedGamepad?.buttonA {
                if A.isPressed {
                    ren.addModelNamed("2.obj", scale: 1.0, rotateX: 0, tag: 12)
                }
            }
            if let B = controller.gameController.extendedGamepad?.buttonB {
                if B.isPressed {
                    ren.addModelNamed("arcanegolem.obj", scale: 1.0, rotateX: 0, tag: 11)
                }
            }
        }
    }
}
