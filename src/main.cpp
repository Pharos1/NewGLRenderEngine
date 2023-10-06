#include "pch.h"

#include "Render/Vertex.hpp"
#include "Render/Mesh.hpp"
#include "Render/Texture.hpp"
#include "Render/Model.hpp"
#include "Render/Shader.hpp"
#include "Render/Camera.hpp"
#include "Render/Light.hpp"
#include "Render/Framebuffer.hpp"
#include "Render/Entity.hpp"
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
void initImGui();
void setupUBOs();
void cleanup();

//Every frame
void draw(const Shader& shader);

//CSM
std::vector<glm::vec4> getFrustumCornersWorldSpace(const glm::mat4& proj, const glm::mat4& view);
glm::mat4 getLightSpaceMatrix(const float nearPlane, const float farPlane);
std::vector<glm::mat4> getLightSpaceMatrices();

//Application Specific
uint32_t scrWidth = 1280;
uint32_t scrHeight = 920;
GLFWwindow* window;

//Shaders
Shader mainShader;
Shader lightBoxShader;
Shader renderQuadShader;
Shader postprocShader;
Shader depthPassShader;
Shader deferredShader;
Shader fxaaShader;
Shader CSMShader;

//Objects
Mesh cube;
Mesh quad;
Entity planeEntity;
Entity sponzaEntity;
Entity humanEntity;
Entity sceneEntity;

//Lights
DirLight dirLight;
PointLight pointLight;
SpotLight spotLight;

//Materials
//Material noiseTexture;
//Texture gPosition, gNormal, gAlbedo;

//Camera
Camera cam({ 0.f, .15f, .35f });

//Framebuffers
Framebuffer postprocFB;
Framebuffer fxaaFB;
Framebuffer shadowFB;

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
	{{ 1.f, 0.f, 1.f}, { 0.f, 1.f, 0.f}, {1.f*25, 0.f*25}},
	{{ 1.f, 0.f,-1.f}, { 0.f, 1.f, 0.f}, {1.f*25, 1.f*25}},
	{{-1.f, 0.f,-1.f}, { 0.f, 1.f, 0.f}, {0.f*25, 1.f*25}},
	{{-1.f, 0.f, 1.f}, { 0.f, 1.f, 0.f}, {0.f*25, 0.f*25}},
	{{ 1.f, 0.f, 1.f}, { 0.f, 1.f, 0.f}, {1.f*25, 0.f*25}},
	{{-1.f, 0.f,-1.f}, { 0.f, 1.f, 0.f}, {0.f*25, 1.f*25}},
};

//Queries
Query depthPassQuery;
Query renderPassQuery;
Query postprocQuery;
Query guiPassQuery;
Query shadowPassQuery;
Query fxaaPassQuery;

//Software and Hardwere Info
std::string openGLVersion;
std::string glslVersion;
std::string vendor;
std::string gpuVersion;

//OpenGL Options
bool vsyncOn = false;
bool mouseLocked = true;
int msaaSamples = 4;
bool msaaEnabled = true;
int msaaMaxSamples = 0;

//Post-processing Options
float exposure = 2.2f;
float gamma = 2.2f;
bool gammaOn = true;
int tonemapMode = 5;
float maxRadiance = 1.f;

//FXAA
#define DEFAULT_EDGE_THRESHOLD_MIN .0312f;
#define DEFAULT_EDGE_THRESHOLD_MAX .125f;
#define DEFAULT_ITERATIONS 12;
#define DEFAULT_SUBPIXEL_QUALITY .75f;
float EDGE_THRESHOLD_MIN = DEFAULT_EDGE_THRESHOLD_MIN;
float EDGE_THRESHOLD_MAX = DEFAULT_EDGE_THRESHOLD_MAX;
int ITERATIONS = DEFAULT_ITERATIONS;
float SUBPIXEL_QUALITY = DEFAULT_SUBPIXEL_QUALITY;
bool fxaaRevert = false;
bool fxaaEnabled = true;

//UBOs
GLuint uboMatrices;
GLuint lightMatricesUBO;

//CSM
float cascadeCount = 3;
bool csmEnabled = true;
uint8_t cascadeCount = 4;
std::vector<float> shadowCascadeLevels; //{ farPlane / 50.0f, farPlane / 25.0f, farPlane / 10.0f, farPlane / 2.0f };
#define DEFAULT_SHADOW_RESOLUTION 2048
GLuint shadowResolution = DEFAULT_SHADOW_RESOLUTION;
GLuint newShadowResolution = DEFAULT_SHADOW_RESOLUTION;
bool cascadeDebugView = false;
bool freezeCSM = false;

int main() {
	std::filesystem::current_path(std::filesystem::path(__FILE__).parent_path().parent_path()); //Working dir = solution path

	//Basically initialization and setup
	initDependencies();
	setupApplication();

	double lastTime = 0;
	double lastSpotTime = 0; //For spotLight on/off delay

	while (!glfwWindowShouldClose(window)) {
		double now = glfwGetTime();
		
		if (now - lastTime > 1) {
			depthPassQuery.retrieveResult();
			renderPassQuery.retrieveResult();
			postprocQuery.retrieveResult();
			guiPassQuery.retrieveResult();
			shadowPassQuery.retrieveResult();
			fxaaPassQuery.retrieveResult();
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
		if (mouseLocked) cam.processInput(window);

		if (now - lastSpotTime > .2f) {
			if (glfwGetKey(window, GLFW_KEY_F)) {
			spotLight.enabled = !spotLight.enabled;
			lastSpotTime = now;
		}
		}

		spotLight.setPos(cam.pos);
		spotLight.setDir(cam.front);

		spotLight.set("spotLight", mainShader);
		dirLight.set("dirLight", mainShader);
		pointLight.set("pointLight", mainShader);

		mainShader.use();
		mainShader.setVec3("viewPos", cam.pos);

		if (!freezeCSM) {
		const auto lightMatrices = getLightSpaceMatrices();
		glBindBuffer(GL_UNIFORM_BUFFER, lightMatricesUBO);
		for (size_t i = 0; i < lightMatrices.size(); i++) {
				glBufferSubData(GL_UNIFORM_BUFFER, i * sizeof(glm::mat4), sizeof(glm::mat4), &lightMatrices[i]);
		}
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
		}
		
		shadowPassQuery.begin();
		if (csmEnabled) {
		glDepthFunc(GL_LESS);
		glViewport(0, 0, shadowResolution, shadowResolution);
		shadowFB.bind();
		shadowFB.clear();
		glCullFace(GL_FRONT);
		glEnable(GL_DEPTH_CLAMP); //From a comment by https://disqus.com/by/disqus_XCUOEk9iLH/? on disqus.
		sceneEntity.draw(CSMShader);
		glDisable(GL_DEPTH_CLAMP);
		glCullFace(GL_BACK);
		shadowFB.unbind();
		glViewport(0, 0, scrWidth, scrHeight);
		}
		shadowPassQuery.end();
		
		glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(cam.view));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
		
		//Framebuffer::clear();
		//renderQuadShader.use();
		//glDepthFunc(GL_LEQUAL);
		//shadowFB.texture.bind(0);
		//quad.draw();

		draw(mainShader);

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

	cam.updateProj((float)scrWidth / scrHeight);//proj = glm::perspective(glm::radians(fov), (float)scrWidth / scrHeight, nearPlane, farPlane);

	fxaaShader.use();
	fxaaShader.setVec2("inverseScreenSize", glm::vec2(1.f / scrWidth, 1.f / scrHeight));

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
	if (button == GLFW_MOUSE_BUTTON_1 && !GUI::io.WantCaptureMouse) {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		mouseLocked = true;
		GUI::style->Alpha = 0.3f;
	}
}
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_E)
		glfwSetWindowShouldClose(window, GLFW_TRUE);

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		mouseLocked = false;
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		GUI::style->Alpha = GUI::windowAlpha;
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
	glfwSwapInterval(vsyncOn);
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

	//Software and Hardware Info
	openGLVersion = (char*)glGetString(GL_VERSION);
	glslVersion = (char*)glGetString(GL_SHADING_LANGUAGE_VERSION);
	vendor = (char*)glGetString(GL_VENDOR);
	gpuVersion = (char*)glGetString(GL_RENDERER);

	//Configure GL options
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_DEPTH_TEST);

	glfwWindowHint(GLFW_SAMPLES, msaaSamples);
	if (msaaEnabled) glEnable(GL_MULTISAMPLE);
	glGetIntegerv(GL_MAX_INTEGER_SAMPLES, &msaaMaxSamples);

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
	glClearColor(1.f, 0.f, 1.f, 1.f);
}
void setupApplication() {
	ScopedTimer t1("Setup Application");

	//Shaders
	mainShader.loadShader("src/Shaders/main.vert", "src/Shaders/main.frag");
	lightBoxShader.loadShader("src/Shaders/lightBox.vert", "src/Shaders/lightBox.frag");
	renderQuadShader.loadShader("src/Shaders/basicQuad.vert", "src/Shaders/renderQuad.frag");
	postprocShader.loadShader("src/Shaders/basicQuad.vert", "src/Shaders/postproc.frag");
	depthPassShader.loadShader("src/Shaders/basicDraw.vert", "src/Shaders/depthPrePass.frag");
	deferredShader.loadShader("src/Shaders/main.vert", "src/Shaders/deferred.frag");
	fxaaShader.loadShader("src/Shaders/basicQuad.vert", "src/Shaders/fxaa.frag");
	CSMShader.loadShader("src/Shaders/CSM.vert", "src/Shaders/CSM.frag", "src/Shaders/CSM.geom");

	//Transformation
	cam.updateProj((float)scrWidth / scrHeight);

	//Objects
	cube.create(cubeVerts);
	quad.create(quadVerts);

	planeEntity.meshes.push_back(Mesh(planeVerts)); planeEntity.updateVertCount();
	planeEntity.meshes[0].material.addTexture("Textures/Props/Checkerboard.jpg", 0);
	sponzaEntity.loadModel("Models/Sponza/sponza.glTF");
	//humanEntity.loadModel("Models/Characters/heita/heita.obj");

	sceneEntity.addChild(sponzaEntity);
	sceneEntity.addChild(humanEntity);
	sceneEntity.addChild(planeEntity);
	
	planeEntity.transform.setLocalScale(glm::vec3(100.f));
	humanEntity.transform.setLocalPos(glm::vec3(-1.f, 0.f, 0.f));
	humanEntity.transform.setLocalScale(glm::vec3(.03f));

	//Lights
	dirLight = DirLight({ -1.f, -4.f, -1.f }, glm::vec3(4.f), true);
	pointLight = PointLight({ 0.f, .20f, .8f }, glm::vec3((10.f + 5)), false);
	spotLight = SpotLight(cam.pos, cam.front, glm::vec3((20.f + 60.f)), 12.5f, 15.f, false);

	//Uniforms and stuff
	mainShader.use();
	mainShader.set1f("specularExponent", 32.f);
	mainShader.set1f("farPlane", cam.farPlane);

	dirLight.set("dirLight", mainShader);
	pointLight.set("pointLight", mainShader);
	spotLight.set("spotLight", mainShader);

	lightBoxShader.use();
	lightBoxShader.setMat4("model", glm::scale(glm::mat4(1.f), glm::vec3(0.1f)));

	postprocShader.use();
	postprocShader.set1b("gammaOn", gammaOn);
	postprocShader.set1f("exposure", exposure);
	postprocShader.set1f("gamma", gamma);
	postprocShader.set1f("maxRadiance", maxRadiance);
	postprocShader.set1i("tonemapMode", tonemapMode);

	fxaaShader.use();
	fxaaShader.set1b("fxaaEnabled", fxaaEnabled);
	fxaaShader.setVec2("inverseScreenSize", glm::vec2(1.f / scrWidth, 1.f / scrHeight));
	fxaaShader.set1f("EDGE_THRESHOLD_MIN", EDGE_THRESHOLD_MIN);
	fxaaShader.set1f("EDGE_THRESHOLD_MAX", EDGE_THRESHOLD_MAX);
	fxaaShader.set1i("ITERATIONS", ITERATIONS);
	fxaaShader.set1f("SUBPIXEL_QUALITY", SUBPIXEL_QUALITY);

	//Queries
	depthPassQuery.loadQuery(GL_TIME_ELAPSED);
	renderPassQuery.loadQuery(GL_TIME_ELAPSED);
	postprocQuery.loadQuery(GL_TIME_ELAPSED);
	guiPassQuery.loadQuery(GL_TIME_ELAPSED);
	shadowPassQuery.loadQuery(GL_TIME_ELAPSED);
	fxaaPassQuery.loadQuery(GL_TIME_ELAPSED);

	//Generate and set Cascades Far Planes
	mainShader.use();
	mainShader.set1b("csmEnabled", csmEnabled);
	mainShader.set1i("cascadeCount", cascadeCount);

	shadowCascadeLevels.clear();
	float lambda = 0.9f;// .8f;
	float virtualNearPlane = 1.f;// nearPlane;// 1.f;
	for (uint32_t i = 1; i < cascadeCount; i++) { //Practical view frustum split (combination between uniform and logarithmic)
		float k = (float)i / (float)cascadeCount;
		float f = lambda * (virtualNearPlane * powf(cam.farPlane / virtualNearPlane, k)) + (1.f - lambda) * (virtualNearPlane + (cam.farPlane - virtualNearPlane) * k);
		shadowCascadeLevels.push_back(f);
	}
	shadowCascadeLevels.push_back(cam.farPlane);

	for (uint32_t i = 0; i < cascadeCount; i++) {
		mainShader.set1f("cascadePlaneDistances[" + std::to_string(i) + "]", shadowCascadeLevels[i]);
	}

	setupScreenRelated();
	initImGui();
	setupUBOs();
}
void setupScreenRelated() {
	//Framebuffers
	postprocFB.create2D(GL_TEXTURE_2D, scrWidth, scrHeight, GL_RGBA16F);
	fxaaFB.create2D(GL_TEXTURE_2D, scrWidth, scrHeight, GL_RGBA, GL_RGBA, NULL);

	shadowFB.createLayered2D(GL_TEXTURE_2D_ARRAY, shadowResolution, shadowResolution, cascadeCount, GL_DEPTH_COMPONENT16, GL_DEPTH_COMPONENT, NULL, GL_DEPTH_ATTACHMENT, GL_FLOAT);
	shadowFB.texture.setBorderColor({ 1.f, 1.f, 1.f, 1.f });
	shadowFB.texture.setFilterMin(GL_NEAREST);
	shadowFB.texture.setFilterMag(GL_NEAREST);
	shadowFB.texture.setWrapS(GL_CLAMP_TO_BORDER);
	shadowFB.texture.setWrapT(GL_CLAMP_TO_BORDER);
}
void initImGui(){
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	//ImGuiIO& io = GetIO(); (void)io;
	//StyleColorsDark();

	//Style
	if (GUI::currentStyle == 0) ImGui::StyleColorsClassic();
	else if (GUI::currentStyle == 1) ImGui::StyleColorsDark();
	else if (GUI::currentStyle == 2) ImGui::StyleColorsLight();

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 450");
	GUI::style = &ImGui::GetStyle();
	GUI::windowAlpha = GUI::style->Alpha;

	GUI::style->Alpha = .3f;
}
void setupUBOs() {
	if (!uboMatrices) {
		glGenBuffers(1, &uboMatrices);

		glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
		glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
		glBindBufferBase(GL_UNIFORM_BUFFER, 0, uboMatrices);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(cam.proj));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	if (!lightMatricesUBO) {
		glGenBuffers(1, &lightMatricesUBO);
	
		glBindBuffer(GL_UNIFORM_BUFFER, lightMatricesUBO);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) * 16, NULL, GL_STATIC_DRAW);
		glBindBufferBase(GL_UNIFORM_BUFFER, 1, lightMatricesUBO);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	
}
}
void cleanup() {
	GUI::cleanup();
	glfwTerminate();
}

//Every frame
void draw(const Shader& shader) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glDepthFunc(GL_LEQUAL);

	//Draw
	postprocFB.bind();
	postprocFB.clear();

	depthPassQuery.begin();
	sceneEntity.draw(depthPassShader);
	depthPassQuery.end();

	renderPassQuery.begin();
	shadowFB.texture.bind(4);
	sceneEntity.draw(shader);

	//Light cube pass
	lightBoxShader.use();
	lightBoxShader.setVec3("lightColor", pointLight.getColor());
	lightBoxShader.setVec3("lightPos", pointLight.getPos());
	cube.draw();

	renderPassQuery.end();

	//Post-processing pass
	postprocFB.unbind();
	fxaaFB.bind();

	postprocQuery.begin();
	postprocShader.use();
	postprocFB.texture.bind(0);
	glDepthFunc(GL_LEQUAL);
	quad.draw();
	glDepthFunc(GL_LESS);
	postprocFB.texture.unbind();
	postprocQuery.end();

	fxaaFB.unbind();

	fxaaPassQuery.begin();
	fxaaShader.use();
	fxaaFB.texture.bind(0);
	glDepthFunc(GL_LEQUAL);
	quad.draw();
	glDepthFunc(GL_LESS);
	fxaaPassQuery.end();

	guiPassQuery.begin();
	GUI::updateGUI();
	guiPassQuery.end();
}

//CSM
std::vector<glm::vec4> getFrustumCornersWorldSpace(const glm::mat4& proj, const glm::mat4& view) {
	const auto inv = glm::inverse(proj * view);

	std::vector<glm::vec4> frustumCorners;
	for (unsigned int x = 0; x < 2; ++x) {
		for (unsigned int y = 0; y < 2; ++y) {
			for (unsigned int z = 0; z < 2; ++z) {
				const glm::vec4 pt =
					inv * glm::vec4(
						2.0f * x - 1.0f,
						2.0f * y - 1.0f,
						2.0f * z - 1.0f,
						1.0f);
				frustumCorners.push_back(pt / pt.w);
			}
		}
	}

	return frustumCorners;
}
glm::mat4 getLightSpaceMatrix(const float nearPlane, const float farPlane) {
	const auto proj = glm::perspective(glm::radians(cam.fov), (float)scrWidth / scrHeight, nearPlane, farPlane);

	auto corners = getFrustumCornersWorldSpace(proj, cam.view);
	glm::vec3 center = glm::vec3(0, 0, 0);
	for (const auto& v : corners) {
		center += glm::vec3(v);
	}
	center /= corners.size();

	const auto lightView = glm::lookAt(center - dirLight.getDir(), center, glm::vec3(0.0f, 1.0f, 0.0f));


	float minX = std::numeric_limits<float>::max();
	float maxX = std::numeric_limits<float>::lowest();
	float minY = std::numeric_limits<float>::max();
	float maxY = std::numeric_limits<float>::lowest();
	float minZ = std::numeric_limits<float>::max();
	float maxZ = std::numeric_limits<float>::lowest();
	for (const auto& v : corners) {
		const auto trf = lightView * v;
		minX = std::min(minX, trf.x);
		maxX = std::max(maxX, trf.x);
		minY = std::min(minY, trf.y);
		maxY = std::max(maxY, trf.y);
		minZ = std::min(minZ, trf.z);
		maxZ = std::max(maxZ, trf.z);
	}

	//Tune this parameter according to the scene. Changes the depth of the light frustum. Therefor changes the precision of the depth value that is written to the buffer.
	constexpr float zMult = 10.f;
	if (minZ < 0) {
		minZ *= zMult;
	}
	else {
		minZ /= zMult;
	}
	if (maxZ < 0) {
		maxZ /= zMult;
	}
	else {
		maxZ *= zMult;
	}

	glm::mat4 lightProjection = glm::ortho(minX, maxX, minY, maxY, minZ, maxZ);

	return lightProjection * lightView;
}
std::vector<glm::mat4> getLightSpaceMatrices() {
	std::vector<glm::mat4> ret;
	for (uint32_t i = 0; i < cascadeCount; i++) {
		if (i == 0) {
			ret.push_back(getLightSpaceMatrix(cam.nearPlane, shadowCascadeLevels[i]));
		}
		else {
			ret.push_back(getLightSpaceMatrix(shadowCascadeLevels[i - 1], shadowCascadeLevels[i]));
		}
	}
	return ret;
}

namespace GUI {
	//GUI
	ImGuiIO io;
	ImGuiStyle* style;
	float windowAlpha;
	
	//UI Options
	int currentStyle = 0; //0 - Classic, 1 - Dark, 2 - Light
	float bgAlpha = .8f;

void updateGUI() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	io = ImGui::GetIO();

	ImGuiStyle& style = ImGui::GetStyle();

	ImGui::SetNextWindowSizeConstraints(ImVec2(200, scrHeight), ImVec2(scrWidth, scrHeight));
	ImGui::SetNextWindowBgAlpha(bgAlpha);

	ImGui::Begin("Option Window", NULL, ImGuiWindowFlags_NoMove);
	ImGui::SetWindowPos(ImVec2(scrWidth - ImGui::GetWindowWidth(), 0));

	if (ImGui::CollapsingHeader("Rendering", ImGuiTreeNodeFlags_DefaultOpen)) {
		if (ImGui::TreeNode("Post-processing")) {
			ImGui::Text("Anti-Aliasing");
			if (ImGui::Checkbox("FXAA", &fxaaEnabled)) {
				fxaaShader.use();
				fxaaShader.set1b("fxaaEnabled", fxaaEnabled);
			}

			ImGui::NewLine();

			ImGui::Text("FXAA Options");
			ImGui::BeginDisabled(!fxaaEnabled);
			if (ImGui::SliderFloat("FXAA_EDGE_THRESHOLD_MIN", &EDGE_THRESHOLD_MIN, 0, 1, "%.4f")) {
				fxaaShader.use();
				fxaaShader.set1f("EDGE_THRESHOLD_MIN", EDGE_THRESHOLD_MIN);
				fxaaRevert = true;
			}
			if (ImGui::SliderFloat("FXAA_EDGE_THRESHOLD_MAX", &EDGE_THRESHOLD_MAX, 0, 1, "%.4f")) {
				fxaaShader.use();
				fxaaShader.set1f("EDGE_THRESHOLD_MAX", EDGE_THRESHOLD_MAX);
				fxaaRevert = true;
			}
			if (ImGui::SliderInt("FXAA_ITERATIONS", &ITERATIONS, 0, 12)) {
				fxaaShader.use();
				fxaaShader.set1i("ITERATIONS", ITERATIONS);
				fxaaRevert = true;
			}
			if (ImGui::SliderFloat("FXAA_SUBPIXEL_QUALITY", &SUBPIXEL_QUALITY, 0, 1, "%.4f")) {
				fxaaShader.use();
				fxaaShader.set1f("SUBPIXEL_QUALITY", SUBPIXEL_QUALITY);
				fxaaRevert = true;
			}
			if (fxaaRevert) {
				if (ImGui::Button("Revert")) {
					EDGE_THRESHOLD_MIN = DEFAULT_EDGE_THRESHOLD_MIN;
					EDGE_THRESHOLD_MAX = DEFAULT_EDGE_THRESHOLD_MAX;
					ITERATIONS = DEFAULT_ITERATIONS;
					SUBPIXEL_QUALITY = DEFAULT_SUBPIXEL_QUALITY;

					fxaaShader.use();
					fxaaShader.set1f("EDGE_THRESHOLD_MIN", EDGE_THRESHOLD_MIN);
					fxaaShader.set1f("EDGE_THRESHOLD_MAX", EDGE_THRESHOLD_MAX);
					fxaaShader.set1i("ITERATIONS", ITERATIONS);
					fxaaShader.set1f("SUBPIXEL_QUALITY", SUBPIXEL_QUALITY);

					fxaaRevert = false;
				}
			}
			ImGui::EndDisabled();

			ImGui::NewLine();

			ImGui::Text("Tonemapping");
			std::vector<const char*> tonemapModes = { "None", "Reinhard", "Extended Reinhard", "Uncharted2", "Hill ACES", "Narkowicz ACES", "Manual Exposure" };

			if (ImGui::Combo("Tonemap Mode", &tonemapMode, tonemapModes.data(), tonemapModes.size())) {
				postprocShader.use();
				postprocShader.set1i("tonemapMode", tonemapMode);
			}
			ImGui::NewLine();

			if (ImGui::SliderFloat("Maximum Radiance", &maxRadiance, 0, 10)) {
				postprocShader.use();
				postprocShader.set1f("maxRadiance", maxRadiance);
			}
			if (ImGui::SliderFloat("Manual Exposure", &exposure, 0, 10)) {
				postprocShader.use();
				postprocShader.set1f("exposure", exposure);
			}
			

			ImGui::Text("Gamma Correction");
			if (ImGui::Checkbox("Use Gamma Correction", &gammaOn)) {
				postprocShader.use();
				postprocShader.set1b("gammaOn", gammaOn);
			}

			ImGui::BeginDisabled(!gammaOn);
			if (ImGui::SliderFloat("Gamma", &gamma, 0.f, 10.f)) {
				postprocShader.use();
				postprocShader.set1f("gamma", gamma);
			}

			ImGui::EndDisabled();
			
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("CSM")) {
			if (ImGui::Checkbox("CSM Enabled", &csmEnabled)) {
				mainShader.use();
				mainShader.set1b("csmEnabled", csmEnabled);
			}
			ImGui::BeginDisabled(!csmEnabled);

			if (ImGui::InputInt("Shadow Resolution", (int*)&newShadowResolution, 0)) {
				if (newShadowResolution < 1) newShadowResolution = 1;
				if (newShadowResolution > 16384) newShadowResolution = 16384;
			}
			
			if (shadowResolution != newShadowResolution) {
				if (ImGui::Button("Apply")) {
					shadowResolution = newShadowResolution;
					setupScreenRelated();
				}
			}
			//ImGui::SliderInt("Cascades Count", &cascadeCount, 0, 16); //Cannot be changed as I need to change CSM.geom in order for it to work.
			ImGui::EndDisabled();
			ImGui::TreePop();
		}
	}
	if (ImGui::CollapsingHeader("Shaders")) {
		if (ImGui::Button("Reload")) {
			mainShader.loadShader("src/Shaders/main.vert", "src/Shaders/main.frag");

			mainShader.use();
			mainShader.set1f("farPlane", cam.farPlane);
			mainShader.set1b("csmEnabled", csmEnabled);
			mainShader.set1i("cascadeCount", cascadeCount);
			for (uint32_t i = 0; i < cascadeCount; i++) {
				mainShader.set1f("cascadePlaneDistances[" + std::to_string(i) + "]", shadowCascadeLevels[i]);
			}
			setupUBOs();

			dirLight.changed = true;
			pointLight.changed = true;
			spotLight.changed = true;

			dirLight.set("dirLight", mainShader);
			pointLight.set("pointLights", mainShader);
			spotLight.set("spotLight", mainShader);
		}
		ImGui::SameLine(); ImGui::Text("Main Shader");
		
		if (ImGui::Button("Reload##0")) {
			postprocShader.loadShader("src/Shaders/basicQuad.vert", "src/Shaders/postProc.frag");

			postprocShader.use();
			postprocShader.set1b("gammaOn", gammaOn);
			postprocShader.set1b("fxaaEnabled", fxaaEnabled);
			postprocShader.set1f("exposure", exposure);
			postprocShader.set1f("gamma", gamma);
			postprocShader.set1f("maxRadiance", maxRadiance);
			postprocShader.set1i("tonemapMode", tonemapMode);

			postprocShader.setVec2("inverseScreenSize", glm::vec2(1.f / scrWidth, 1.f / scrHeight));
			postprocShader.set1f("EDGE_THRESHOLD_MIN", EDGE_THRESHOLD_MIN);
			postprocShader.set1f("EDGE_THRESHOLD_MAX", EDGE_THRESHOLD_MAX);
			postprocShader.set1i("ITERATIONS", ITERATIONS);
			postprocShader.set1f("SUBPIXEL_QUALITY", SUBPIXEL_QUALITY);
		}
		ImGui::SameLine(); ImGui::Text("Post-processing Shader");
	}
	if (ImGui::CollapsingHeader("UI Options")) {
		ImGui::SliderFloat("BG Alpha", &bgAlpha, 0.f, 1.f);

		std::vector<const char*> styles = { "Classic", "Dark", "Light" };
		if (ImGui::Combo("Style", &currentStyle, styles.data(), styles.size())) {
			if (currentStyle == 0) ImGui::StyleColorsClassic();
			else if (currentStyle == 1) ImGui::StyleColorsDark();
			else if (currentStyle == 2) ImGui::StyleColorsLight();
		}
	}
	if (ImGui::CollapsingHeader("Profiling", ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::Text((std::string("Vertices drawn: ") + std::to_string(sceneEntity.verticesCount)).c_str());
		ImGui::Text((std::string("Faces drawn:    ") + std::to_string(sceneEntity.verticesCount / 3)).c_str());

		ImGui::NewLine();
			ImGui::Text(("ImGui Average framerate: " + std::to_string((int)io.Framerate) + " FPS").c_str());
			ImGui::Text(("ImGui Average frametime: " + std::to_string(1000 / io.Framerate) + " ms").c_str());
			ImGui::Text(("Average Delta Time:      " + std::to_string(Time::avgMsTime) + " ms").c_str());
			ImGui::NewLine();

			ImGui::Text("GPU Times [Queries]");
			double gpuFrametime = (depthPassQuery.getResult() + renderPassQuery.getResult() + guiPassQuery.getResult() + postprocQuery.getResult() + shadowPassQuery.getResult() + fxaaPassQuery.getResult()) / 1000000.0;
			//Text(("Approx. CPU time: " + std::to_string(1000 / io.Framerate - gpuFrametime) + " ms").c_str());
			ImGui::Text(("GPU time: " + std::to_string(gpuFrametime) + " ms").c_str());
			ImGui::NewLine();

			ImGui::Text(("CSM Pass:       " + std::to_string(shadowPassQuery.getResult() / 1000000.0) + "  ms").c_str());
			ImGui::Text(("Depth Pass:     " + std::to_string(depthPassQuery.getResult() / 1000000.0) + "  ms").c_str());
			ImGui::Text(("Render Pass:    " + std::to_string(renderPassQuery.getResult() / 1000000.0) + "  ms").c_str());
			ImGui::Text(("GUI Pass:       " + std::to_string(guiPassQuery.getResult() / 1000000.0) + "  ms").c_str());
			ImGui::Text(("Post-Proc Pass: " + std::to_string(postprocQuery.getResult() / 1000000.0) + "  ms").c_str());
			ImGui::Text(("FXAA Pass:      " + std::to_string(fxaaPassQuery.getResult() / 1000000.0) + "  ms").c_str());
		}
	if (ImGui::CollapsingHeader("Software & Hardware Info")) {
		ImGui::Text(("OpenGL Version: " + openGLVersion).c_str());
		ImGui::Text(("GLSL Version: " + glslVersion).c_str());
		ImGui::Text(("Vendor: " + vendor).c_str());
		ImGui::Text(("GPU Version: " + gpuVersion).c_str());
	}
	if (ImGui::CollapsingHeader("OpenGL Options")) {
		if (ImGui::Checkbox("VSync", &vsyncOn)) {
			glfwSwapInterval(vsyncOn);
		}
		if (ImGui::Checkbox("MSAA", &msaaEnabled)) {
			if (msaaEnabled) glEnable(GL_MULTISAMPLE);
			else glDisable(GL_MULTISAMPLE);
		} ImGui::SameLine();
		if (ImGui::SliderInt("##0", &msaaSamples, 1, msaaMaxSamples)) {
			glfwWindowHint(GLFW_SAMPLES, msaaSamples);
		}
	}
	if (ImGui::CollapsingHeader("Debugging", ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::Text((std::string("Cam Pos: ") + "X: " + std::format("{:.3f}", cam.pos.x) + ", Y: " + std::format("{:.3f}", cam.pos.y) + ", Z: " + std::format("{:.3f}", cam.pos.z)).c_str());
		ImGui::SameLine(); if (ImGui::SmallButton("Copy##0")) {
			ImGui::SetClipboardText((std::format("{:.3f}", cam.pos.x) + ", " + std::format("{:.3f}", cam.pos.y) + ", " + std::format("{:.3f}", cam.pos.z)).c_str());
		}
		ImGui::Text((std::string("Cam Rot: ") + "Pitch: " + std::to_string(cam.pitch) + ", Yaw: " + std::to_string(cam.yaw)).c_str());

		ImGui::NewLine();
		if (ImGui::Checkbox("Cascade Debug View", &cascadeDebugView)) {
			mainShader.use();
			mainShader.set1b("cascadeDebugView", cascadeDebugView);
		}
		if (ImGui::Checkbox("Freeze CSM", &freezeCSM)) {
			mainShader.use();
			mainShader.set1b("freezeCSM", freezeCSM);
			mainShader.setVec3("oldViewPos", cam.pos);
		}
	}

	ImGui::End();
	//ImGui::ShowMetricsWindow();
	//ImGui::ShowDemoWindow();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
	void cleanup() {
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}
}