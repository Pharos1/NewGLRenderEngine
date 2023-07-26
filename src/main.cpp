#include "pch.h"

#include "Logger.hpp"
#include "Vertex.hpp"
#include "Shader.hpp"
#include "Mesh.hpp"
#include "Camera.hpp"
#include "DeltaTime.hpp"
#include "Light.hpp"
#include "Texture.hpp"

#define glCheckError() glCheckError_(__FILE__, __LINE__)

//On Application Start
void initDependencies();

//Error checking from learnopengl.com
GLenum glCheckError_(const char* file, int line);
void APIENTRY glDebugOutput(GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length, const char* message, const void* userParam);

//On state change(buttons, resize, keys, etc.)
	//--UI

//--Window and OS
void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void mouseCallback(GLFWwindow* window, double xpos, double ypos);
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

//Every frame

//Application
uint32_t scrWidth = 1280;
uint32_t scrHeight = 920;
GLFWwindow* window;

//Shaders
Shader mainShader;
Shader lightBoxShader;
Shader gradientSkyboxShader;

//Scene
/*
std::vector<Vertex> vertices = {
	{{-1.f, 1.f, 1.f}, {0.f, 0.f, 1.f}, {0.f, 1.f}},
	{{ 1.f, 1.f, 1.f}, {0.f, 0.f, 1.f}, {1.f, 1.f}},
	{{ 1.f,-1.f, 1.f}, {0.f, 0.f, 1.f}, {1.f, 0.f}},
	{{-1.f,-1.f, 1.f}, {0.f, 0.f, 1.f}, {0.f, 0.f}},

	{{-1.f, 1.f,-1.f}, {0.f, 0.f,-1.f}, {0.f, 1.f}},
	{{ 1.f, 1.f,-1.f}, {0.f, 0.f,-1.f}, {1.f, 1.f}},
	{{ 1.f,-1.f,-1.f}, {0.f, 0.f,-1.f}, {1.f, 0.f}},
	{{-1.f,-1.f,-1.f}, {0.f, 0.f,-1.f}, {0.f, 0.f}},
};
std::vector<uint32_t> indices = {
	0, 1, 2, //Front
	0, 3, 2,
	4, 5, 6, //Back
	4, 7, 6,
	0, 3, 7, //Left
	4, 7, 3,
	1, 5, 6, //Right
	1, 2, 6,
	4, 5, 1, //Up
	4, 0, 1,
	6, 7, 3, //Down
	6, 2, 3,
};
*/
std::vector<Vertex> cubeVerts = {
	{{-1.f,-1.f, 1.f}, { 0.f, 0.f, 1.f}, {0.f, 0.f}}, //Front
	{{ 1.f,-1.f, 1.f}, { 0.f, 0.f, 1.f}, {1.f, 0.f}},
	{{ 1.f, 1.f, 1.f}, { 0.f, 0.f, 1.f}, {1.f, 1.f}},
	{{-1.f,-1.f, 1.f}, { 0.f, 0.f, 1.f}, {0.f, 0.f}},
	{{ 1.f, 1.f, 1.f}, { 0.f, 0.f, 1.f}, {1.f, 1.f}},
	{{-1.f, 1.f, 1.f}, { 0.f, 0.f, 1.f}, {0.f, 1.f}},

	{{ 1.f, 1.f,-1.f}, { 0.f, 0.f,-1.f}, {0.f, 1.f}}, //Back
	{{ 1.f,-1.f,-1.f}, { 0.f, 0.f,-1.f}, {0.f, 0.f}},
	{{-1.f,-1.f,-1.f}, { 0.f, 0.f,-1.f}, {1.f, 0.f}},
	{{-1.f, 1.f,-1.f}, { 0.f, 0.f,-1.f}, {1.f, 1.f}},
	{{ 1.f, 1.f,-1.f}, { 0.f, 0.f,-1.f}, {0.f, 1.f}},
	{{-1.f,-1.f,-1.f}, { 0.f, 0.f,-1.f}, {1.f, 0.f}},

	{{-1.f,-1.f,-1.f}, {-1.f, 0.f, 0.f}, {0.f, 0.f}}, //Left
	{{-1.f,-1.f, 1.f}, {-1.f, 0.f, 0.f}, {1.f, 0.f}},
	{{-1.f, 1.f, 1.f}, {-1.f, 0.f, 0.f}, {1.f, 1.f}},
	{{-1.f,-1.f,-1.f}, {-1.f, 0.f, 0.f}, {0.f, 0.f}},
	{{-1.f, 1.f, 1.f}, {-1.f, 0.f, 0.f}, {1.f, 1.f}},
	{{-1.f, 1.f,-1.f}, {-1.f, 0.f, 0.f}, {0.f, 1.f}},

	{{ 1.f, 1.f, 1.f}, { 1.f, 0.f, 0.f}, {1.f, 0.f}}, //Right
	{{ 1.f,-1.f, 1.f}, { 1.f, 0.f, 0.f}, {0.f, 0.f}},
	{{ 1.f,-1.f,-1.f}, { 1.f, 0.f, 0.f}, {0.f, 1.f}},
	{{ 1.f, 1.f,-1.f}, { 1.f, 0.f, 0.f}, {1.f, 1.f}},
	{{ 1.f, 1.f, 1.f}, { 1.f, 0.f, 0.f}, {1.f, 0.f}},
	{{ 1.f,-1.f,-1.f}, { 1.f, 0.f, 0.f}, {0.f, 1.f}},

	{{-1.f, 1.f, 1.f}, { 0.f, 1.f, 0.f}, {0.f, 0.f}}, //Up
	{{ 1.f, 1.f, 1.f}, { 0.f, 1.f, 0.f}, {1.f, 0.f}},
	{{ 1.f, 1.f,-1.f}, { 0.f, 1.f, 0.f}, {1.f, 1.f}},
	{{-1.f, 1.f, 1.f}, { 0.f, 1.f, 0.f}, {0.f, 0.f}},
	{{ 1.f, 1.f,-1.f}, { 0.f, 1.f, 0.f}, {1.f, 1.f}},
	{{-1.f, 1.f,-1.f}, { 0.f, 1.f, 0.f}, {0.f, 1.f}},

	{{ 1.f,-1.f,-1.f}, { 0.f,-1.f, 0.f}, {1.f, 0.f}}, //Down
	{{ 1.f,-1.f, 1.f}, { 0.f,-1.f, 0.f}, {1.f, 1.f}},
	{{-1.f,-1.f, 1.f}, { 0.f,-1.f, 0.f}, {0.f, 1.f}},
	{{-1.f,-1.f,-1.f}, { 0.f,-1.f, 0.f}, {0.f, 0.f}},
	{{ 1.f,-1.f,-1.f}, { 0.f,-1.f, 0.f}, {1.f, 0.f}},
	{{-1.f,-1.f, 1.f}, { 0.f,-1.f, 0.f}, {0.f, 1.f}},
};
std::vector<Vertex> quadVerts = {
	{{-1.f,-1.f, 0.f}, { 0.f, 0.f, 1.f}, {0.f, 0.f}},
	{{ 1.f,-1.f, 0.f}, { 0.f, 0.f, 1.f}, {1.f, 0.f}},
	{{ 1.f, 1.f, 0.f}, { 0.f, 0.f, 1.f}, {1.f, 1.f}},
	{{-1.f,-1.f, 0.f}, { 0.f, 0.f, 1.f}, {0.f, 0.f}},
	{{ 1.f, 1.f, 0.f}, { 0.f, 0.f, 1.f}, {1.f, 1.f}},
	{{-1.f, 1.f, 0.f}, { 0.f, 0.f, 1.f}, {0.f, 1.f}},
};
std::vector<Vertex> planeVerts = {
	{{ 1.f, 0.f, 1.f}, { 0.f, 1.f, 0.f}, {1.f, 0.f}},
	{{ 1.f, 0.f,-1.f}, { 0.f, 1.f, 0.f}, {1.f, 1.f}},
	{{-1.f, 0.f,-1.f}, { 0.f, 1.f, 0.f}, {0.f, 1.f}},
	{{-1.f, 0.f, 1.f}, { 0.f, 1.f, 0.f}, {0.f, 0.f}},
	{{ 1.f, 0.f, 1.f}, { 0.f, 1.f, 0.f}, {1.f, 0.f}},
	{{-1.f, 0.f,-1.f}, { 0.f, 1.f, 0.f}, {0.f, 1.f}},
};
//Mesh cube;
MaterialMesh cube;
Mesh quad;
Mesh plane;

DirLight dirLight;
PointLight pointLight;
SpotLight spotLight;

Texture texture0;

//Transformation
glm::mat4 model;
glm::mat4 view;
glm::mat4 proj;

//Camera
float fov = 45.f;
Camera cam({0.f, 1.f, 5.f});

int main() {
	//Basically initialization and setup
	initDependencies();

	mainShader.loadShader("Shaders/main.vert", "Shaders/main.frag");
	lightBoxShader.loadShader("Shaders/lightBox.vert", "Shaders/lightBox.frag");
	gradientSkyboxShader.loadShader("Shaders/basic.vert", "Shaders/gradientSkybox.frag");

	model = glm::mat4(1.f);
	model = glm::translate(model, { 0.f, .5f, 0.f });
	model = glm::scale(model, glm::vec3(0.5f));
	view = glm::mat4(1.f);
	proj = glm::perspective(glm::radians(fov), (float)scrWidth / scrHeight, .1f, 100.f);
	
	cube.create(&cubeVerts);
	cube.material.albedo.loadTexture("Textures/Other/Marble.jpg");
	//cube.create(&cubeVerts);
	quad.create(&quadVerts);
	plane.create(&planeVerts);

	dirLight = DirLight({ -1.f, -1.f, -1.f }, glm::vec3(1.f));
	pointLight = PointLight({ 0.f, 1.f, 4.f }, glm::vec3(1.f));
	spotLight = SpotLight(cam.pos, cam.front, glm::vec3(1.f), glm::cos(glm::radians(12.5f)), glm::cos(glm::radians(15.f)));
	
	texture0.loadTexture("Textures/Other/Wood.png");

	//Uniforms and stuff
	mainShader.use();
	mainShader.setMat4("model", model);
	mainShader.setMat4("view", view);
	mainShader.setMat4("proj", proj);
	mainShader.set1f("specularExponent", 32.f);

	dirLight.set("dirLight", mainShader);
	pointLight.set("pointLight", mainShader);
	spotLight.set("spotLight", mainShader);

	lightBoxShader.use();
	lightBoxShader.setMat4("model", glm::scale(glm::mat4(1.f), glm::vec3(0.1f)));
	lightBoxShader.setMat4("view", view);
	lightBoxShader.setMat4("proj", proj);


	while (!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT);
		glClear(GL_DEPTH_BUFFER_BIT);
		DT::update();
		cam.processInput(window);
		view = cam.getView();

		mainShader.use();
		//model = glm::rotate(model, 0.0002f, glm::vec3(0.f, 1.f, 0.f));
		mainShader.setMat4("model", model);
		mainShader.setMat4("view", view);
		mainShader.setVec3("viewPos", cam.pos);

		pointLight.pos = glm::vec3(0.f, 1.f, (glm::sin(glfwGetTime()) + 1.4));
		pointLight.set("pointLight", mainShader);

		spotLight.pos = cam.pos;
		spotLight.dir = cam.front;
		spotLight.set("spotLight", mainShader);

		cube.draw(0);
		texture0.bind(0);
		mainShader.setMat4("model", glm::scale(glm::mat4(1.f), { 10.f, .1f, 10.f }));
		plane.draw();
		texture0.unbind(0);

		lightBoxShader.use();
		lightBoxShader.setMat4("view", view);
		lightBoxShader.setVec3("lightColor", pointLight.color);
		lightBoxShader.setVec3("lightPos", pointLight.pos);
		cube.draw(0);

		gradientSkyboxShader.use();
		gradientSkyboxShader.setMat4("view", view);
		glDepthFunc(GL_LEQUAL);
		quad.draw();
		glDepthFunc(GL_LESS);

		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	mLog("Program is now terminated.", Log::LogInfo);
	glfwTerminate();
	return 0;
}

//On Application Start
void initDependencies() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true); //TODO: this should be removed if I want performance

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	window = glfwCreateWindow(scrWidth, scrHeight, "GLRenderEngine", NULL, NULL); //Change the first null to glfwGetPrimaryMonitor() for fullscreen

	if (window == NULL) {
		mLog("Failed to create a window", Log::LogError);
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
	glfwSetKeyCallback(window, keyCallback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) { //Normal GLAD
		//if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress)) { //GLAD2
		mLog("Failed to initialize GLAD!", Log::LogError);
	}

	GLenum err = glewInit();
	if (err != GLEW_OK) {
		std::string st = "Glew Error: ";
		st += (const char*)glewGetErrorString(GLEW_OK);

		Log::log(st, Log::LogError, __LINE__);
	}

	//Debugging
	int flags; glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
	if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(glDebugOutput, nullptr);
		//glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
		glDebugMessageControl(GL_DEBUG_SOURCE_API,
			GL_DEBUG_TYPE_ERROR,
			GL_DEBUG_SEVERITY_HIGH,
			0, nullptr, GL_TRUE);
	}
	
	//Configure GL options
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_DEPTH_TEST);
	
	//Face cull
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	
	//Input setup
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouseCallback);
	
	//Background Color
	glClearColor(0.f, 0.f, 0.f, 1.f);
}

//Error checking from learnopengl.com
GLenum glCheckError_(const char* file, int line) {
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR)
	{
		std::string error;
		switch (errorCode)
		{
		case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
		case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
		case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
		case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
		case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
		case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
		}
		std::cout << error << " | " << file << " (" << line << ")" << std::endl;
	}
	return errorCode;
}
void APIENTRY glDebugOutput(GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length, const char* message, const void* userParam) {
	// ignore non-significant error/warning codes
	if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

	std::cout << "---------------" << std::endl;
	std::cout << "Debug message (" << id << "): " << message << std::endl;

	switch (source)
	{
	case GL_DEBUG_SOURCE_API:             std::cout << "Source: API"; break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::cout << "Source: Window System"; break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cout << "Source: Shader Compiler"; break;
	case GL_DEBUG_SOURCE_THIRD_PARTY:     std::cout << "Source: Third Party"; break;
	case GL_DEBUG_SOURCE_APPLICATION:     std::cout << "Source: Application"; break;
	case GL_DEBUG_SOURCE_OTHER:           std::cout << "Source: Other"; break;
	} std::cout << std::endl;

	switch (type)
	{
	case GL_DEBUG_TYPE_ERROR:               std::cout << "Type: Error"; break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "Type: Deprecated Behaviour"; break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cout << "Type: Undefined Behaviour"; break;
	case GL_DEBUG_TYPE_PORTABILITY:         std::cout << "Type: Portability"; break;
	case GL_DEBUG_TYPE_PERFORMANCE:         std::cout << "Type: Performance"; break;
	case GL_DEBUG_TYPE_MARKER:              std::cout << "Type: Marker"; break;
	case GL_DEBUG_TYPE_PUSH_GROUP:          std::cout << "Type: Push Group"; break;
	case GL_DEBUG_TYPE_POP_GROUP:           std::cout << "Type: Pop Group"; break;
	case GL_DEBUG_TYPE_OTHER:               std::cout << "Type: Other"; break;
	} std::cout << std::endl;

	switch (severity)
	{
	case GL_DEBUG_SEVERITY_HIGH:         std::cout << "Severity: high"; break;
	case GL_DEBUG_SEVERITY_MEDIUM:       std::cout << "Severity: medium"; break;
	case GL_DEBUG_SEVERITY_LOW:          std::cout << "Severity: low"; break;
	case GL_DEBUG_SEVERITY_NOTIFICATION: std::cout << "Severity: notification"; break;
	} std::cout << std::endl;
	std::cout << std::endl;
}

//On state change(buttons, resize, keys, etc.)
	//--UI

//--Window and OS
void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
	scrWidth = width;
	scrHeight = height;
	glViewport(0, 0, scrWidth, scrHeight);

	proj = glm::perspective(glm::radians(fov), (float)scrWidth / scrHeight, .1f, 100.f);
	mainShader.use();
	mainShader.setMat4("proj", proj);
	lightBoxShader.use();
	lightBoxShader.setMat4("proj", proj);
}
void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
	cam.processMouse(xpos, ypos);
}
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_E)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
}

//Every frame