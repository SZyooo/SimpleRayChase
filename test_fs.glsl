#version 450 core


uniform mat4 view;
layout(binding = 0)uniform sampler2D dir;

out vec4 color;


struct Triangle{
	vec4 p1,p2,p3;
	vec4 color;
};

struct Sphere{
	vec4 centre;
	vec4 color;
	float radius;
};


layout(std140,binding = 0)uniform Shapes{
	Triangle triangle;
	Sphere sphere;
};

struct Ray{
	vec3 origin;
	vec3 dir;
};

struct Hit{
	vec4 color;
	float t;
};

float HitTriangle(Ray ray,Triangle triangle){
	vec3 p1 = vec3(view * triangle.p1);
	vec3 p2 = vec3(view * triangle.p2);
	vec3 p3 = vec3(view * triangle.p3);
	vec3 p1p2 = p2 - p1;
	vec3 p2p3 = p3 - p2;
	vec3 p3p1 = p1 - p3;
	vec3 norm = normalize(cross(p1p2,p2p3));
	if(dot(ray.dir,norm) == 0){
		return -1;
	}
	float t = (dot(norm,p1.xyz) - dot(norm,ray.origin))/dot(norm,ray.dir);
	vec3 hiton = ray.dir*t + ray.origin;
	vec3 p1h = hiton - p1;
	vec3 p2h = hiton - p2;
	vec3 p3h = hiton - p3;

	vec3 c1 = cross(p1p2,p1h);
	vec3 c2 = cross(p2p3,p2h);
	vec3 c3 = cross(p3p1,p3h);

	if(dot(c1,norm) >= 0 && dot(c2,norm) >=0 && dot(c3,norm) >= 0){
		return t;
	}
	else if(dot(c1,norm) <= 0 && dot(c2,norm) <= 0 && dot(c3,norm) <= 0){
		return t;
	}
	return -1;
}
float HitSphere(Ray ray,Sphere sphere){
	vec4 centre = view * sphere.centre;
	vec3 v = ray.origin - centre.xyz;
	float B = 2 * dot(ray.dir,v);
	float C = dot(v,v) - sphere.radius*sphere.radius;
	float Delta = B*B - 4*C;
	if(Delta <0){
		return -1;
	}
	float sqrtDelta = sqrt(Delta);
	float t1 = -B + sqrtDelta;
	float t2 = -B - sqrtDelta;
	float t = min(max(t1,0),max(t2,0));
	if(t == 0)return -1;
	return t * 0.5;
}



Hit HitShapes(Ray ray){
	Hit hit;
	float hit_sphere = HitSphere(ray,sphere);
	float hit_triangle = HitTriangle(ray,triangle);
	if(hit_sphere < 0){
		hit.t = hit_triangle;
		hit.color = triangle.color;
	}
	else if(hit_triangle < 0){
		hit.t = hit_sphere;
		hit.color = sphere.color;
	}
	else{
		if(hit_sphere < hit_triangle){
			hit.t = hit_sphere;
			hit.color = sphere.color;
		}
		else{
			hit.t = hit_triangle;
			hit.color = triangle.color;
		}
	}
	return hit;
}



void main(){
	Ray ray;
	ray.origin = vec3(0);
	ray.dir = texelFetch(dir,ivec2(gl_FragCoord),0).xyz;
	Hit hit = HitShapes(ray);
	if(hit.t > 0){
		color = hit.color;
	}
	else{
		color = vec4(0);
	}
}