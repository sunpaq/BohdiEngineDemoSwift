# BohdiEngineDemoSwift
a BohdiEngine demo using swift

### please install latest version of dependecies first

	pod update
	pod install

### in VR mode you can use iOS MFi game controllers (with trigger buttons)

	left-trigger  -> wire mode
	right-trigger -> zoom in

	right-trigger-on + left-shoudler  -> zoom lock
	right-trigger-off + left-shoudler -> zoom unlock

	left-stick    -> rotate horizontally
	right-stick   -> rotate vertically

	A-button      -> next model
	B-button      -> prev model
	X-button      -> clear screen
	Y-button      -> add a background

### requirement for 3D models

    .obj format with .mtl file
    should have normal data

### requirement for textures

    texures can not contain alpha channel
    BohdiEngine use SOIL library read texture, so it can support all the textures SOIL supported
