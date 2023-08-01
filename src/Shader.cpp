#include "pch.h"
#include "Shader.hpp"
#include "Logger.hpp"

void Shader::checkCompileErrors(GLuint shader, std::string type) {
		GLint success;
		GLchar infoLog[1024];
		if (type != "PROGRAM") {
			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
			if (!success) {
				glGetShaderInfoLog(shader, 1024, NULL, infoLog);
				std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << std::endl;
			}
		}
		else {
			glGetProgramiv(shader, GL_LINK_STATUS, &success);
			if (!success) {
				glGetProgramInfoLog(shader, 1024, NULL, infoLog);
				std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << std::endl;
			}
		}
	}

void Shader::loadShader(const char* vertexPath, const char* fragmentPath, const char* geometryPath) {
	//MODIFIED VERSION OF LEARNOPENGL.COM's SHADER LOADING
	std::string vertexContent;
	std::string fragmentContent;
	std::string geometryContent;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;
	std::ifstream gShaderFile;

	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try {
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);

		std::stringstream vShaderStream, fShaderStream;

		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();

		vShaderFile.close();
		fShaderFile.close();

		vertexContent = vShaderStream.str();
		fragmentContent = fShaderStream.str();

		if (geometryPath != nullptr) {
			gShaderFile.open(geometryPath);
			std::stringstream gShaderStream;
			gShaderStream << gShaderFile.rdbuf();
			gShaderFile.close();
			geometryContent = gShaderStream.str();
		}
	}
	catch (std::ifstream::failure e) {
		mLog(std::string("Shader file not successfully read! Ifstream failure message: ") + e.what(), Log::LogError);
	}

	//Define shaders
	const char* vShaderContent = vertexContent.c_str();
	const char* fShaderContent = fragmentContent.c_str();
	const char* gShaderContent = geometryContent.c_str();

	GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);
	GLuint geometry = (geometryPath != nullptr) ? glCreateShader(GL_GEOMETRY_SHADER) : 0;
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

	if (geometryPath != nullptr) {
		const char* gShaderCode = geometryContent.c_str();
		geometry = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(geometry, 1, &gShaderCode, NULL);
		glCompileShader(geometry);
		checkCompileErrors(geometry, "GEOMETRY");
	}

	//Define shader program
	if (!id) id = glCreateProgram(); //If it doesn't have an id just give it

	glAttachShader(id, vertex);
	glAttachShader(id, fragment);
	if (geometryPath != nullptr) glAttachShader(id, geometry);
	glLinkProgram(id);

	//Check for errors
	glGetProgramiv(id, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(id, 512, NULL, infoLog);
		mLog(std::string("Program linking failed! Reason: ") + infoLog, Log::LogError);
	}

	//Clear the shader after they are linked
	glDetachShader(id, vertex);
	glDetachShader(id, fragment);
	if (geometryPath != nullptr) glDetachShader(id, geometry);

	glDeleteShader(vertex);
	glDeleteShader(fragment);
	if (geometryPath != nullptr) glDeleteShader(geometry);
}
Shader::Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath) {
	loadShader(vertexPath, fragmentPath, geometryPath);
};
Shader::~Shader() {
	mLog("~Shader has been triggered. Hint: Shader Program ID -> " + std::to_string(id), Log::LogDestructorInfo);
	deleteProgram();
}

Shader::Shader(Shader&& other) noexcept {
	id = std::exchange(other.id, 0);
}
Shader& Shader::operator=(Shader&& other) noexcept {
	deleteProgram();

	id = std::exchange(other.id, 0);
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

void Shader::set1b(const std::string& name, bool value) const {
	int location = glGetUniformLocation(id, name.c_str());

	if (location != -1)
		glUniform1i(location, (int)value);
	else
		return;// std::cout << "WARNING::SHADER.H::An active uniform corresponding to the name '" << name << "' was not found!\n";
}
void Shader::set1i(const std::string& name, int value) const {
	int location = glGetUniformLocation(id, name.c_str());

	if (location != -1)
		glUniform1i(location, (int)value);
	else
		return; //std::cout << "WARNING::SHADER.H::An active uniform corresponding to the name '" << name << "' was not found!\n";
}
void Shader::set1ui(const std::string& name, uint32_t value) const {
	int location = glGetUniformLocation(id, name.c_str());

	if (location != -1)
		glUniform1ui(location, (int)value);
	else
		return; //std::cout << "WARNING::SHADER.H::An active uniform corresponding to the name '" << name << "' was not found!\n";
}
void Shader::set1f(const std::string& name, float value) const {
	int location = glGetUniformLocation(id, name.c_str());

	if (location != -1)
		glUniform1f(location, (float)value);
	else
		return;// std::cout << "WARNING::SHADER.H::An active uniform corresponding to the name '" << name << "' was not found!\n";
}
void Shader::setMat3(const std::string& name, const glm::mat3& mat3) const {
	int location = glGetUniformLocation(id, name.c_str());

	if (location != -1)
		glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(mat3));
	else
		return; //std::cout << "WARNING::SHADER.H::An active uniform corresponding to the name '" << name << "' was not found!\n";
}
void Shader::setMat4(const std::string& name, const glm::mat4& mat4) const {
	int location = glGetUniformLocation(id, name.c_str());

	if (location != -1)
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(mat4));
	else
		return; //std::cout << "WARNING::SHADER.H::An active uniform corresponding to the name '" << name << "' was not found!\n";
}
void Shader::setVec2(const std::string& name, const glm::vec2& vec2) const {
	int location = glGetUniformLocation(id, name.c_str());

	if (location != -1)
		glUniform2f(location, vec2.x, vec2.y);
	else
		return; //std::cout << "WARNING::SHADER.H::An active uniform corresponding to the name '" << name << "' was not found!\n";
}
void Shader::setVec3(const std::string& name, const glm::vec3& vec3) const {
	int location = glGetUniformLocation(id, name.c_str());

	if (location != -1)
		glUniform3f(location, vec3.x, vec3.y, vec3.z);
	else
		return; //std::cout << "WARNING::SHADER.H::An active uniform corresponding to the name '" << name << "' was not found!\n";
}
void Shader::setVec4(const std::string& name, const glm::vec4& vec4) const {
	int location = glGetUniformLocation(id, name.c_str());

	if (location != -1)
		glUniform4f(glGetUniformLocation(id, name.c_str()), vec4.x, vec4.y, vec4.z, vec4.w);
	else
		return; //std::cout << "WARNING::SHADER.H::An active uniform corresponding to the name '" << name << "' was not found!\n";
}