#version 450 core


out vec4 color;

layout(binding = 0)uniform sampler2D accumulated_color;
uniform uint sample_count;

void main(){
	color = vec4(1);
	color.rgb = texelFetch(accumulated_color,ivec2(gl_FragCoord),0).rgb / sample_count;
}