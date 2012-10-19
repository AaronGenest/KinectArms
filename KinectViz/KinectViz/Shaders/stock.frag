uniform sampler2D Tex;
uniform float fadeFactor;
uniform vec3 tint;

void main(void)
{
	vec4 texel0 = texture2D(Tex, gl_TexCoord[0].st);
	//if (texel0.r == 0.0 && texel0.g == 0.0 && texel0.b == 0.0)
	//	texel0.a = 0.0;
	//else
	
	texel0.a = texel0.a * fadeFactor;
	
	texel0.rgb *= tint;  // multiply pixel color by tint

	gl_FragColor = texel0;
}