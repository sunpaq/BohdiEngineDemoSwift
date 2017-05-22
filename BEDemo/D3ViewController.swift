//
//  3DViewController.swift
//  BEDemo
//
//  Created by Sun YuLi on 2017/5/22.
//  Copyright © 2017年 SODEC. All rights reserved.
//

import UIKit

class D3ViewController: UIViewController {
    
    @IBOutlet weak var beview: BEView!
    
    override func viewDidLoad() {
        super.viewDidLoad()
        beview.loadModelNamed("2.obj")
    }
    
    override func viewDidAppear(_ animated: Bool) {
        super.viewDidAppear(animated)
        beview.startDraw3DContent(BECameraRotateAroundModelManual)
    }
    
    override func viewWillDisappear(_ animated: Bool) {
        super.viewWillDisappear(animated)
        beview.stopDraw3DContent()
    }
    
    @IBAction func onFullscreen(_ sender: Any) {
        beview.frame = self.view.frame
        beview.renderer.scissorAllScene(self.view.frame)
    }
    
    @IBAction func onPan(_ sender: Any) {
        let trans = (sender as! UIPanGestureRecognizer).translation(in: self.view)
        beview.renderer.handlePanGesture(trans)
    }
    
    @IBAction func onPinch(_ sender: Any) {
        let zoom = (sender as! UIPinchGestureRecognizer).scale
        beview.renderer.handlePinchGesture(Float(zoom))
    }
}