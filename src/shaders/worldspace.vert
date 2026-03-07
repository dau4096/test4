/* worldspace.vert */
#version 460 core

layout(location=0) in vec3 aPos;
layout(location=1) in vec2 aUV;
//layout(location=2) vec3 aNormal; //Optional.

out vec2 fragUV;
//out vec3 fragNormal; //Normals optional.

uniform mat4 pvmMatrix;

void main() {
	gl_Position = (pvmMatrix * vec4(aPos, 1.0f));
	fragUV = aUV;
	//fragNormal = aNormal; //Optional.
}
