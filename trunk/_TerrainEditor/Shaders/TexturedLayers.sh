//*****************************************************************************
//**
//**   File:               TexturedLayers.sh
//**   Author:             $Author: Joep.Moritz $
//**   Last Change:        $Date: 2008-03-09 14:34:51 +0000 (Sun, 09 Mar 2008) $
//**   Last Revision:      $Revision: 126 $
//**
//*****************************************************************************

[Shader]
Name = TexturedLayers
Vertex = basicvs.vert
Fragment = TexturedLayers.frag

#*** Fragment shader uniforms

[Uniform : texrepeats]
Components = 4
Datatype = float

[Uniform : heightmap]
Components = 1
Datatype = int

[Uniform : layer1map]
Components = 1
Datatype = int

[Uniform : layer2map]
Components = 1
Datatype = int

[Uniform : layer3map]
Components = 1
Datatype = int

[Uniform : layer4map]
Components = 1
Datatype = int

[Uniform : layerBounds]
Components = 3
Datatype = float

[Uniform : bleedBounds]
Components = 3
Datatype = float

[Uniform : seaLevel]
Components = 1
Datatype = float