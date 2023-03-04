#include<GL/glew.h>
#include<GLFW/glfw3.h>
#include<Tool.h>
#include"Triangle.h"
#include"Sphere.h"
#include<random>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include<stb_image_write.h>

#pragma comment(lib,"mylib.lib")

//float CameraControllor::dt = 0;


void move_triangles(char*& p, Triangle* triangles, int count) {
	for (int i = 0; i < count; i++) {
		char* start = p;
		memcpy(p, glm::value_ptr(triangles[i].v1), 16); p += 16;
		memcpy(p, glm::value_ptr(triangles[i].v2), 16); p += 16;
		memcpy(p, glm::value_ptr(triangles[i].v3), 16); p += 16;
		memcpy(p, glm::value_ptr(triangles[i].color), 16); p += 16;
		memcpy(p, glm::value_ptr(triangles[i].norm), 16); p += 16;
		memcpy(p, &triangles[i].emissive,4);p += 4;
		memcpy(p, &triangles[i].reflect_rate,4); p += 4;
		p = start + Triangle::StructSize;
	}
}
void move_spheres(char*& p, Sphere* spheres, int count) {
	for (int i = 0; i < count; i++) {
		char* start = p;
		memcpy(p, glm::value_ptr(spheres[i].centre), 16); p += 16;
		memcpy(p, glm::value_ptr(spheres[i].color), 16); p += 16;
		memcpy(p, &spheres[i].radius, 4); p += 4;
		memcpy(p, &spheres[i].emissive, 4); p += 4;
		memcpy(p, &spheres[i].reflect_rate, 4); p += 4;
		p = start + Sphere::StructSize;
	}
}

enum { WINDOW_HEIGHT = 450, WINDOW_WIDTH = 500 };

enum { REFLECT_NUM = 5 };

enum{SAMPLE_COUNT = 20};



void write_image_png(const char* name, int wid, int hei, const unsigned char* data) {
	stbi_flip_vertically_on_write(1);
	stbi_write_png(name, wid, hei, 4, data, 0);
}




int main() {
	//init 
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "opengl window", NULL, NULL);
	glfwMakeContextCurrent(window);
	if (glewInit() != GLEW_OK) {
		return -1;
	}
	glClearColor(0, 0, 0, 0);
	GLuint vao;
	glCreateVertexArrays(1, &vao);
	glBindVertexArray(vao);

	//cout << "sample 数量："; cin >> SAMPLE_COUNT;
	float half_size = 3.f;
	Triangle triangles[14] = {
		{{-half_size,-half_size,half_size,1} ,{-half_size,half_size,half_size,1},{half_size,half_size,half_size,1},{0.125,0.365,0.154,1},{0,0,-1,0},0,0.56 },
		{{-half_size,-half_size,half_size,1} ,{half_size,-half_size,half_size,1},{half_size,half_size,half_size,1},{0.125,0.365,0.154,1},{0,0,-1,0},0,0.56},

		{{half_size,-half_size,half_size,1} ,{half_size,half_size,half_size,1},{half_size,half_size,-half_size,1},{0.563,0.365,0.204,1},{-1,0,0,0},0,0.73},
		{{half_size,-half_size,half_size,1} ,{half_size,-half_size,-half_size,1},{half_size,half_size,-half_size,1},{0.563,0.365,0.204,1},{-1,0,0,0},0,0.73},

		{{half_size,-half_size,-half_size,1} ,{half_size,half_size,-half_size,1},{-half_size,half_size,-half_size,1},{0.145,0.541,0.702,1},{0,0,1,0},0,1},
		{{half_size,-half_size,-half_size,1} ,{-half_size,-half_size,-half_size,1},{-half_size,half_size,-half_size,1},{0.145,0.541,0.702,1},{0,0,1,0},0,1},

		{{-half_size,-half_size,-half_size,1} ,{-half_size,half_size,-half_size,1},{-half_size,half_size,half_size,1},{0.456,0.102,0.450,1},{1,0,0,0},0,1},
		{{-half_size,-half_size,-half_size,1} ,{-half_size,-half_size,half_size,1},{-half_size,half_size,half_size,1},{0.456,0.102,0.450,1},{1,0,0,0},0,1},

		{{-half_size,half_size,half_size,1} ,{-half_size,half_size,-half_size,1},{half_size,half_size,-half_size,1},{1,1,1,1},{0,-1,0,0},0,1},
		{{-half_size,half_size,half_size,1} ,{half_size,half_size,half_size,1},{half_size,half_size,-half_size,1},{1,1,1,1},{0,-1,0,0},0,1},

		{{-half_size,-half_size,half_size,1} ,{-half_size,-half_size,-half_size,1},{half_size,-half_size,-half_size,1},{0.7,0.7,0.7,1},{0,1,0,0},0,1},
		{{-half_size,-half_size,half_size,1} ,{half_size,-half_size,half_size,1},{half_size,-half_size,-half_size,1},{0.7,0.7,0.7,1},{0,1,0,0},0,1},

		{{-half_size*0.5,2,half_size * 0.5,1} ,{-half_size * 0.5,2,-half_size * 0.5,1},{half_size * 0.5,2,-half_size * 0.5,1},{0.9,0.9,0.9,1},{0,-1,0,0},1,1},
		{{-half_size * 0.5,2,half_size * 0.5,1} ,{half_size * 0.5,2,half_size * 0.5,1},{half_size * 0.5,2,-half_size * 0.5,1},{0.9,0.9,0.9,1},{0,-1,0,0},1,1},
	};

	Sphere spheres[5] = {
		//centre color radius emissive
		{{0,0,0,1},{0.265,0.123,0.145,1},0.1f,0,0.96},
		{{-1.5,-1.5,-1.5,1},{0.781,0.715,0.64,1},1,0,0.78},
		{{1.5,1.5,1.5,1},{0.125,0.135,0.147,1},0.7,0,0.78},
		{{1.7,1.5,-1.6,1},{0.362,0.314,0.145,1},0.5,0,0.90},
		{{-1.7,-1.5,1.6,1},{0.145,0.361,0.245,1},0.6,0,0.67},
	};


	Camera camera(2.5, 2.5, WINDOW_WIDTH, WINDOW_HEIGHT, -15, -180, {2.9,0,0});
	CameraControllor cc;
	cc.SetUpCamera(&camera);
	cc.SetUpCentre(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
	glfwSetKeyCallback(window,CameraControllor::KeyInputCallback);
	glfwSetCursorPosCallback(window, CameraControllor::MouseMoveCallback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	//配置camera  ing...
	//测试---
	//GLuint test = create_program("test_vs.glsl", "test_fs.glsl", 0, 0, "quad_gs.glsl");
	//Triangle testTriangle;
	//testTriangle.v1 = glm::vec4(0, 0.5, -1, 1);
	//testTriangle.v2 = glm::vec4(-0.5, -0.5, -1, 1);
	//testTriangle.v3 = glm::vec4(0.5, -0.5, -1, 1);
	//testTriangle.color = glm::vec4(0.5, 0.1, 0.6, 1);
	//Sphere testsphere;
	//testsphere.centre = glm::vec4(-0.5, 0, -2, 1);
	//testsphere.radius = 1;
	//testsphere.color = glm::vec4(0.125, 0.321, 0.451, 1);
	//GLuint uniform_buffer;
	//glCreateBuffers(1, &uniform_buffer);
	//glNamedBufferStorage(uniform_buffer, Triangle::StructSize + Sphere::StructSize, 0, GL_MAP_WRITE_BIT);
	//char* p = static_cast<char*>(glMapNamedBuffer(uniform_buffer,GL_WRITE_ONLY));
	//move_triangles(p, &testTriangle, 1);
	//move_spheres(p, &testsphere, 1);
	//glUnmapNamedBuffer(uniform_buffer);
	//glBindBufferBase(GL_UNIFORM_BUFFER, 0, uniform_buffer);

	//------

	//texture 定义声明
	GLuint dir_tex,origin_tex;
	glCreateTextures(GL_TEXTURE_2D, 1, &dir_tex);
	glTextureStorage2D(dir_tex, 1, GL_RGBA32F, WINDOW_WIDTH, WINDOW_HEIGHT);
	glCreateTextures(GL_TEXTURE_2D, 1, &origin_tex);
	glTextureStorage2D(origin_tex, 1, GL_RGBA32F, WINDOW_WIDTH, WINDOW_HEIGHT);
	//中间存储颜色的texture
	GLuint intermidiate_texs;
	glCreateTextures(GL_TEXTURE_2D_ARRAY, 1, &intermidiate_texs);
	glTextureStorage3D(intermidiate_texs, 1, GL_RGBA32F, WINDOW_WIDTH, WINDOW_HEIGHT, REFLECT_NUM);
	//记录停止的texture
	GLuint stop;
	glCreateTextures(GL_TEXTURE_2D,1, &stop);
	glTextureStorage2D(stop, 1, GL_R8UI, WINDOW_WIDTH, WINDOW_HEIGHT);
	//存储累计颜色的texture
	GLuint accum_tex;
	glCreateTextures(GL_TEXTURE_2D, 1, &accum_tex);
	glTextureStorage2D(accum_tex, 1, GL_RGBA32F, WINDOW_WIDTH, WINDOW_HEIGHT);
	//存储最终输出颜色的texture
	GLuint output_tex;
	glCreateTextures(GL_TEXTURE_2D, 1,& output_tex);
	glTextureStorage2D(output_tex, 1, GL_RGBA32F, WINDOW_WIDTH, WINDOW_HEIGHT);

	//framebuffers
	//初始化dir和origin的framebuffer
	GLuint init_framebuffer;
	glCreateFramebuffers(1, &init_framebuffer);
	glNamedFramebufferTexture(init_framebuffer, GL_COLOR_ATTACHMENT0, dir_tex, 0);
	glNamedFramebufferTexture(init_framebuffer, GL_COLOR_ATTACHMENT1, origin_tex, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, init_framebuffer);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "init framebuffer is incomplete" << std::endl;
		exit(-1);
	}
	GLenum drawbuffers[] = {GL_COLOR_ATTACHMENT0,GL_COLOR_ATTACHMENT1};
	glDrawBuffers(2, drawbuffers);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//进行追踪的 framebuffer
	GLuint trase_framebuffer;
	glCreateFramebuffers(1, &trase_framebuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, trase_framebuffer);
	glDrawBuffers(1, drawbuffers);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	//累计颜色的framebuffer
	GLuint accum_framebuffer;
	glCreateFramebuffers(1, &accum_framebuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, accum_framebuffer);
	glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, accum_tex, 0);
	glDrawBuffers(1, drawbuffers);
	if (glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "init framebuffer is incomplete" << std::endl;
		exit(-1);
	}
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);


	//programs 
	//1.初始化程序
	GLuint init_pass = create_program("init_vs.glsl", "init_fs.glsl", 0, 0, "quad_gs.glsl");
	glUseProgram(init_pass);
	glUniform1i(glGetUniformLocation(init_pass, "wind_hei"), WINDOW_HEIGHT);
	glUniform1i(glGetUniformLocation(init_pass, "wind_wid"), WINDOW_WIDTH);
	glUniform1f(glGetUniformLocation(init_pass, "near_dis"), 0.1);
	glm::mat4 proj_inv = glm::inverse(camera.GetProjectMat());
	glUniformMatrix4fv(glGetUniformLocation(init_pass, "proj_inv"), 1, GL_FALSE, glm::value_ptr(proj_inv));
	//追踪程序
	GLuint trase_pass = create_program("trase_vs.glsl", "trase_fs.glsl", 0, 0, "quad_gs.glsl");
	glUseProgram(trase_pass);
	glBindImageTexture(0, dir_tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
	glBindImageTexture(1, origin_tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
	glBindImageTexture(2, stop, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R8UI);
	glUniformMatrix4fv(glGetUniformLocation(trase_pass,"view"),1,GL_FALSE,glm::value_ptr(camera.GetViewMat()));
	glUniform1ui(glGetUniformLocation(trase_pass, "triangle_count"),14);
	glUniform1ui(glGetUniformLocation(trase_pass, "sphere_count"), 5);
	glUniform1ui(glGetUniformLocation(trase_pass, "reflect_num_bound"), REFLECT_NUM);
	GLuint tri_buffer,sphere_buffer;
	glCreateBuffers(1, &tri_buffer);
	glNamedBufferStorage(tri_buffer, 14 * Triangle::StructSize, nullptr, GL_MAP_WRITE_BIT);
	char* ptr = static_cast<char*>(glMapNamedBuffer(tri_buffer,GL_WRITE_ONLY));
	move_triangles(ptr, triangles, 14);
	glUnmapNamedBuffer(tri_buffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, tri_buffer);
	glCreateBuffers(1, &sphere_buffer);
	glNamedBufferStorage(sphere_buffer, 5 * Sphere::StructSize, nullptr, GL_MAP_WRITE_BIT);
	ptr = static_cast<char*>(glMapNamedBuffer(sphere_buffer, GL_WRITE_ONLY));
	move_spheres(ptr, spheres, 5);
	glUnmapNamedBuffer(sphere_buffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, sphere_buffer);


	GLuint accum_pass = create_program("accum_vs.glsl", "accum_fs.glsl", 0, 0, "quad_gs.glsl");
	GLuint output_pass = create_program("output_vs.glsl", "output_fs.glsl", 0, 0, "quad_gs.glsl");



	auto init = [&init_framebuffer,&init_pass,accum_framebuffer]() {
		glUseProgram(init_pass);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, init_framebuffer);
		glDrawArrays(GL_POINTS, 0, 1);

	}; 
	srand(time(0));
	auto chase = [&trase_pass,&camera,&trase_framebuffer, intermidiate_texs,stop]() {
		auto random_vec3 = []()->glm::vec3 {
			glm::vec3 v;
			v.x = (rand() % 100 + 1) / 100.f * 2 - 1;
			v.y = (rand() % 100 + 1) / 100.f * 2 - 1;
			v.z = (rand() % 100 + 1) / 100.f;
			return glm::normalize(v);
		};

		glUseProgram(trase_pass);
		glUniformMatrix4fv(glGetUniformLocation(trase_pass, "view"), 1, GL_FALSE, glm::value_ptr(camera.GetViewMat()));
		
		glBindFramebuffer(GL_FRAMEBUFFER, trase_framebuffer);
		
		int zero = 0;
		glClearTexSubImage(stop, 0, 0, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 1, GL_RED_INTEGER, GL_UNSIGNED_INT, &zero);
		//glBindImageTexture(2, stop, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R8UI);
		for (int i = 0; i < REFLECT_NUM; i++) {
			glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, intermidiate_texs,0, i);
			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
				std::cout << "trase framebuffer is incomplete." << endl;
				exit(-1);
			}
			//auto rvec3 = random_vec3();
			//cout << rvec3.x << "," << rvec3.y << "," << rvec3.z << endl;
			glUniform1ui(glGetUniformLocation(trase_pass, "time"),glfwGetTime() * 1000000);
			glUniform1ui(glGetUniformLocation(trase_pass, "reflect_num"), i+1);
			glDrawArrays(GL_POINTS, 0, 1);
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	};
	auto accum = [&accum_pass, accum_framebuffer, intermidiate_texs]() {
		glUseProgram(accum_pass);
		glBindFramebuffer(GL_FRAMEBUFFER, accum_framebuffer);
		//输出到accum_tex
		glBindTextureUnit(0,intermidiate_texs);
		glUniform1ui(glGetUniformLocation(accum_pass, "color_depth"), REFLECT_NUM);
		//颜色累加
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE);
		glBlendEquation(GL_FUNC_ADD);
		glDrawArrays(GL_POINTS, 0, 1);
		glDisable(GL_BLEND);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	};
	auto output = [output_pass, accum_tex]() {
		glUseProgram(output_pass);
		glUniform1ui(glGetUniformLocation(output_pass, "sample_count"), SAMPLE_COUNT);
		glBindTextureUnit(0, accum_tex);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT);
		glDrawArrays(GL_POINTS, 0, 1);
	};


	//auto draw = [test,dir_tex,&camera,&cc](double dt) {
	//	glUseProgram(test);
	//	cc.SetUpDeltaTime(dt);
	//	glUniformMatrix4fv(glGetUniformLocation(test, "view"),1,GL_FALSE,glm::value_ptr(camera.GetViewMat()));
	//	glBindTextureUnit(0, dir_tex);
	//	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//	glDrawArrays(GL_POINTS, 0, 1);
	//};
	double lastTime = glfwGetTime();


	
	unsigned char* imgData = new unsigned char[WINDOW_HEIGHT * WINDOW_WIDTH*4];
	while (!glfwWindowShouldClose(window)) {
		double time = glfwGetTime();
		//draw(time - lastTime);
		cc.SetUpDeltaTime(time - lastTime);
		lastTime = time;
		//清理accum_tex
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, accum_framebuffer);
		glClearColor(0, 0, 0, 0);
		glClear(GL_COLOR_BUFFER_BIT);
		for (int i = 0; i < SAMPLE_COUNT; i++) {
			init();
			chase();
			accum();
		}
		output();
		//glReadPixels(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, imgData);
		//write_image_png("output.png", WINDOW_WIDTH, WINDOW_HEIGHT, imgData);
		glfwSwapBuffers(window);
		glfwPollEvents();
		//system("output.png");
		//break;
	}
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
	delete []imgData;
	
	glfwTerminate();
	return 0;
}