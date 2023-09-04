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
* CSM With Regular Shadow Mapping

## TODOs
* Frustum Culling with scene graph
* Skyboxes. Cubemap and HDRI.
* Sub-pixel shadow mapping with CSM. <!--Here I'd like to save [this](https://disqus.com/by/disqus_XCUOEk9iLH/?) comment on LearnOpenGL) --> Or it's fast counterpart <!--http://www.kunzhou.net/2009/subpixel_shadow.pdf-->
* Quaternions for inner calculations. I want to keep Euler Angles for rotations through the GUI.
* Infinite Grid. [This](http://asliceofrendering.com/scene%20helper/2020/01/05/InfiniteGrid/) is a helpful blog post
* ECS
* Cleanup the setupApplication func
* Make camera class hold the projMat, fov, aspect ratio, zFar, zNear.
* IBL like [this](https://www.youtube.com/watch?v=qbDrqARX07o&t=1131s)
* Optimization
* CSM doesn't support transparent textures currently. Easy fix is using depthPrePass.frag and having CSM.vert output texCoord too.
* Area Lights
* CSM Freeze Option
* CSM Visualization
* Meshes should have vao vbo ebo destruction in the destructor
* Interpolate better between cascades <!--https://learn.microsoft.com/en-us/windows/win32/dxtecharts/cascaded-shadow-maps-->
* Improve CSM Quality <!--https://learn.microsoft.com/en-us/windows/win32/dxtecharts/common-techniques-to-improve-shadow-depth-maps-->

## Todo for things I am not sure about. Probably do a lot of them when I buy an ACTUAL GPU THAT IS NOT AN iGPU.
* Deferred Rendering (it is also good match with SSAO and light volumes). Also remove the occlusion culling when implemented.
* Post-Processing
	* SSAO
* Light volumes with deferred
* MSAA. Note: Currently doesn't work because the depth fbo should have multi sample color and depth attachments. Or maybe I could resolve the attachments somehow.
* Application wrapper. Note: I have a branch with it, doesn't look better and callbacks have to be static.
* Make vao, vbo and ebo raii wrappers and remove mesh's destructor, copy constructor and move operator. Note: Buffers have a lot of functions associated with them and they are mostly one liners. Seems useless to make a wrapper.

## Problems
* Because we use index drawing, the normals get interpolated between edges of a triangle which is not very pleasant and at angles makes rendering seem very wrong.
* For some reason the gpu takes like a ms less than the cpu. Maybe use my wacky technique, first draw then do the math.
* I think FXAA should be after all postproc effects.

## Non-problematic problems. More like, things that can be implemented cleaner.
* Texture reuse optimization for models.

## Ideas for an actual name for the engine
* PenumbraEngine. First I have to implement shadows though (: Edit: They are implemented but they are hard  shadows, nvm.

<!---
## Passes (I have to minimize them, and there will be a lot more)
* Depth Pre-pass
* PBR Pass
* Gradient Skybox Pass (It is ugly but better than nothing)
* Post-Processing pass (all previous passes were rendered on an HDR framebuffer)
-->