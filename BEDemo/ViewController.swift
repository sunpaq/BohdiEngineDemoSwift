//
//  ViewController.swift
//  BEDemo
//
//  Created by YuliSun on 16/01/2017.
//  Copyright © 2017 SODEC. All rights reserved.
//

import UIKit
import GLKit

class ViewController: GLKViewController {
    
    @objc static var instance: ViewController!
    var indicator: UIActivityIndicatorView!
    
    @objc func BEStartLoading() {
        indicator.center = self.view.center
        self.view.addSubview(indicator!)
        indicator.startAnimating()
    }
    
    @objc func BEStopLoading() {
        indicator.stopAnimating()
        indicator.removeFromSuperview()
    }
    
    override func viewDidLoad() {
        super.viewDidLoad()
        ViewController.instance = self
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
    }
    
    override func viewWillLayoutSubviews() {
        let width  = UInt32(self.view.bounds.width)
        let height = UInt32(self.view.bounds.height)
        BEResizeGL(width, height)
    }
    
    override func glkView(_ view: GLKView, drawIn rect: CGRect) {
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

