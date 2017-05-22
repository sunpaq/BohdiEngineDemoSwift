//
//  PanoramaViewController.swift
//  BEDemo
//
//  Created by Sun YuLi on 2017/5/22.
//  Copyright © 2017年 SODEC. All rights reserved.
//

import UIKit

class PanoramaViewController: UIViewController {
    
    @IBOutlet weak var beview: BEView!

    override func viewDidLoad() {
        super.viewDidLoad()
        beview.loadSkysphNamed("panorama360.jpg")
    }
    
    override func viewDidAppear(_ animated: Bool) {
        super.viewDidAppear(animated)
        beview.startDraw3DContent()
    }
    
    override func viewWillDisappear(_ animated: Bool) {
        super.viewWillDisappear(animated)
        beview.stopDraw3DContent()
    }

}
