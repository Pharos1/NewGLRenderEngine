<!--- https://github.com/adam-p/markdown-here/wiki/Markdown-Cheatsheet#headers -->
# NewGLRenderEngine (Cool name, right?)

## Features
* Metallic Workflow PBR with Cook-Tolerance BRDF
* Occlusion Culling with Depth Pre-pass (Early-Z Rejection)
* Directional, Omnidirectional and Spot Lights
* Model Loading with ASSIMP
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
	
### Todo for things I am not sure about
* Check Occlusion Query feature and use it instead for Occlusion Culling

<!---
## Passes (I have to minimize them, and there will be a lot more)
* Depth Pre-pass
* PBR Pass
* Gradient Skybox Pass (It is ugly but better than nothing)
* Post-Processing pass (all previous passes were rendered on an HDR framebuffer)
-->