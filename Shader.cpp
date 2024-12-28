#include "Shader.hpp"

#include <glm/gtc/type_ptr.inl>

#include "GlobalObjects.h"

namespace globals
{
	std::string Shader::readShaderFile(std::string fileName) {
		std::ifstream shaderFile;
		std::string shaderString;

		//open shader file
		shaderFile.open(fileName);

		std::stringstream shaderStringStream;

		//read shader content into stream
		shaderStringStream << shaderFile.rdbuf();

		//close shader file
		shaderFile.close();

		//convert stream into GLchar array
		shaderString = shaderStringStream.str();
		return shaderString;
	}

	void Shader::shaderCompileLog(GLuint shaderId) {
		GLint success;
		GLchar infoLog[512];

		//check compilation info
		glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);

		if (!success) {
			glGetShaderInfoLog(shaderId, 512, NULL, infoLog);
			std::cout << "Shader compilation error\n" << infoLog << std::endl;
		}
	}

	void Shader::shaderLinkLog(GLuint shaderProgramId) {
		GLint success;
		GLchar infoLog[512];

		//check linking info
		glGetProgramiv(shaderProgramId, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
			std::cout << "Shader linking error\n" << infoLog << std::endl;
		}
	}

	void Shader::loadShader(std::string vertexShaderFileName, std::string fragmentShaderFileName) {
		//read, parse and compile the vertex shader
		std::string v = readShaderFile(vertexShaderFileName);
		const GLchar* vertexShaderString = v.c_str();
		GLuint vertexShader;
		vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShader, 1, &vertexShaderString, NULL);
		glCompileShader(vertexShader);
		//check compilation status
		shaderCompileLog(vertexShader);

		//read, parse and compile the vertex shader
		std::string f = readShaderFile(fragmentShaderFileName);
		const GLchar* fragmentShaderString = f.c_str();
		GLuint fragmentShader;
		fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShader, 1, &fragmentShaderString, NULL);
		glCompileShader(fragmentShader);
		//check compilation status
		shaderCompileLog(fragmentShader);

		//attach and link the shader programs
		this->shaderProgram = glCreateProgram();
		glAttachShader(this->shaderProgram, vertexShader);
		glAttachShader(this->shaderProgram, fragmentShader);
		glLinkProgram(this->shaderProgram);
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
		//check linking info
		shaderLinkLog(this->shaderProgram);
	}

	void Shader::useShaderProgram() {
		glUseProgram(this->shaderProgram);
	}

	void Shader::setMat4(const std::string& name, const glm::mat4& value) {
		glUniformMatrix4fv(glGetUniformLocation(this->shaderProgram, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
	}

	void Shader::setInt(const std::string& name, int value) {
		glUniform1i(glGetUniformLocation(this->shaderProgram, name.c_str()), value);
	}


	void initBasicShaderLocations() {
		globals::Shader& myBasicShader = globals::getBasicShader();
		myBasicShader.useShaderProgram();
		GLuint shaderProgram = myBasicShader.shaderProgram;
		globals_structs::ShaderLocationsBasic& shaderLocations = globals::getBasicShaderLocations();

		// vertex shader
		shaderLocations.viewLoc = glGetUniformLocation(shaderProgram, "view");
		shaderLocations.modelLoc = glGetUniformLocation(shaderProgram, "model");
		shaderLocations.projectionLoc = glGetUniformLocation(shaderProgram, "projection");
		shaderLocations.lightSpaceMatrix = glGetUniformLocation(shaderProgram, "lightSpaceMatrix");
		shaderLocations.normalMatrixLoc = glGetUniformLocation(shaderProgram, "normalMatrix");

		// fragment shader specific
		shaderLocations.shadowMap = glGetUniformLocation(shaderProgram, "shadowMap");

		shaderLocations.lightDirDir = glGetUniformLocation(shaderProgram, "lightDir");
		shaderLocations.lightDirColor = glGetUniformLocation(shaderProgram, "lightColor");

		shaderLocations.lightPointLoc = glGetUniformLocation(shaderProgram, "lightPointLoc");
		shaderLocations.lightPointColor = glGetUniformLocation(shaderProgram, "lightPointColor");
	}

	void initDepthShaderLocations() {
		globals::Shader& depthShader = globals::getDepthShader();
		depthShader.useShaderProgram();
		GLuint shaderProgram = depthShader.shaderProgram;
		globals_structs::ShaderLocationsDepth& shaderLocations = globals::getDepthShaderLocations();

		// Retrieve uniform locations
		shaderLocations.lightSpaceMatrix = glGetUniformLocation(shaderProgram, "lightSpaceMatrix");
		shaderLocations.model = glGetUniformLocation(shaderProgram, "model");
	}

	void initShadersLocations() {
		initBasicShaderLocations();
		initDepthShaderLocations();
	}
}
