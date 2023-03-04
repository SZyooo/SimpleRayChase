#version 450 core

layout(location = 0)out vec4 color;
//中间记录下来的颜色
layout(binding = 0) uniform sampler2DArray colors;
uniform uint color_depth;


void main(){
	color = vec4(1);
	for(int i=0;i<color_depth;i++){
		vec4 recorded_color = texelFetch(colors,ivec3(gl_FragCoord.xy,i),0);
		color.rgb *= recorded_color.rgb*recorded_color.a;
	}
	color.rgb *= 3.141592654 * 2;
	color.a = 1;
}