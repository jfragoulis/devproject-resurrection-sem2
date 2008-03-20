//*****************************************************************************
//**
//**   File:               perpixelnotex.frag
//**   Author:             $Author: Joep.Moritz $
//**   Last Change:        $Date: 2008-03-09 14:34:51 +0000 (Sun, 09 Mar 2008) $
//**   Last Revision:      $Revision: 126 $
//**
//*****************************************************************************

varying vec4 diffuse,ambient;
varying vec3 normal,lightDir,viewVec;


void main()
{
	vec3 n,halfV,viewV,ldir;
	float NdotL,NdotHV;
	vec4 color = ambient;
	
	n = normalize(normal);
	
	NdotL = max(dot(n,lightDir),0.0);

	if (NdotL > 0.0) {
		vec3 V = normalize(viewVec);
		halfV = reflect(lightDir,n);
		NdotHV = max(dot(halfV,V),0.0);
		color += gl_FrontMaterial.specular * pow(NdotHV,gl_FrontMaterial.shininess);
		color += diffuse * NdotL;
	}

	gl_FragColor = color;
}