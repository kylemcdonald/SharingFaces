#version 120

uniform sampler2DRect tex;
uniform vec3 whitePoint;

void main() {
	gl_FragColor = vec4(texture2DRect(tex, gl_TexCoord[0].st).rgb * whitePoint, 1.);
}