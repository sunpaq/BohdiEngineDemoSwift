# BohdiEngine

[![Carthage compatible](https://img.shields.io/badge/Carthage-compatible-4BC51D.svg?style=flat)](https://github.com/Carthage/Carthage)
[![Cocoapods compatible](https://cocoapod-badges.herokuapp.com/v/BohdiEngine/badge.png)](https://cocoapods.org)

## Example

To run the example project, clone the repo, and run `pod install` from the Example directory first.

## Requirements

iOS devices support OpenGLES 3.0 with arm64 CPU (>=iPhone5s)

## Installation

By Carthage, add the following line to your Cartfile:

	github "sunpaq/BohdiEngine-pod"

BohdiEngine is available through [CocoaPods](http://cocoapods.org). To install
it, simply add the following line to your Podfile:

	pod "BohdiEngine"

If you need to use the developing version:

	target 'App' do
    	pod 'BohdiEngine', :git => 'https://github.com/sunpaq/BohdiEngine-pod.git', :branch => 'develop'
	end

## Build your App use BohdiEngine

	1. add a UIView to any of your interface in storyboard
	2. set the class of the view -> BEView
	3. set a outlet of the view to your controller
	4. call the beview.loadModelNamed("monkey2.obj") method
	5. call the beview.startDraw3DContent(BECameraRotateAroundModelManual) method

	for swift you also need add a bridge header and import:
	#import <monkc/monkc-umbrella.h>

for OpenGL setup and Engine usage, please check the Example of this Pod (Objective-C)
there also have a [demo written use swift](https://github.com/sunpaq/BohdiEngineDemoSwift)

## Author

Sun YuLi, sunpaq@gmail.com

## License

BohdiEngine is available under the BSD license. See the LICENSE file for more info.
