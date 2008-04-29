uniform sampler2D particleTex;
varying vec4 color;
varying vec2 vTexCoord;


void main(void)
{
	float lum = texture2D(particleTex,gl_TexCoord[0].st).r;
	gl_FragColor = lum*color*vec4(vec3(1.0),lum);
}