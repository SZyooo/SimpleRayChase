#version 450 core

out vec4 color;

layout(binding = 0,rgba32f) uniform image2D dir;
layout(binding = 1,rgba32f) uniform image2D origin;
layout(binding = 2,r8ui) uniform uimage2D stop;

uniform mat4 view;
uniform uint triangle_count,sphere_count;
uniform uint time;
uniform uint reflect_num;
uniform uint reflect_num_bound;

struct Triangle{
	//0 16 p1
	//16 32 p2
	//32 48 p3
	//48 64 color
	//64 68 emissive
	vec4 p1,p2,p3;
	vec4 color;
	vec4 norm;
	int emissive;
	float reflect_rate;
};

struct Sphere{
	vec4 centre;
	vec4 color;
	float radius;
	int emissive;
	float reflect_rate;
};

struct Ray{
	vec3 origin;
	vec3 dir;
};

struct Hit{
	float dis;
	vec4 color;
	int emissive;
	float consine;
	vec3 norm;
	float ref_rate;
};

layout(std140,binding = 0)buffer Triangles{
	Triangle triangles[14];
};
layout(std140,binding = 1)buffer Spheres{
	Sphere spheres[5];
};

float bias = 0.005;
Hit HitTriangle(Ray ray,Triangle triangle){
	Hit hit;
	hit.dis = -1;
	vec3 p1 = vec3(view * triangle.p1);
	vec3 p2 = vec3(view * triangle.p2);
	vec3 p3 = vec3(view * triangle.p3);
	vec3 p1p2 = p2 - p1;
	vec3 p2p3 = p3 - p2;
	vec3 p3p1 = p1 - p3;
	vec3 norm = normalize((transpose(inverse(view))*triangle.norm).xyz);
	if(dot(ray.dir,norm) == 0){
		return hit;
	}
	float t = (dot(norm,p1.xyz) - dot(norm,ray.origin))/dot(norm,ray.dir);
	vec3 hiton = ray.dir*t + ray.origin;
	vec3 p1h = hiton - p1;
	vec3 p2h = hiton - p2;
	vec3 p3h = hiton - p3;

	vec3 c1 = cross(p1p2,p1h);
	vec3 c2 = cross(p2p3,p2h);
	vec3 c3 = cross(p3p1,p3h);

	if((dot(c1,norm) >= 0 && dot(c2,norm) >=0 && dot(c3,norm) >= 0) || (dot(c1,norm) <= 0 && dot(c2,norm) <= 0 && dot(c3,norm) <= 0)){
		hit.dis = t - bias;
		hit.color = triangle.color;
		hit.emissive = triangle.emissive;
		hit.consine = abs(dot(norm,ray.dir));
		//if(dot(norm,ray.dir) < 0){norm = -norm;}
		hit.norm = norm;
		hit.ref_rate = triangle.reflect_rate;
	}
	return hit;
}
Hit HitSphere(Ray ray,Sphere sphere){
	Hit hit;
	hit.dis = -1;
	vec4 centre = view * sphere.centre;
	vec3 v = ray.origin - centre.xyz;
	float B = 2 * dot(ray.dir,v);
	float C = dot(v,v) - sphere.radius*sphere.radius;
	float Delta = B*B - 4*C;
	if(Delta <0){
		return hit;
	}
	float sqrtDelta = sqrt(Delta);
	float t1 = -B + sqrtDelta;
	float t2 = -B - sqrtDelta;
	float t = min(max(t1,0),max(t2,0));
	if(t == 0)return hit;
	hit.dis = t * 0.5 - bias;
	hit.color = sphere.color;
	hit.emissive = sphere.emissive;
	vec3 norm = normalize(ray.origin + t*ray.dir - centre.xyz);
	hit.consine = abs(dot(norm,-ray.dir));
	//if(dot(ray.dir,norm) < 0)norm = -norm;
	hit.norm = norm;
	hit.ref_rate = sphere.reflect_rate;
	return hit;
}

Hit traversePrimitives(Ray ray){
	Hit trySpheres;
	trySpheres.dis = -1;
	for(int i =0;i<sphere_count;i++){
		Hit try = HitSphere(ray,spheres[i]);
		if(trySpheres.dis < 0 || (trySpheres.dis > try.dis && try.dis > 0)){
			trySpheres = try;
		}
	}
	Hit tryTriangles;
	tryTriangles.dis = -1;
	for(int i=0;i<triangle_count;i++){
		Hit try = HitTriangle(ray,triangles[i]);
		if(tryTriangles.dis < 0 || (tryTriangles.dis > try.dis && try.dis > 0)){
			tryTriangles = try;
		}
	}
	if(trySpheres.dis < 0) return tryTriangles;
	else if(tryTriangles.dis < 0) return trySpheres;
	else{
		if(trySpheres.dis < tryTriangles.dis)return trySpheres;
		else return tryTriangles;
	}
}

float PHI = 1.61803398874989484820459;  // Φ = Golden Ratio   

float gold_noise(in vec2 xy, in float seed){
       return fract(tan(distance(xy*PHI, xy)*seed)*xy.x);
}

vec3 randVec3(vec2 pos){
	int pi[] = { 3,1,4,1,5,9,2,6,5,3,5,8,9,7,9,3,2,3,8,4,6,2,6,4,3,3,8,3,2,7,9,5,0,2,
	8,8,4,1,9,7,1,6,9,3,9,9,3,7,5,1,0,6,9,3,9,9,3,7,5,1,0,4,5,9,2,3,0,7,8,1,6,4,0,
	6,2,8,6,2,0,8,9,9,8,6,2,8,0,3,4,8,2,5,3,4,2,1,1,7,0,6,7,9,8,8,2,0,4,6,6,5,2,1,
	3,8,4,1,4,6,9,5,1,9,4,1,5,1,1,6,0,9,4,3,3,0,5,7,2,7,0,3,6,5,7,5,9,5,9,1,9,5,3,
	0,9,2,1,8,6,1,1,7,3,8,1,9,3,2,6,1,1,7,9,3,1,0,5,1,1,8,5,4,8,0,7,4,4,6,2,3,7,9,
	9,6,2,7,4,9,5,6,7,3,5,1,8,8,5,7,5,2,7,2,4,8,9,1,2,2,7,9,3,8,1,8,3,0,1,1,9,4,9,
	1,2,9,8,3,3,6,7,3,3,6,2,4,4,0,6,5,6,6,4,3,0,8,6,0,2,1,3,9,4,9,4,6,3,9,5,2,2,4,
	7,3,7,1,9,0,7,0,2,1,7,9,8,6,0,9,4,3,7,0,2,7,7,0,5,3,9,2,1,7,1,7,6,2,9,3,1,7,6,
	7,5,2,3,8,4,6,7,4,8,1,8,4,6,7,6,6,9,4,0,3,0,0,1,9,2,7,8,7,6,6,1,1,1,9,5,9,0,9,
		2,1,6,4,2,0,1,9,8,9,3,8,0,9,5,2,5,7,2,0,1,0,6,5,4,8,5,8,6,3,2,7,8,8,6,5,9,
		3,6,1,5,3,3,8,1,8,2,7,9,6,8,2,3,0,3,0};
	vec3 v;
	v.x = fract(tan(distance(pos*PHI, pos)*(time%101))*pos.x);
	v.y = fract(cos(distance(pos*PHI, pos)*(time%101))*pos.y);
	v.z = fract(sin(distance(pos*PHI, pos)*(time%101))*(pos.y * pos.x));
	return normalize(v);
}


void main(){
	uint end = imageLoad(stop,ivec2(gl_FragCoord)).x;
	//如果已经结束了，则直接写入（1，1，1，1）
	if(end == 1){
		color = vec4(1);
		return ;
	}
	else{
		Ray ray;
		ray.origin = imageLoad(origin,ivec2(gl_FragCoord)).xyz;
		ray.dir = imageLoad(dir,ivec2(gl_FragCoord)).xyz;
		Hit hit = traversePrimitives(ray);
		//没有碰撞
		if(hit.dis < 0) {
			color = vec4(0);
			imageStore(stop,ivec2(gl_FragCoord),uvec4(1));
		}
		else{
			//反射次数已经到头了
			if(reflect_num == reflect_num_bound){
				//最后击中的仍然不是发光物
				if(hit.emissive == 0){
					color = vec4(0);
				}
				//击中了发光物
				else{
					color = hit.color;
					color.a = 1;
				}
			}
			//反射次数还没有到头
			else{
				//击中发光物标记结束
				if(hit.emissive == 1){
					imageStore(stop,ivec2(gl_FragCoord),uvec4(1));
				}
				else{
					//反射
					float ref = gold_noise(gl_FragCoord.xy,time%997) * 0.5 + 0.5;
					vec3 v;
					if(ref < hit.ref_rate){
						v= normalize(randVec3(vec2(gl_FragCoord)) + hit.norm).xyz;//随机反射
					}
					else{
						v = normalize(reflect(ray.dir,hit.norm));//全反射
					}
					color.a = hit.consine;
					imageStore(dir,ivec2(gl_FragCoord),vec4(v,0));
					//imageStore(dir,ivec2(gl_FragCoord),vec4(normalize(reflect(ray.dir,hit.norm)),0));//正常反射
					//输出randVec3
					//imageStore(dir,ivec2(gl_FragCoord),vec4(normalize(randVec3(vec2(gl_FragCoord))),0));
					imageStore(origin,ivec2(gl_FragCoord),vec4(ray.origin + ray.dir * hit.dis,1));
				}
				color = hit.color;
			}
		}
	}
}

