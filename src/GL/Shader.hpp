#pragma once
#include "../pch.h"

//Note: Shaders remove inactive uniforms i.e. uniforms that don't contribute to the final result.
class Shader {
private:
	uint32_t id = 0;

	void checkCompileErrors(GLuint shader, std::string type);

public:
	void loadShader(const std::string& vPath, const std::string& fPath, const std::string& gPath = "");
	Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr);
	Shader() = default;
	~Shader();

	Shader(const Shader&) = delete;
	Shader& operator=(const Shader&) = delete;

	Shader(Shader&& other) noexcept;
	Shader& operator=(Shader&& other) noexcept;

	int getID() const;
	void use() const;
	void unuse() const;
	void deleteProgram();

	void set1b(const std::string& name, const bool& value) const;
	void set1i(const std::string& name, const int& value, const int count = 1) const;
	void set1ui(const std::string& name, const uint32_t& value, const int count = 1) const;
	void set1f(const std::string& name, const float& value, const int count = 1) const;
	void setMat3(const std::string& name, const glm::mat3& mat, const int count = 1) const;
	void setMat4(const std::string& name, const glm::mat4& mat, const int count = 1) const;
	void setVec2(const std::string& name, const glm::vec2& vec, const int count = 1) const;
	void setVec3(const std::string& name, const glm::vec3& vec, const int count = 1) const;
	void setVec4(const std::string& name, const glm::vec4& vec, const int count = 1) const;
};