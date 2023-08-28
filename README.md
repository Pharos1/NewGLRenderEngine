<!--- https://github.com/adam-p/markdown-here/wiki/Markdown-Cheatsheet#headers -->
# NewGLRenderEngine (Cool name, right?)

## Features
* Metallic Workflow PBR with Cook-Tolerance BRDF
* Occlusion Culling with Depth Pre-pass (Early-Z Rejection)
* Directional, Omnidirectional and Spot Lights
* Model Loading with ASSIMP
* Texture loading with STBI
* Post-Processing (From HDR)
	* FXAA (In a separate pass after other post-processing effects, LDR and sRGB color space)
	* Tonemapping
	* Gamma Correction
* Scene Graph

## Harder TODOs
* Frustum Culling with scene graph
* Skyboxes. Cubemap and HDRI.
* CSM <!--Here I'd like to save [this](https://disqus.com/by/disqus_XCUOEk9iLH/?) comment on LearnOpenGL) -->
* Texture reuse optimization for models.
* Quaternions
* Infinite Grid. [This](http://asliceofrendering.com/scene%20helper/2020/01/05/InfiniteGrid/) is a helpful blog post
* ECS (Not sure)
* Make vao, vbo and ebo raii wrappers and remove mesh's destructor, copy constructor and move operator.
* Cleanup the setupApplication func

## Todo for things I am not sure about
* Check Occlusion Query feature and use it instead for Occlusion Culling
* Maybe I can remove early-z rejection as deferred will be sufficient enough
* Deferred Rendering (it is also good match with SSAO and light volumes)
* Post-Processing
	* SSAO
* Light volumes with deferred
* MSAA. Note: Currently doesn't work because the depth fbo should have multi sample color and depth attachments. Or maybe I could resolve the attachments somehow.
* Application wrapper. Note: I have a branch with it, doesn't look better and callbacks have to be static.

## Problems
* Because we use index drawing, the normals get interpolated between edges of a triangle which is not very pleasant and at angles makes rendering seem very wrong.
* For some reason the gpu takes like a ms less than the cpu. Maybe use my wacky technique, first draw then do the math.
* I think FXAA should be after all postproc effects.

## Ideas for an actual name for the engine
* PenumbraEngine (first I have to implement shadows though)

<!---
## Passes (I have to minimize them, and there will be a lot more)
* Depth Pre-pass
* PBR Pass
* Gradient Skybox Pass (It is ugly but better than nothing)
* Post-Processing pass (all previous passes were rendered on an HDR framebuffer)
-->