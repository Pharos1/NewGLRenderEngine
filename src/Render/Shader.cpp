#include "../pch.h"
#include "Shader.hpp"
#include "../Utilities/Logger.hpp"

void Shader::checkCompileErrors(GLuint shader, std::string type) {
		GLint success;
		GLchar infoLog[1024];
		if (type != "PROGRAM") {
			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
			if (!success) {
				glGetShaderInfoLog(shader, 1024, NULL, infoLog);
				nLog(std::string("Shader Compilation Failed. Type: ") + type + ". Info Log: " + infoLog, Log::LogError, "SHADER");
			}
		}
		else {
			glGetProgramiv(shader, GL_LINK_STATUS, &success);
			if (!success) {
				glGetProgramInfoLog(shader, 1024, NULL, infoLog);
				nLog(std::string("Program Linking Failed. Info Log: ") + infoLog, Log::LogError, "SHADER");
			}
		}
	}

void Shader::loadShader(const std::string& vPath, const std::string& fPath, const std::string& gPath) {
	std::string vContent;
	std::string fContent;
	std::string gContent;

	std::ifstream vShaderFile, fShaderFile, gShaderFile;
	std::stringstream vShaderStream, fShaderStream, gShaderStream;

	vShaderFile.open(vPath);
	fShaderFile.open(fPath);
	if (!gPath.empty()) gShaderFile.open(gPath);

	if (!vShaderFile.is_open())
		nLog(std::string("File '") + vPath + "' " + "not successfully opened!", Log::LogError, "SHADER");

	if (!fShaderFile.is_open())
		nLog(std::string("File '") + fPath + "' " + "not successfully opened!", Log::LogError, "SHADER");

	if (!gPath.empty()) {
		if (!gShaderFile.is_open())
			nLog(std::string("File '") + gPath + "' " + "not successfully opened!", Log::LogError, "SHADER");
	}


	vShaderStream << vShaderFile.rdbuf();
	fShaderStream << fShaderFile.rdbuf();
	gShaderStream << gShaderFile.rdbuf();

	vContent = vShaderStream.str();
	fContent = fShaderStream.str();
	gContent = gShaderStream.str();

	vShaderFile.close(); //Yes! I know about RAII, but ignore that, if you even saw it.
	fShaderFile.close();
	gShaderFile.close();

	const char* vShaderContent = vContent.c_str();
	const char* fShaderContent = fContent.c_str();
	const char* gShaderContent = gContent.c_str();

	//Now to the GL objects
	GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);
	GLuint geometry = (gPath.empty()) ? 0 : glCreateShader(GL_GEOMETRY_SHADER);

	int success;
	char infoLog[512];

	//Vertex Shader
	glShaderSource(vertex, 1, &vShaderContent, NULL);
	glCompileShader(vertex);
	checkCompileErrors(vertex, "VERTEX");

	//Fragment Shader
	glShaderSource(fragment, 1, &fShaderContent, NULL);
	glCompileShader(fragment);
	checkCompileErrors(fragment, "FRAGMENT");

	if (!gPath.empty()) {
		geometry = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(geometry, 1, &gShaderContent, NULL);
		glCompileShader(geometry);
		checkCompileErrors(geometry, "GEOMETRY");
	}

	//Define shader program
	if (!id) id = glCreateProgram();

	glAttachShader(id, vertex);
	glAttachShader(id, fragment);
	if (!gPath.empty()) glAttachShader(id, geometry);
	glLinkProgram(id);

	//Check for errors
	glGetProgramiv(id, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(id, 512, NULL, infoLog);
		mLog(std::string("Program linking failed! Reason: ") + infoLog, Log::LogError, "SHADER");
	}

	//Clear the shader after they are linked
	glDetachShader(id, vertex);
	glDeleteShader(vertex);

	glDetachShader(id, fragment);
	glDeleteShader(fragment);

	if (!gPath.empty()) {
		glDetachShader(id, geometry);
		glDeleteShader(geometry);
	}
}
Shader::Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath) {
	loadShader(vertexPath, fragmentPath, geometryPath);
};
Shader::~Shader() {
	mLog("~Shader has been triggered. Hint: Shader Program ID -> " + std::to_string(id), Log::LogDestructorInfo, "SHADER");
	deleteProgram();
}

Shader::Shader(Shader&& other) noexcept {
	id = std::exchange(other.id, 0);
}
Shader& Shader::operator=(Shader&& other) noexcept {
	if (this == &other) return *this;
	
	deleteProgram();

	id = std::exchange(other.id, 0);

	return *this;
}

int Shader::getID() const {
	return id;
};
void Shader::use() const {
	glUseProgram(id);
};
void Shader::unuse() const {
	glUseProgram(0);
};
void Shader::deleteProgram() {
	glDeleteProgram(id);
	this->id = 0;
};

void Shader::set1b(const std::string& name, bool value, const int count) const {
	int location = glGetUniformLocation(id, name.c_str());

	if (location != -1)
		glUniform1iv(location, count, (int*)&value);
	else
		return;// std::cout << "WARNING::SHADER.H::An active uniform corresponding to the name '" << name << "' was not found!\n";
}
void Shader::set1i(const std::string& name, int value, const int count) const {
	int location = glGetUniformLocation(id, name.c_str());

	if (location != -1)
		glUniform1iv(location, count, &value);
	else
		return; //std::cout << "WARNING::SHADER.H::An active uniform corresponding to the name '" << name << "' was not found!\n";
}
void Shader::set1ui(const std::string& name, uint32_t value, const int count) const {
	int location = glGetUniformLocation(id, name.c_str());

	if (location != -1)
		glUniform1uiv(location, count, &value);
	else
		return; //std::cout << "WARNING::SHADER.H::An active uniform corresponding to the name '" << name << "' was not found!\n";
}
void Shader::set1f(const std::string& name, float value, const int count) const {
	int location = glGetUniformLocation(id, name.c_str());

	if (location != -1)
		glUniform1fv(location, count, &value);
	else
		return;// std::cout << "WARNING::SHADER.H::An active uniform corresponding to the name '" << name << "' was not found!\n";
}
void Shader::setMat3(const std::string& name, const glm::mat3& mat, const int count) const {
	int location = glGetUniformLocation(id, name.c_str());

	if (location != -1)
		glUniformMatrix3fv(location, count, GL_FALSE, glm::value_ptr(mat));
	else
		return; //std::cout << "WARNING::SHADER.H::An active uniform corresponding to the name '" << name << "' was not found!\n";
}
void Shader::setMat4(const std::string& name, const glm::mat4& mat, const int count) const {
	int location = glGetUniformLocation(id, name.c_str());

	if (location != -1)
		glUniformMatrix4fv(location, count, GL_FALSE, glm::value_ptr(mat));
	else
		return; //std::cout << "WARNING::SHADER.H::An active uniform corresponding to the name '" << name << "' was not found!\n";
}
void Shader::setVec2(const std::string& name, const glm::vec2& vec, const int count) const {
	int location = glGetUniformLocation(id, name.c_str());

	if (location != -1)
		glUniform2fv(location, count, glm::value_ptr(vec));
	else
		return; //std::cout << "WARNING::SHADER.H::An active uniform corresponding to the name '" << name << "' was not found!\n";
}
void Shader::setVec3(const std::string& name, const glm::vec3& vec, const int count) const {
	int location = glGetUniformLocation(id, name.c_str());

	if (location != -1)
		glUniform3fv(location, count, glm::value_ptr(vec));
	else
		return; //std::cout << "WARNING::SHADER.H::An active uniform corresponding to the name '" << name << "' was not found!\n";
}
void Shader::setVec4(const std::string& name, const glm::vec4& vec, const int count) const {
	int location = glGetUniformLocation(id, name.c_str());

	if (location != -1)
		glUniform4fv(location, count, glm::value_ptr(vec));
	else
		return; //std::cout << "WARNING::SHADER.H::An active uniform corresponding to the name '" << name << "' was not found!\n";
}