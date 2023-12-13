#include "../pch.h"
#include "Renderer.hpp"

void Renderer::initDependencies() {
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
	//if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) { //Normal GLAD
		//if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress)) { //GLAD2
	//	mLog("Failed to initialize GLAD!", Log::LogError);
	//}

	GLenum err = glewInit();
	if (err != GLEW_OK)
		nLog(std::string("Glew Error: ") + (const char*)glewGetErrorString(GLEW_OK), Log::LogError, "");

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 450");
}
void Renderer::init() {
	//Software and Hardware Info
	openGLVersion = (char*)glGetString(GL_VERSION);
	glslVersion = (char*)glGetString(GL_SHADING_LANGUAGE_VERSION);
	vendor = (char*)glGetString(GL_VENDOR);
	gpuVersion = (char*)glGetString(GL_RENDERER);

	//Configure GL options
	glfwSwapInterval(vsyncOn);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_DEPTH_TEST);

	glfwWindowHint(GLFW_SAMPLES, msaaSamples);
	if (msaaEnabled) glEnable(GL_MULTISAMPLE);
	glGetIntegerv(GL_MAX_INTEGER_SAMPLES, &msaaMaxSamples);

	//Face cull
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	if (wireframeMode) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	if (mouseLocked) glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//Background Color
	glClearColor(0.5f, 0.5f, 0.5f, 1.f);

	//Setup Queries
	depthPassQuery.loadQuery(GL_TIME_ELAPSED);
	renderPassQuery.loadQuery(GL_TIME_ELAPSED);
	postprocQuery.loadQuery(GL_TIME_ELAPSED);
	guiPassQuery.loadQuery(GL_TIME_ELAPSED);
	shadowPassQuery.loadQuery(GL_TIME_ELAPSED);
	fxaaPassQuery.loadQuery(GL_TIME_ELAPSED);

	//ImGui

	//Style
	if (currentStyle == 0) ImGui::StyleColorsClassic();
	else if (currentStyle == 1) ImGui::StyleColorsDark();
	else if (currentStyle == 2) ImGui::StyleColorsLight();

	style = &ImGui::GetStyle();
	windowAlpha = style->Alpha;

	style->Alpha = offAlpha;

	//Render quad
	quad.create(quadVerts);

	//Shaders
	mainShader.loadShader("src/Shaders/main.vert", "src/Shaders/main.frag");
	//lightBoxShader.loadShader("src/Shaders/lightBox.vert", "src/Shaders/lightBox.frag");
	renderQuadShader.loadShader("src/Shaders/basicQuad.vert", "src/Shaders/renderQuad.frag");
	postprocShader.loadShader("src/Shaders/basicQuad.vert", "src/Shaders/postproc.frag");
	depthPassShader.loadShader("src/Shaders/basicDraw.vert", "src/Shaders/depthPrePass.frag");
	deferredShader.loadShader("src/Shaders/main.vert", "src/Shaders/deferred.frag");
	fxaaShader.loadShader("src/Shaders/basicQuad.vert", "src/Shaders/fxaa.frag");
	CSMShader.loadShader("src/Shaders/CSM.vert", "src/Shaders/CSM.frag", "src/Shaders/CSM.geom");

	shadowCascadeLevels.clear();
	float lambda = 0.9f;// .8f;
	float virtualNearPlane = 1.f;// nearPlane;// 1.f;
	for (uint32_t i = 1; i < cascadeCount; i++) { //Practical view frustum split (combination between uniform and logarithmic)
		float k = (float)i / (float)cascadeCount;
		float f = lambda * (virtualNearPlane * powf(cam->farPlane / virtualNearPlane, k)) + (1.f - lambda) * (virtualNearPlane + (cam->farPlane - virtualNearPlane) * k);
		shadowCascadeLevels.push_back(f);
	}
	shadowCascadeLevels.push_back(cam->farPlane);

	//Ge cascadePlaneDistances
	mainShader.use();
	for (uint32_t i = 0; i < cascadeCount; i++) {
		mainShader.set1f("cascadePlaneDistances[" + std::to_string(i) + "]", shadowCascadeLevels[i]);
	}

	//Set mandatory uniform values
	mainShader.use();
	mainShader.set1f("specularExponent", 32.f);
	mainShader.set1f("farPlane", cam->farPlane);
	mainShader.set1b("csmEnabled", csmEnabled);
	mainShader.set1i("cascadeCount", cascadeCount);

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

	setupFramebuffers();
	setupUBOs();
}
void Renderer::setupFramebuffers() {
	postprocFB.create2D(GL_TEXTURE_2D, scrWidth, scrHeight, GL_RGBA16F);
	fxaaFB.create2D(GL_TEXTURE_2D, scrWidth, scrHeight, GL_RGBA, GL_RGBA, NULL);

	shadowFB.createLayered2D(GL_TEXTURE_2D_ARRAY, shadowResolution, shadowResolution, cascadeCount, GL_DEPTH_COMPONENT16, GL_DEPTH_COMPONENT, NULL, GL_DEPTH_ATTACHMENT, GL_FLOAT);
	shadowFB.texture.setBorderColor({ 1.f, 1.f, 1.f, 1.f });
	shadowFB.texture.setFilterMin(GL_NEAREST);
	shadowFB.texture.setFilterMag(GL_NEAREST);
	shadowFB.texture.setWrapS(GL_CLAMP_TO_BORDER);
	shadowFB.texture.setWrapT(GL_CLAMP_TO_BORDER);
}
void Renderer::setupUBOs() {
	if (!uboMatrices) {
		glGenBuffers(1, &uboMatrices);

		glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
		glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
		glBindBufferBase(GL_UNIFORM_BUFFER, 0, uboMatrices);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	//Set proj
	glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(cam->proj));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	if (!lightMatricesUBO) {
		glGenBuffers(1, &lightMatricesUBO);

		glBindBuffer(GL_UNIFORM_BUFFER, lightMatricesUBO);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) * 16, NULL, GL_STATIC_DRAW);
		glBindBufferBase(GL_UNIFORM_BUFFER, 1, lightMatricesUBO);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

	}
}
void Renderer::render() {
	//CSM Pass
	if (!freezeCSM) {
		const auto lightMatrices = getLightSpaceMatrices();
		glBindBuffer(GL_UNIFORM_BUFFER, lightMatricesUBO);

		for (uint32_t i = 0; i < lightMatrices.size(); i++) {
			glBufferSubData(GL_UNIFORM_BUFFER, i * sizeof(glm::mat4), sizeof(glm::mat4), &lightMatrices[i]);
		}
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	//Set lights
	dirLight->set("dirLight", mainShader);
	for (auto pointLight : pointLights) {
		pointLight->set("pointLight", mainShader);
	}
	for (auto spotLight : spotLights) {
		spotLight->set("spotLight", mainShader);
	}

	glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(cam->view));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	if (wireframeMode) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glDepthFunc(GL_LEQUAL);

	//renderQuadShader.use();
	//renderQuadShader.set1i("layer", 0);
	//shadowFB.texture.bind(0);
	//glDepthFunc(GL_LEQUAL);
	//quad.draw();
	//glDepthFunc(GL_LESS);

	//Shadow Pass
	shadowPassQuery.begin();

	if (csmEnabled) {
		glDepthFunc(GL_LESS);
		glViewport(0, 0, shadowResolution, shadowResolution);

		glCullFace(GL_FRONT);
		glEnable(GL_DEPTH_CLAMP); //From a comment by https://disqus.com/by/disqus_XCUOEk9iLH/? on disqus.

		shadowFB.bind();
		shadowFB.clear();

		for (auto entity : entities) {
			entity->draw(CSMShader);
		}
		shadowFB.unbind();

		glDisable(GL_DEPTH_CLAMP);
		glCullFace(GL_BACK);

		glViewport(0, 0, scrWidth, scrHeight);
		glDepthFunc(GL_LEQUAL);
	}
	shadowPassQuery.end();

	//Render Pass
	postprocFB.bind();
	postprocFB.clear();

	//Depth pre-pass
	depthPassQuery.begin();

	for (auto entity : entities) {
		entity->draw(depthPassShader);
	}
	depthPassQuery.end();

	//Draw Pass
	renderPassQuery.begin();
	shadowFB.texture.bind(4);

	for (auto entity : entities) {
		entity->draw(mainShader);
	}
	renderPassQuery.end();

	postprocFB.unbind();

	//Reset glPolygonMode so you can draw the render quad normally
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	//Post-processing pass
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

	//FXAA Pass
	fxaaPassQuery.begin();
	fxaaShader.use();
	fxaaFB.texture.bind(0);
	glDepthFunc(GL_LEQUAL);

	quad.draw();

	glDepthFunc(GL_LESS);
	fxaaPassQuery.end();

	//GUI Pass
	guiPassQuery.begin();
	updateGUI();
	guiPassQuery.end();

	//Display Frame
	glfwPollEvents();
	glfwSwapBuffers(window);
}

void Renderer::updateGUI() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

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
			ImGui::NewLine();

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
					setupFramebuffers();
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
			mainShader.set1f("farPlane", cam->farPlane);
			mainShader.set1b("csmEnabled", csmEnabled);
			mainShader.set1i("cascadeCount", cascadeCount);
			for (uint32_t i = 0; i < cascadeCount; i++) {
				mainShader.set1f("cascadePlaneDistances[" + std::to_string(i) + "]", shadowCascadeLevels[i]);
			}
			setupUBOs();

			dirLight->changed = true;
			dirLight->set("dirLight", mainShader);
			for (auto pointLight : pointLights) {
				pointLight->changed = true;
				pointLight->set("pointLights", mainShader);
			}
			for (auto spotLight : spotLights) {
				spotLight->changed = true;
				spotLight->set("spotLight", mainShader);
			}
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
		uint32_t verticesCount = 0;
		uint32_t indicesCount = 0;

		for (auto entity : entities) {
			verticesCount += entity->verticesCount;
			indicesCount += entity->indicesCount;
		}

		ImGui::Text((std::string("Vertices drawn: ") + std::to_string(verticesCount)).c_str());
		ImGui::Text((std::string("Faces drawn:    ") + std::to_string(indicesCount / 3)).c_str());

		ImGui::NewLine();
		ImGui::Text(("ImGui Average framerate: " + std::to_string((int)ImGui::GetIO().Framerate) + " FPS").c_str());
		ImGui::Text(("ImGui Average frametime: " + std::to_string(1000 / ImGui::GetIO().Framerate) + " ms").c_str());
		ImGui::Text(("Average Delta Time:      " + std::to_string(Time::avgMsTime) + " ms").c_str());
		ImGui::NewLine();

		ImGui::Text("GPU Times [Queries]");
		double gpuFrametime = (depthPassQuery.getResult() + renderPassQuery.getResult() + guiPassQuery.getResult() + postprocQuery.getResult() + shadowPassQuery.getResult() + fxaaPassQuery.getResult()) / 1000000.0;
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

		if (ImGui::Checkbox("Draw wireframe", &wireframeMode)) {
			if (wireframeMode) {
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			}
			else {
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			}
		}
	}
	if (ImGui::CollapsingHeader("Debugging", ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::Text((std::string("Cam Pos: ") + "X: " + std::format("{:.3f}", cam->pos.x) + ", Y: " + std::format("{:.3f}", cam->pos.y) + ", Z: " + std::format("{:.3f}", cam->pos.z)).c_str());
		ImGui::SameLine(); if (ImGui::SmallButton("Copy##0")) {
			ImGui::SetClipboardText((std::format("{:.3f}", cam->pos.x) + ", " + std::format("{:.3f}", cam->pos.y) + ", " + std::format("{:.3f}", cam->pos.z)).c_str());
		}
		ImGui::Text((std::string("Cam Rot: ") + "Pitch: " + std::to_string(cam->pitch) + ", Yaw: " + std::to_string(cam->yaw)).c_str());

		ImGui::NewLine();
		if (ImGui::Checkbox("Cascade Debug View", &cascadeDebugView)) {
			mainShader.use();
			mainShader.set1b("cascadeDebugView", cascadeDebugView);
		}
		if (ImGui::Checkbox("Freeze CSM", &freezeCSM)) {
			mainShader.use();
			mainShader.set1b("freezeCSM", freezeCSM);
			mainShader.setVec3("oldViewPos", cam->pos);
		}
	}

	ImGui::End();
	//ImGui::ShowMetricsWindow();
	//ImGui::ShowDemoWindow();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
void Renderer::cleanupGUI() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

//CSM
std::vector<glm::vec4> Renderer::getFrustumCornersWorldSpace(const glm::mat4& proj, const glm::mat4& view) {
	const auto inv = glm::inverse(proj * view);

	std::vector<glm::vec4> frustumCorners;
	for (uint32_t x = 0; x < 2; x++) {
		for (uint32_t y = 0; y < 2; y++) {
			for (uint32_t z = 0; z < 2; z++) {
				const glm::vec4 pt = inv * glm::vec4(2.0f * x - 1.0f, 2.0f * y - 1.0f, 2.0f * z - 1.0f, 1.0f);
				frustumCorners.push_back(pt / pt.w);
			}
		}
	}

	return frustumCorners;
}
glm::mat4 Renderer::getLightSpaceMatrix(const float nearPlane, const float farPlane) {
	const auto proj = glm::perspective(glm::radians(cam->fov), (float)scrWidth / scrHeight, nearPlane, farPlane);

	auto corners = getFrustumCornersWorldSpace(proj, cam->view);
	glm::vec3 center = glm::vec3(0, 0, 0);
	for (const auto& v : corners) {
		center += glm::vec3(v);
	}
	center /= corners.size();

	const auto lightView = glm::lookAt(center - dirLight->getDir(), center, glm::vec3(0.0f, 1.0f, 0.0f));


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
std::vector<glm::mat4> Renderer::getLightSpaceMatrices() {
	std::vector<glm::mat4> ret;

	for (uint32_t i = 0; i < cascadeCount; i++) {
		if (i == 0) {
			ret.push_back(getLightSpaceMatrix(cam->nearPlane, shadowCascadeLevels[i]));
		}
		else {
			ret.push_back(getLightSpaceMatrix(shadowCascadeLevels[i - 1], shadowCascadeLevels[i]));
		}
	}

	return ret;
}