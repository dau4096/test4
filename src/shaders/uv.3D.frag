/* uv.3D.frag */
#version 460 core

in vec2 fragUV;
out vec4 fragColour;


layout(binding=0) uniform sampler2D tex;


void main() {
	//Simply shows the UV per pixel on this triangle.
	fragColour = vec4(fract(fragUV.xy * 16.0f), 0.0f, 1.0f);

	//Shows the texture on the surface.
	fragColour = texture(tex, fragUV.xy);
	if (fragColour.a < 0.5f) {discard;}
}
