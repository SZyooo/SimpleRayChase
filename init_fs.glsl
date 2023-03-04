#version 450 core

layout(location = 0)out vec3 dir;
layout(location = 1)out vec3 origin;
uniform int wind_hei,wind_wid;
uniform float near_dis;
uniform mat4 proj_inv;
void main(){
	vec4 NDC;
	NDC.x = 2.f * float(gl_FragCoord.x) / wind_wid - 1;
	NDC.y = 2.f * float(gl_FragCoord.y) / wind_hei - 1;
	NDC.z = -1;
	NDC.w = 1;
	vec4 clip = NDC * near_dis;
	dir = normalize((proj_inv * clip).xyz);
	origin = vec3(0);
}