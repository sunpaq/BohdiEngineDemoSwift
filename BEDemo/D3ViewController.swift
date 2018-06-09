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
        if let models = BEResource.shared().objModelNames as? [String] {
            beview.loadModelNamed(models[AppDelegate.currentIndex])
            AppDelegate.currentIndex += 1
            if AppDelegate.currentIndex >= models.count {
                AppDelegate.currentIndex = 0
            }
        }
    }
    
    override func viewDidAppear(_ animated: Bool) {
        super.viewDidAppear(animated)
        //beview.renderer.setBackgroundColor(UIColor.black)
        beview.startDraw3DContent(BECameraRotateAroundModelManual)
    }
    
    override func viewWillDisappear(_ animated: Bool) {
        super.viewWillDisappear(animated)
        beview.stopDraw3DContent()
    }
    
    @IBAction func onFullscreen(_ sender: Any) {
        beview.frame = UIScreen.main.bounds
    }
    
    @IBAction func onPan(_ sender: Any) {
        let trans = (sender as! UIPanGestureRecognizer).translation(in: self.view)
        beview.renderer.rotateModel(byPanGesture: trans)
    }
    
    @IBAction func onPinch(_ sender: Any) {
        let zoom = (sender as! UIPinchGestureRecognizer).scale
        beview.renderer.zoomModel(byPinchGesture: zoom)
    }
}
