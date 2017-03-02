//
//  ViewController.swift
//  BEDemo
//
//  Created by YuliSun on 16/01/2017.
//  Copyright © 2017 SODEC. All rights reserved.
//

import UIKit
import GLKit
import CoreMotion

class GLViewController: GLKViewController {
    
    var motionManager: CMMotionManager!
    var referenceAttitude: CMAttitude?
    
    //@objc static var instance: ViewController!
    var indicator: UIActivityIndicatorView!
    
    func startLoading(closure: @escaping ()->()) {
        indicator.center = self.view.center
        self.view.addSubview(indicator!)
        indicator.startAnimating()
        
        DispatchQueue.global().async {
            closure()
            self.stopLoading()
        }
    }
    
    func stopLoading() {
        DispatchQueue.main.async {
            self.indicator.stopAnimating()
            self.indicator.removeFromSuperview()
        }
    }
    
    func startDeviceMotion() {
        self.motionManager = CMMotionManager();
        if !self.motionManager.isDeviceMotionActive {
            self.motionManager.deviceMotionUpdateInterval = 1.0/60.0;
            self.motionManager.startDeviceMotionUpdates(using: CMAttitudeReferenceFrame.xArbitraryZVertical);
        }
    }
    
    func stopDeviceMotion() {
        self.motionManager.stopDeviceMotionUpdates();
    }
    
    func saveReferenceAttitude() {
        self.referenceAttitude = self.motionManager.deviceMotion?.attitude;
        
    }
    
    func getDeltaAttitude() -> CMAttitude? {
        if self.referenceAttitude == nil {
            saveReferenceAttitude();
        }

        let att = self.motionManager.deviceMotion?.attitude;
        att?.multiply(byInverseOf: self.referenceAttitude!);
        return att;
    }
    
    override func viewDidLoad() {
        super.viewDidLoad()
        //ViewController.instance = self
        indicator = UIActivityIndicatorView.init(activityIndicatorStyle: UIActivityIndicatorViewStyle.whiteLarge)
        
        //setup GL context
        EAGLContext.setCurrent(EAGLContext.init(api: EAGLRenderingAPI.openGLES3))
        (self.view as! GLKView).context = EAGLContext.current()
        (self.view as! GLKView).drawableColorFormat = GLKViewDrawableColorFormat.RGBA8888
        (self.view as! GLKView).drawableDepthFormat = GLKViewDrawableDepthFormat.format24
        (self.view as! GLKView).drawableStencilFormat = GLKViewDrawableStencilFormat.format8
        (self.view as! GLKView).drawableMultisample = GLKViewDrawableMultisample.multisampleNone

        self.preferredFramesPerSecond = 60
        
        //setup BohdiEngine
        let width  = UInt32(self.view.bounds.width)
        let height = UInt32(self.view.bounds.height)
        BESetupGL(width, height)

        startLoading(closure: { BEAddModelNamed("2.obj") })

        BESetRotateCamera(LandingViewController.instance.rotateCameraSwitch.isOn)
        BESetWireFrameMode(LandingViewController.instance.wireFrameSwitch.isOn)
        
        //setup core motion
        startDeviceMotion()
    }
    
    override func viewWillLayoutSubviews() {
        let width  = UInt32(self.view.bounds.width)
        let height = UInt32(self.view.bounds.height)
        BEResizeGL(width, height)
    }
    
    override func glkView(_ view: GLKView, drawIn rect: CGRect) {
        if let att = getDeltaAttitude() {
            let mat = att.rotationMatrix
            BEPullRotateMatrix(mat.m11, mat.m12, mat.m13,
                               mat.m21, mat.m22, mat.m23,
                               mat.m31, mat.m32, mat.m33)
        }
        BEUpdateGL()
        BEDrawGL()
    }
    
    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
    }
    
    @IBAction func onPan(_ sender: Any) {
        let trans = (sender as! UIPanGestureRecognizer).translation(in: self.view)
        BEPanGesture(Float(trans.x), Float(trans.y))
    }
    
    @IBAction func onPinch(_ sender: Any) {
        let zoom = (sender as! UIPinchGestureRecognizer).scale
        BEPinchGesture(Float(zoom))
    }
    
}

