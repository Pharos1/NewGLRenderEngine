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
* CSM

## TODOs
* Frustum Culling with scene graph
* Skyboxes. Cubemap and HDRI.
* Sub-pixel shadow mapping with CSM. <!--Here I'd like to save [this](https://disqus.com/by/disqus_XCUOEk9iLH/?) comment on LearnOpenGL) --> Or it's fast counterpart <!--http://www.kunzhou.net/2009/subpixel_shadow.pdf-->
* Quaternions for inner calculations. I want to keep Euler Angles for rotations through the GUI.
* Infinite Grid. [This](http://asliceofrendering.com/scene%20helper/2020/01/05/InfiniteGrid/) is a helpful blog post
* Cleanup the setupApplication func
* IBL like [this](https://www.youtube.com/watch?v=qbDrqARX07o&t=1131s)
* Optimization
* Area Lights
* Meshes should have vao vbo ebo destruction in the destructor
<!--* Interpolate better between cascades <!--https://learn.microsoft.com/en-us/windows/win32/dxtecharts/cascaded-shadow-maps-->
<!--* Improve CSM Quality <!--https://learn.microsoft.com/en-us/windows/win32/dxtecharts/common-techniques-to-improve-shadow-depth-maps-->
* Maybe Eye Adaptation
* Make a debug option to display the shadow maps maybe?? Would be a problem if I can change the number of cascades!
* ECS

## Todo for things I am not sure about. Probably do a lot of them when I buy an ACTUAL GPU THAT IS NOT AN iGPU.
* Deferred Rendering (it is also good match with SSAO and light volumes). Also remove the occlusion culling when implemented.
* Post-Processing
	* SSAO
* Light volumes with deferred
* MSAA. Note: Currently doesn't work because the depth fbo should have multi sample color and depth attachments. Or maybe I could resolve the attachments somehow.
* Application wrapper. Note: I have a branch with it, doesn't look better and callbacks have to be static.
* Make vao, vbo and ebo raii wrappers and remove mesh's destructor, copy constructor and move operator. Note: Buffers have a lot of functions associated with them and they are mostly one liners. Seems useless to make a wrapper.
* Fix faces count as it is not verts/3 but indices/3
* Generate simpler meshes for LOD using some mesh vertex thingy
* Start using docking branch of ImGui when I implement ECS
* Gizmos with ImGui

### Implementation of Renderer class and ECS. Now the renderer will become an engine.
* Make lights be an Entity with a light component
* Make class Entity pretty much empty and use a transform component for objects, which:
	* Would update the child transform component
* Renderer would have vector of Entities that would be rendered every frame
* Have a fixed render() method inside the Renderer class that would render every entity from Entity vector based on the predifined fixed pipeline of the engine
	* Would have to respond to booleans like "freezeCSM", "enableCSM", etc. Renderer class would hold all those variables, they would be public, aside from Entities vector and others.
	* Would have to call a function, e.g. onUpdate, that would update the state of the renderer/color buffer based on the component that the entity has, if any. For example, mesh component would draw and light component would set light. Maybe onUpdate will be part of Entity class and it will call onUpdate of every component.
* For ImGui
	* ImGui code will be inside the renderer
	* Make components have functions that would draw themselves with ImGui
	* Make components have function that would display warnings on the same line of the component or an console like in Unity
	* Make functions that would 
	* Make component add, copy and delete
* Have to think how model class would work as I would have Material Component and Mesh Component be seperate

### Entity inspector
* Like in Unity, have a disable option

### Components to add:
* Transform
* Mesh
	* If there isn't a Material component then draw a NULL Texture, pink and black squares
	* Have a display option
* Material
	* Display a warning if there is no mesh/The object is never drawn
* Light
	* Have all that is needed to represent any light
	* Have an enum that specifies what light it is
	* Have spherical/3D coordinates representing the direction of the light
	* Have light color
* ShadowCaster
	* Automatically add Light component if it isn't already added to the Entity
	* Give error if the Light component has light type set to anything other than directional light

## Problems
* Because we use index drawing, the normals get interpolated between edges of a triangle which is not very pleasant and at angles makes rendering seem very wrong.
* For some reason the gpu takes like a ms less than the cpu. Maybe use my wacky technique, first draw then do the math.
* When I discard pixels if alpha is less than 0.5f the sponza transparent leaves textures don't work correctly. Only works when using < 0.05f
* The only problem that I am faced with making an Application wrapper instead of writing everything inside main.cpp is that callback funcs cannot be in a class, nor can they be static while accessing object's variables.

* SOme problems with exporting the app, everytime you run it the map is being loaded differently ) :. More like it is rendered strangely but the shadows are normal. Problem only when using release

## Non-problematic problems. More like, things that can be implemented cleaner.
* Texture reuse optimization for models.

## Ideas for an actual name for the engine
* PenumbraEngine. First I have to implement shadows though (: Edit: They are implemented but they are hard shadows, nvm.

<!---
## Passes (I have to minimize them, and there will be a lot more)
* Depth Pre-pass
* PBR Pass
* Gradient Skybox Pass (It is ugly but better than nothing)
* Post-Processing pass (all previous passes were rendered on an HDR framebuffer)
-->