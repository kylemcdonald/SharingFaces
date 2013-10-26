#version 120

uniform sampler2DRect tex;

const vec3 whitePoint = vec3(1.0842105, 1., 1.045685);

void main() {
	gl_FragColor = vec4(texture2DRect(tex, gl_TexCoord[0].st).rgb * whitePoint, 1.);
}