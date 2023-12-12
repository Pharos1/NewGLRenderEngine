#pragma once
#include "../pch.h"

#include "../GL/Texture.hpp"
#include "../GL/Shader.hpp"
#include "../Object/Vertex.hpp"
#include "../Object/Mesh.hpp"
#include "../Object/Model.hpp"
#include "../Object/Entity.hpp"
#include "../Renderer/Camera.hpp"
#include "../Renderer/Light.hpp"
#include "../GL/Framebuffer.hpp"
#include "../Utilities/Logger.hpp"
#include "../Utilities/Time.hpp"
#include "../Utilities/Query.hpp"

class Renderer {
public: //Must be private but for debuging purposes
	std::vector<Vertex> quadVerts = {
		{{-1.f,-1.f, 0.f}, { 0.f, 0.f, 1.f}, {0.f, 0.f}},
		{{ 1.f,-1.f, 0.f}, { 0.f, 0.f, 1.f}, {1.f, 0.f}},
		{{ 1.f, 1.f, 0.f}, { 0.f, 0.f, 1.f}, {1.f, 1.f}},
		{{-1.f,-1.f, 0.f}, { 0.f, 0.f, 1.f}, {0.f, 0.f}},
		{{ 1.f, 1.f, 0.f}, { 0.f, 0.f, 1.f}, {1.f, 1.f}},
		{{-1.f, 1.f, 0.f}, { 0.f, 0.f, 1.f}, {0.f, 1.f}},
	};

	Mesh quad;

	//OpenGL Options
	bool vsyncOn = false;
	int msaaSamples = 4;
	bool msaaEnabled = true;
	int msaaMaxSamples = 0;

	//Queries
	Query depthPassQuery;
	Query renderPassQuery;
	Query postprocQuery;
	Query guiPassQuery;
	Query shadowPassQuery;
	Query fxaaPassQuery;

	//Framebuffers
	Framebuffer postprocFB;
	Framebuffer fxaaFB;
	Framebuffer shadowFB;

	//UBOs
	GLuint uboMatrices;
	GLuint lightMatricesUBO;

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

	//Software and Hardwere Info
	std::string openGLVersion;
	std::string glslVersion;
	std::string vendor;
	std::string gpuVersion;

	//Post-processing Options
	float exposure = 2.2f;
	float gamma = 2.2f;
	bool gammaOn = true;
	int tonemapMode = 5;
	float maxRadiance = 1.f;

public: //Have to make some private
	//Application Specific
	//UI
	ImGuiIO io;
	ImGuiStyle* style;
	float windowAlpha;
	bool mouseLocked = true;

	//UI Options
	int currentStyle = 0; //0 - Classic, 1 - Dark, 2 - Light
	float bgAlpha = .8f;
	float offAlpha = .3f;

	uint32_t scrWidth = 1280;
	uint32_t scrHeight = 920;
	GLFWwindow* window;

	//Shaders
	Shader mainShader;
	//Shader lightBoxShader;
	Shader renderQuadShader;
	Shader postprocShader;
	Shader depthPassShader;
	Shader deferredShader;
	Shader fxaaShader;
	Shader CSMShader;

	//CSM
	bool csmEnabled = true;
	uint8_t cascadeCount = 3;
	std::vector<float> shadowCascadeLevels; //{ farPlane / 50.0f, farPlane / 25.0f, farPlane / 10.0f, farPlane / 2.0f };
	#define DEFAULT_SHADOW_RESOLUTION 2048
	GLuint shadowResolution = DEFAULT_SHADOW_RESOLUTION;
	GLuint newShadowResolution = DEFAULT_SHADOW_RESOLUTION;
	bool cascadeDebugView = false;
	bool freezeCSM = false;

public: //Still there aren't any getters/setters as this is WIP
	DirLight* dirLight;
	std::vector<PointLight*> pointLights;
	std::vector<SpotLight*> spotLights;
	Camera* cam;
	std::vector<Entity*> entities;

	Renderer() = default;

	Renderer(const Renderer&) = delete;
	Renderer& operator=(const Renderer&) = delete;

	Renderer(Renderer&&) = delete;
	Renderer& operator=(Renderer&&) = delete;

	void initDependencies();
	void init();
	void setupFramebuffers();
	void setupUBOs();
	void render();

	void updateGUI();
	void cleanupGUI();

	void retrieveQueryResults();

	//CSM
	std::vector<glm::vec4> getFrustumCornersWorldSpace(const glm::mat4& proj, const glm::mat4& view);
	glm::mat4 getLightSpaceMatrix(const float nearPlane, const float farPlane);
	std::vector<glm::mat4> getLightSpaceMatrices();
};