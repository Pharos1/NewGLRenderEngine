#pragma once

#include <iostream>
#include <algorithm>
#include <functional>
#include <memory>
#include <thread>
#include <utility>
#include <optional>
#include <ostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <stdlib.h>
#include <future>
#include <mutex>
#include <format>

//Data structures
#include <string>
#include <stack>
#include <deque>
#include <array>
#include <array>
#include <vector>
#include <set>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <variant>

//Windows API
#include <Windows.h>

//OpenGL Dependencies
#include <GLEW/glew.h> //Needs a preprocessor GLEW_STATIC in the project settings
//#include <GLAD/gl.h>
#include <GLFW/glfw3.h>
#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <GLM/gtc/quaternion.hpp>
#include <GLM/gtx/quaternion.hpp>
#include <GLM/ext/quaternion_common.hpp>
#include <SOIL2/stb_image.h>
#include <SOIL2/SOIL2.h>
#include <ASSIMP/Importer.hpp>
#include <ASSIMP/scene.h>
#include <ASSIMP/postprocess.h>
#include "../Dependencies/include/imgui/imgui.h"
#include "../Dependencies/include/imgui/imgui_impl_glfw.h"
#include "../Dependencies/include/imgui/imgui_impl_opengl3.h"