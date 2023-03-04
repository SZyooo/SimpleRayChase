#pragma once
#include<glm/common.hpp>
struct Sphere
{
	//48
	glm::vec4 centre;//0 16
	glm::vec4 color;//16 32
	float radius;//32 36
	int emissive;//36 40
	float reflect_rate;//40 44
	static unsigned int StructSize;
};

