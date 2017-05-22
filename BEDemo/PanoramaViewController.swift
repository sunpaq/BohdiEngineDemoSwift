//
//  PanoramaViewController.swift
//  BEDemo
//
//  Created by Sun YuLi on 2017/5/22.
//  Copyright © 2017年 SODEC. All rights reserved.
//

import UIKit

class PanoramaViewController: UIViewController {
    
    @IBOutlet weak var panoview: BEPanoramaView!
    
    override func viewDidLoad() {
        super.viewDidLoad()
        panoview.loadPanoramaTexture("panorama360.jpg")
    }
    
    override func viewDidAppear(_ animated: Bool) {
        super.viewDidAppear(animated)
        panoview.startDraw()
    }
    
    override func viewWillDisappear(_ animated: Bool) {
        super.viewWillDisappear(animated)
        panoview.stopDraw()
    }

}
