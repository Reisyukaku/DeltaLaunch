#version 330 compatibility
varying vec4 v_color;
varying vec2 v_texCoord;
uniform sampler2D tex0;

void main()
{
	vec4 col = texture2D(tex0, v_texCoord) * v_color;
	gl_FragColor = col;
}
