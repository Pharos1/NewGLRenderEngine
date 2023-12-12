#include "pch.h"

#include "GL/Texture.hpp"
#include "GL/Shader.hpp"
#include "Object/Vertex.hpp"
#include "Object/Mesh.hpp"
#include "Object/Model.hpp"
#include "Object/Entity.hpp"
#include "Renderer/Camera.hpp"
#include "Renderer/Light.hpp"
#include "Renderer/Renderer.hpp"
#include "GL/Framebuffer.hpp"
#include "Utilities/Logger.hpp"
#include "Utilities/Time.hpp"
#include "Utilities/Query.hpp"

#define glCheckError() glCheckError_(__FILE__, __LINE__)

//Error checking from learnopengl.com
GLenum glCheckError_(const char* file, int line);
void APIENTRY glDebugOutput(GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length, const char* message, const void* userParam);

//Callbacks
void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

void cleanup();

//Objects
Mesh cube;
Entity planeEntity;
Entity sponzaEntity;
Entity humanEntity;

//Lights
DirLight dirLight;
PointLight pointLight;
SpotLight spotLight;

//Materials
//Material noiseTexture;
//Texture gPosition, gNormal, gAlbedo;

//Camera
Camera cam({ 0.f, .15f, .35f });

//Object Data
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
	{{ 1.f, 0.f, 1.f}, { 0.f, 1.f, 0.f}, {1.f*25, 0.f*25}},
	{{ 1.f, 0.f,-1.f}, { 0.f, 1.f, 0.f}, {1.f*25, 1.f*25}},
	{{-1.f, 0.f,-1.f}, { 0.f, 1.f, 0.f}, {0.f*25, 1.f*25}},
	{{-1.f, 0.f, 1.f}, { 0.f, 1.f, 0.f}, {0.f*25, 0.f*25}},
	{{ 1.f, 0.f, 1.f}, { 0.f, 1.f, 0.f}, {1.f*25, 0.f*25}},
	{{-1.f, 0.f,-1.f}, { 0.f, 1.f, 0.f}, {0.f*25, 1.f*25}},
};

//Renderer
Renderer renderer;

int main() {
	renderer.initDependencies();

	//Setup Debugging
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

	//Register Callbacks
	glfwSetFramebufferSizeCallback(renderer.window, framebufferSizeCallback);
	glfwSetCursorPosCallback(renderer.window, cursorPosCallback);
	glfwSetMouseButtonCallback(renderer.window, mouseButtonCallback);
	glfwSetKeyCallback(renderer.window, keyCallback);

	//Renderer Setup
	Timer applicationLoadTimer("Setup Application"); applicationLoadTimer.start();

	//Transformation
	cam.updateProj((float)renderer.scrWidth / renderer.scrHeight);

	//Lights
	dirLight = DirLight({ -1.f, -4.f, -1.f }, glm::vec3(4.f), true);
	pointLight = PointLight({ 0.f, .20f, .8f }, glm::vec3((10.f + 5)), false);
	spotLight = SpotLight(cam.pos, cam.front, glm::vec3((20.f + 60.f)), 12.5f, 15.f, false);

	applicationLoadTimer.stop();

	double lastTime = 0;
	double lastSpotTime = 0; //For spotLight on/off delay

	//Entities
	renderer.entities.push_back(&sponzaEntity);
	renderer.entities.push_back(&humanEntity);
	renderer.entities.push_back(&planeEntity);

	cube.create(cubeVerts);

	planeEntity.meshes.push_back(Mesh(planeVerts)); planeEntity.updateVertAndIndexCount();
	planeEntity.meshes[0].material.addTexture("Textures/Props/Checkerboard.jpg", 0);
	sponzaEntity.loadModel("Models/Sponza/sponza.glTF");
	//humanEntity.loadModel("Models/Characters/heita/heita.obj");

	planeEntity.transform.setLocalScale(glm::vec3(100.f));
	humanEntity.transform.setLocalPos(glm::vec3(-1.f, 0.f, -1.f));
	//humanEntity.transform.setLocalScale(glm::vec3(.03f));
	humanEntity.transform.setLocalScale(glm::vec3(.075f));

	renderer.dirLight = &dirLight;
	renderer.pointLights.push_back(&pointLight);
	renderer.spotLights.push_back(&spotLight);

	renderer.cam = &cam;

	renderer.init();

	while (!glfwWindowShouldClose(renderer.window)) {
		double now = glfwGetTime();
		
		if (now - lastTime > 1) {
			renderer.depthPassQuery.retrieveResult();
			renderer.renderPassQuery.retrieveResult();
			renderer.postprocQuery.retrieveResult();
			renderer.guiPassQuery.retrieveResult();
			renderer.shadowPassQuery.retrieveResult();
			renderer.fxaaPassQuery.retrieveResult();
			lastTime = now;
			/*
			double depthTime = (double)depthPassQuery.getResult() / 1000000;
			double renderTime = (double)renderPassQuery.getResult() / 1000000;
			double postprocTime = (double)postprocQuery.getResult() / 1000000;

			std::stringstream ss;

			ss << "Depth pre-pass: " << depthTime << "ms  |  ";
			ss << "Render Pass: " << renderTime << "ms  |  ";
			ss << "Post-Processing Pass: " << postprocTime << "ms  |  ";
			ss << "Summed query time: " << depthTime + renderTime + postprocTime << "ms  |  ";
			ss << "Chrono Frame Time: " << Time::avgMsTime << "ms\n";

			glfwSetWindowTitle(window, ss.str().c_str());
			*/
		}
		
		Time::updateDelta();
		if (renderer.mouseLocked) cam.processInput(renderer.window);

		if (now - lastSpotTime > .2f) {
			if (glfwGetKey(renderer.window, GLFW_KEY_F)) {
				spotLight.enabled = !spotLight.enabled;
				lastSpotTime = now;
			}
		}

		spotLight.setPos(cam.pos);
		spotLight.setDir(cam.front);

		renderer.mainShader.use();
		renderer.mainShader.setVec3("viewPos", cam.pos);

		renderer.render();
	}

	nLog("Application stopped.", Log::LogInfo, "MAIN");
	cleanup();
	return 0;
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

//Callbacks
void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
	if (!glfwGetWindowAttrib(window, GLFW_FOCUSED)) //Protect from frambuffer error when alt+tabbing in fullscreen(Cuz alt+tab calls the func)
		return;

	renderer.scrWidth = std::max(width, 1); //Prevent zero width/height
	renderer.scrHeight = std::max(height, 1);
	glViewport(0, 0, renderer.scrWidth, renderer.scrHeight);

	cam.updateProj((float)renderer.scrWidth / renderer.scrHeight);//proj = glm::perspective(glm::radians(fov), (float)scrWidth / scrHeight, nearPlane, farPlane);

	renderer.fxaaShader.use();
	renderer.fxaaShader.setVec2("inverseScreenSize", glm::vec2(1.f / renderer.scrWidth, 1.f / renderer.scrHeight));

	//Recreate framebuffers
	renderer.setupFramebuffers();
	renderer.setupUBOs(); //For the proj
}
void cursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
	ImGui_ImplGlfw_CursorPosCallback(window, xpos, ypos);

	if (renderer.mouseLocked)
		cam.processMouse(xpos, ypos);
	else {
		cam.lastX = xpos; //DO this so the mouse dont skip when entering in view mode
		cam.lastY = ypos;
	}
}
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods){
	ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
	
	if (button == GLFW_MOUSE_BUTTON_1 && !renderer.io.WantCaptureMouse) {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		renderer.mouseLocked = true;
		renderer.style->Alpha = 0.3f;
	}
}
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
	
	if (key == GLFW_KEY_E)
		glfwSetWindowShouldClose(window, GLFW_TRUE);

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		renderer.mouseLocked = false;
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		renderer.style->Alpha = renderer.windowAlpha;
	}
}

//On Application Start/End
void cleanup() {
	renderer.cleanupGUI();
	glfwTerminate();
}