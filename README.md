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

## Todo
* Frustum Culling
* Deferred Rendering (it is also good match with SSAO)
* Skyboxes
* GUI (Dear ImGui)
* CSM (Here I'd like to save [this](https://disqus.com/by/disqus_XCUOEk9iLH/?) comment on LearnOpenGL)
* Post-Processing
	* SSAO
* Texture reuse optimization for models.

## Simple TODOs
* Make texture class manage only the ogl resources, not the path and others.
* Make normal timer and scoped timers different classes
	
### Todo for things I am not sure about
* Check Occlusion Query feature and use it instead for Occlusion Culling
* Maybe I can remove early-z rejection as deferred will be sufficient enough

<!---
## Passes (I have to minimize them, and there will be a lot more)
* Depth Pre-pass
* PBR Pass
* Gradient Skybox Pass (It is ugly but better than nothing)
* Post-Processing pass (all previous passes were rendered on an HDR framebuffer)
-->