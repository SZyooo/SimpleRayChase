#pragma once
#include<glm/common.hpp>
struct Triangle
{
	glm::vec4 v1;//0 16
	glm::vec4 v2;//16 32
	glm::vec4 v3;//32 48
	glm::vec4 color;//48 64
	glm::vec4 norm;//64 80
	int emissive;//80 84
	float reflect_rate;//84 88
	static unsigned int StructSize;
};

