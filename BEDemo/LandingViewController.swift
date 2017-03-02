//
//  LandingViewController.swift
//  BEDemo
//
//  Created by YuliSun on 16/01/2017.
//  Copyright © 2017 SODEC. All rights reserved.
//

import Foundation
import UIKit

class LandingViewController: UIViewController {
    
    static var instance: LandingViewController!
    
    @IBOutlet weak var rotateCameraSwitch: UISwitch!
    
    @IBOutlet weak var wireFrameSwitch: UISwitch!
    
    override func viewDidLoad() {
        super.viewDidLoad()
        LandingViewController.instance = self
    }
}
