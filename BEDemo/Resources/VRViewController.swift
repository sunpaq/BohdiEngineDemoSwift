//
//  VRViewController.swift
//  BEDemo
//
//  Created by Sun YuLi on 2017/5/18.
//  Copyright © 2017年 SODEC. All rights reserved.
//

import UIKit
import GVRKit

class VRViewController: UIViewController, GVRRendererViewControllerDelegate {
    
    func renderer(for displayMode: GVRDisplayMode) -> GVRRenderer! {
        return VRRenderer()
    }
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        let renderer = VRRenderer()
        
        let ctl = GVRRendererViewController.init(renderer: renderer)!
        ctl.delegate = self
        ctl.view.frame = self.view.frame
        ctl.view.autoresizingMask = .flexibleWidth

        self.view.addSubview(ctl.view)
        self.addChildViewController(ctl)
    }
}
