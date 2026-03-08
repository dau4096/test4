/* worldspace.vert */
#version 460 core

layout(location=0) in vec3 aPos;
layout(location=1) in vec2 aUV;

out vec2 fragUV;
uniform mat4 pvmMatrix;

void main() {
	gl_Position = (pvmMatrix * vec4(aPos, 1.0f));
	fragUV = aUV;
}
