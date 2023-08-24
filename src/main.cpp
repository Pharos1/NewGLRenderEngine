#include "pch.h"

#include "Render/Vertex.hpp"
#include "Render/Mesh.hpp"
#include "Render/Texture.hpp"
#include "Render/Model.hpp"
#include "Render/Shader.hpp"
#include "Render/Camera.hpp"
#include "Render/Light.hpp"
#include "Render/Framebuffer.hpp"
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

//On Application Start/End
void initDependencies();
void setupApplication();
void setupScreenRelated();
void setupUBOs();
void cleanup();

//Every frame
void draw(const Shader& shader);
void renderScene(const Shader& shader);

//Application Specific
uint32_t scrWidth = 1280;
uint32_t scrHeight = 920;
GLFWwindow* window;
float nearPlane = .05f;
float farPlane = 100.f;

//Shaders
Shader mainShader;
Shader lightBoxShader;
Shader gradientSkyboxShader;
Shader renderQuadShader;
Shader postprocShader;
Shader depthPrePassShader;
Shader deferredShader;

//Transformation
glm::mat4 view;
glm::mat4 proj;
glm::mat4 cubeModelMat;
glm::mat4 planeModelMat;
glm::mat4 cerberusModelMat;

//Objects
Mesh cube;
Mesh quad;
Mesh plane;
Model cerberusModel;
Model ballModel;
Model sponzaModel;
Model humanModel;

//Lights
DirLight dirLight;
PointLight pointLight;
SpotLight spotLight;

//Materials
Material woodTexture;
Material checkerboardTexture;
Material noiseTexture;
//Texture gPosition, gNormal, gAlbedo;

//Camera
float fov = 45.f;
Camera cam({ 0.f, .15f, .35f });

//Framebuffers
Framebuffer postprocFB;

//Object Data
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
	{{ 1.f, 0.f, 1.f}, { 0.f, 1.f, 0.f}, {1.f*1000, 0.f*1000}},
	{{ 1.f, 0.f,-1.f}, { 0.f, 1.f, 0.f}, {1.f*1000, 1.f*1000}},
	{{-1.f, 0.f,-1.f}, { 0.f, 1.f, 0.f}, {0.f*1000, 1.f*1000}},
	{{-1.f, 0.f, 1.f}, { 0.f, 1.f, 0.f}, {0.f*1000, 0.f*1000}},
	{{ 1.f, 0.f, 1.f}, { 0.f, 1.f, 0.f}, {1.f*1000, 0.f*1000}},
	{{-1.f, 0.f,-1.f}, { 0.f, 1.f, 0.f}, {0.f*1000, 1.f*1000}},
};

//Queries
Query depthPassQuery;
Query renderPassQuery;
Query postprocQuery;

//UBOs
GLuint uboMatrices;

int main() {
	std::filesystem::current_path(std::filesystem::path(__FILE__).parent_path().parent_path()); //Working dir = solution path

	//Basically initialization and setup
	initDependencies();
	setupApplication();

	double lastTime = 0;

	while (!glfwWindowShouldClose(window)) {
		double now = glfwGetTime();
		
		if (now - lastTime > 0.5) {
			depthPassQuery.retrieveResult();
			renderPassQuery.retrieveResult();
			postprocQuery.retrieveResult();

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

			lastTime = now;
		}
		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDepthFunc(GL_LEQUAL);

		Time::updateDelta();
		cam.processInput(window);
		view = cam.getView();

		glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		postprocShader.use();

		if (glfwGetKey(window, GLFW_KEY_0)) postprocShader.set1i("tonemapMode", 0);
		if (glfwGetKey(window, GLFW_KEY_1)) postprocShader.set1i("tonemapMode", 1);
		if (glfwGetKey(window, GLFW_KEY_2)) postprocShader.set1i("tonemapMode", 2);
		if (glfwGetKey(window, GLFW_KEY_3)) postprocShader.set1i("tonemapMode", 3);
		if (glfwGetKey(window, GLFW_KEY_4)) postprocShader.set1i("tonemapMode", 4);
		if (glfwGetKey(window, GLFW_KEY_5)) postprocShader.set1i("tonemapMode", 5);
		if (glfwGetKey(window, GLFW_KEY_6)) postprocShader.set1i("tonemapMode", 6);

		if (!glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1)) spotLight.setColor(glm::vec3(0));
		else spotLight.setColor(glm::vec3(80));
		if (!glfwGetKey(window, GLFW_KEY_F)) dirLight.setColor(glm::vec3(0));
		else dirLight.setColor(glm::vec3(2));
		if (!glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2)) pointLight.setColor(glm::vec3(0));
		else pointLight.setColor(glm::vec3(10));

		spotLight.set("spotLight", mainShader);
		dirLight.set("dirLight", mainShader);
		pointLight.set("pointLight", mainShader);

		//Update lights
		//pointLight.setPos(glm::vec3(0.f, .2f, (glm::sin(glfwGetTime()) + 1.4f) / 5.f);
		//pointLight.set("pointLight", mainShader);

		spotLight.setPos(cam.pos);
		spotLight.setDir(cam.front);
		spotLight.set("spotLight", mainShader);

		//Draw
		postprocFB.bind();
		postprocFB.clear();

		depthPassQuery.begin();
		renderScene(depthPrePassShader);
		depthPassQuery.end();

		renderPassQuery.begin();
		draw(mainShader);
		renderPassQuery.end();

		//Post-processing pass
		postprocFB.unbind();

		postprocQuery.begin();
		postprocShader.use();
		postprocFB.texture.bind(0);
		glDepthFunc(GL_LEQUAL);
		quad.draw();
		glDepthFunc(GL_LESS);
		postprocFB.texture.unbind();
		postprocQuery.end();

		//FINALY wait for the frame to finish
		glfwPollEvents();
		glfwSwapBuffers(window);
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

	scrWidth = std::max(width, 1); //Prevent zero width/height
	scrHeight = std::max(height, 1);
	glViewport(0, 0, scrWidth, scrHeight);

	proj = glm::perspective(glm::radians(fov), (float)scrWidth / scrHeight, nearPlane, farPlane);
	
	mainShader.use();
	mainShader.setMat4("proj", proj);

	lightBoxShader.use();
	lightBoxShader.setMat4("proj", proj);

	depthPrePassShader.use();
	depthPrePassShader.setMat4("proj", proj);

	//Recreate framebuffers
	setupScreenRelated();
	setupUBOs(); //For the proj
}
void cursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
	if (mouseLocked)
	cam.processMouse(xpos, ypos);
	else {
		cam.lastX = xpos; //DO this so the mouse dont skip when entering in view mode
		cam.lastY = ypos;
}
}
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods){
	if (button == GLFW_MOUSE_BUTTON_1 && !io.WantCaptureMouse) {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		mouseLocked = true;
		style->Alpha = 0.3f;
	}
}
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_E)
		glfwSetWindowShouldClose(window, GLFW_TRUE);

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		mouseLocked = false;
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		style->Alpha = windowAlpha;
}
}

//On Application Start/End
void initDependencies() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true); //PERFORMANCE NOTE: this should be removed if I want performance

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	window = glfwCreateWindow(scrWidth, scrHeight, "GLRenderEngine", NULL, NULL); //Change the first null to glfwGetPrimaryMonitor() for fullscreen

	if (window == NULL) {
		nLog("Failed to create a window", Log::LogError, "");
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
	glfwSetKeyCallback(window, keyCallback);
	glfwSwapInterval(0);
	//if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) { //Normal GLAD
		//if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress)) { //GLAD2
	//	mLog("Failed to initialize GLAD!", Log::LogError);
	//}

	GLenum err = glewInit();
	if (err != GLEW_OK)
		nLog(std::string("Glew Error: ") + (const char*)glewGetErrorString(GLEW_OK), Log::LogError, "");

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

	//Callbacks
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
	glfwSetCursorPosCallback(window, cursorPosCallback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);
	glfwSetKeyCallback(window, keyCallback);

	if(mouseLocked) glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//Background Color
	glClearColor(0.f, 0.f, 0.f, 1.f);

}
void setupApplication() {
	//Shaders
	mainShader.loadShader("src/Shaders/main.vert", "src/Shaders/main.frag");
	lightBoxShader.loadShader("src/Shaders/lightBox.vert", "src/Shaders/lightBox.frag");
	gradientSkyboxShader.loadShader("src/Shaders/basicQuad.vert", "src/Shaders/gradientSkybox.frag");
	renderQuadShader.loadShader("src/Shaders/basicQuad.vert", "src/Shaders/renderQuad.frag");
	postprocShader.loadShader("src/Shaders/basicQuad.vert", "src/Shaders/postproc.frag");
	depthPrePassShader.loadShader("src/Shaders/basicDraw.vert", "src/Shaders/depthPrePass.frag");
	deferredShader.loadShader("src/Shaders/main.vert", "src/Shaders/deferred.frag");

	//Transformation
	view = glm::mat4(1.f);
	proj = glm::perspective(glm::radians(fov), (float)scrWidth / scrHeight, nearPlane, farPlane);

	cubeModelMat = glm::mat4(1.f);
	cubeModelMat = glm::translate(cubeModelMat, { 0.f, .5f, 0.f });
	cubeModelMat = glm::scale(cubeModelMat, glm::vec3(0.5f));

	planeModelMat = glm::mat4(1.f);
	planeModelMat = glm::scale(planeModelMat, { 10.f, .1f, 10.f });

	cerberusModelMat = glm::mat4(1.f);
	cerberusModelMat = glm::scale(cerberusModelMat, glm::vec3(0.005f));
	cerberusModelMat = glm::translate(cerberusModelMat, glm::vec3(0.f, 30.0f, 0.f));

	//Objects
	cube.create(cubeVerts);
	quad.create(quadVerts);
	plane.create(planeVerts);

	//cerberusModel.loadModel("Models/Cerberus_by_Andrew_Maximov/Cerberus_LP.FBX");
	//cerberusModel.meshes[0].material.metallic.loadTexture("Models/Cerberus_by_Andrew_Maximov/Textures/Cerberus_M.tga");
	//cerberusModel.meshes[0].material.roughness.loadTexture("Models/Cerberus_by_Andrew_Maximov/Textures/Cerberus_R.tga");
	//cerberusModel.meshes[0].material.normal.loadTexture("Models/Cerberus_by_Andrew_Maximov/Textures/Cerberus_N.tga");
	
	sponzaModel.loadModel("Models/Sponza/sponza.glTF");
	humanModel.loadModel("Models/Human/scene.gltf"); nLog("Human Model's Normal Textures are broken. See the textures bro", Log::LogInfo, "MAIN");
	//loadSponz();
	//loadHuman();

	//{
	//	std::lock_guard<std::mutex> lock(mutexLock);
	//	Timer time("Heyl");
	//std::jthread thread(loadSponz);
	//std::jthread thread2(loadHuman);
	//thread.join();
	//thread2.join();
	//}
	//ballModel.loadModel("Models/sphere.stl");
	//ballModel.meshes[0].material.albedo.loadTexture("Textures/Debug/white.png");

	//Lights
	dirLight = DirLight({ -1.f, -1.f, -1.f }, glm::vec3(5.f * 0));
	pointLight = PointLight({ 0.f, .20f, .8f }, glm::vec3((10.f + 5) * 1));
	spotLight = SpotLight(cam.pos, cam.front, glm::vec3((20.f + 60.f) * 1), glm::cos(glm::radians(12.5f)), glm::cos(glm::radians(15.f)));

	//Textures
	woodTexture.addTexture("Textures/Other/Wood.png", 0);
	checkerboardTexture.addTexture("Textures/Props/Checkerboard.jpg", 0);

	//Uniforms and stuff
	mainShader.use();
	mainShader.setMat4("model", glm::mat4(1.f));
	mainShader.setMat4("view", view);
	mainShader.setMat4("proj", proj);
	mainShader.set1f("specularExponent", 32.f);

	dirLight.set("dirLight", mainShader);
	pointLight.set("pointLight", mainShader);
	spotLight.set("spotLight", mainShader);

	mainShader.use();
	mainShader.setMat4("model", glm::mat4(1.f));
	mainShader.setMat4("view", view);

	lightBoxShader.use();
	lightBoxShader.setMat4("model", glm::scale(glm::mat4(1.f), glm::vec3(0.1f)));
	lightBoxShader.setMat4("view", view);
	lightBoxShader.setMat4("proj", proj);

	postprocShader.use();
	postprocShader.set1b("gammaOn", true);
	postprocShader.set1f("gamma", 2.2f);
	postprocShader.set1i("tonemapMode", 5);

	//Queries
	depthPassQuery.loadQuery(GL_TIME_ELAPSED);
	renderPassQuery.loadQuery(GL_TIME_ELAPSED);
	postprocQuery.loadQuery(GL_TIME_ELAPSED);

	setupScreenRelated();
	setupUBOs();

	cam.calcFrontVec(); //Update it, so it doesn't jump on start

	//Queries
	renderPassQuery.loadQuery(GL_TIME_ELAPSED);

}
void setupUBOs() {
	if (!uboMatrices) {
		glGenBuffers(1, &uboMatrices);

		glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
		glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		glBindBufferRange(GL_UNIFORM_BUFFER, 0, uboMatrices, 0, 2 * sizeof(glm::mat4));
	}

	glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(proj));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
void cleanup() {
	glfwTerminate();
}

//Every frame
void draw(const Shader& shader) {
	renderScene(shader);

	//Light cube pass
	lightBoxShader.use();
	lightBoxShader.setMat4("view", view);
	lightBoxShader.setVec3("lightColor", pointLight.getColor());
	lightBoxShader.setVec3("lightPos", pointLight.getPos());
	cube.draw();

	//Ugly skybox pass
	gradientSkyboxShader.use();
	gradientSkyboxShader.setMat4("view", view);

	quad.draw();
}
void renderScene(const Shader& shader) {
	shader.use();
	
	//Update lights
	//pointLight.setPos(glm::vec3(0.f, .2f, (glm::sin(glfwGetTime()) + 1.4f) / 5.f);
	//pointLight.set("pointLight", mainShader);

	spotLight.setPos(cam.pos);
	spotLight.setDir(cam.front);
	spotLight.set("spotLight", shader);

	//Set shader uniforms
	shader.setMat4("view", view);
	shader.setVec3("viewPos", cam.pos);

	//Scene draw pass
	//shader.setMat4("model", glm::scale(glm::mat4(1.f), glm::vec3(0.002f)));
	//modelBall.draw(0);

	shader.setMat4("model", cerberusModelMat);
	//cerberusModel.draw(0);

	shader.setMat4("model", glm::mat4(1.f));
	sponzaModel.draw();

	shader.setMat4("model", glm::translate(glm::scale(glm::mat4(1.f), glm::vec3(.03f)), { -15.f, 1.f, 0.f }));
	humanModel.draw();

	shader.setMat4("model", glm::scale(glm::mat4(1.f), glm::vec3(1000.f)));
	checkerboardTexture.bind();
	plane.draw();
	checkerboardTexture.unbind();

	shader.setMat4("model", cubeModelMat);
	woodTexture.bind();
	cube.draw();
	woodTexture.unbind();
}