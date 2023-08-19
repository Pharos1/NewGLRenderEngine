<!--- https://github.com/adam-p/markdown-here/wiki/Markdown-Cheatsheet#headers -->
# NewGLRenderEngine (Cool name, right?)

## Features
* Metallic Workflow PBR with Cook-Tolerance BRDF
* Occlusion Culling with Depth Pre-pass (Early-Z Rejection)
* Directional, Omnidirectional and Spot Lights
* Model Loading with ASSIMP
* Texture loading with STBI
* Post-Processing (HDR)
	* Tonemapping
	* Gamma Correction

## Harder TODOs
* Frustum Culling
* Skyboxes. Cubemap and HDRI.
* GUI (Dear ImGui)
* CSM (Here I'd like to save [this](https://disqus.com/by/disqus_XCUOEk9iLH/?) comment on LearnOpenGL)
* Texture reuse optimization for models.

## Simple TODOs
* Make normal timer and scoped timers different classes
* fix specular dots, seen when rendering sponza and writing only the spec value. I think the culprits are the roughness and metallic textures
* Make vao, vbo and ebo raii wrappers and remove mesh's destructor, copy constructor and move operator.

### Todo for things I am not sure about
* Check Occlusion Query feature and use it instead for Occlusion Culling
* Maybe I can remove early-z rejection as deferred will be sufficient enough
* Deferred Rendering (it is also good match with SSAO and light volumes)
* Post-Processing
	* SSAO
* Light volumes with deferred

<!---
## Passes (I have to minimize them, and there will be a lot more)
* Depth Pre-pass
* PBR Pass
* Gradient Skybox Pass (It is ugly but better than nothing)
* Post-Processing pass (all previous passes were rendered on an HDR framebuffer)
-->