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

    var context: EAGLContext?
    
    override func viewDidLoad() {
        super.viewDidLoad()
        self.context = EAGLContext.init(api: EAGLRenderingAPI.openGLES3)
        
        let glview = self.view as! GLKView
        glview.context = context!
        EAGLContext.setCurrent(self.context)
        
        let width  = UInt32(glview.bounds.width)
        let height = UInt32(glview.bounds.height)

        BESetupGL(width, height)
    }
    
    override func glkView(_ view: GLKView, drawIn rect: CGRect) {
        BEUpdateGL()
        BEDrawGL()
    }
    
    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
    }
}

