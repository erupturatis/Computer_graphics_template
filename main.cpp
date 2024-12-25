#define GLM_ENABLE_EXPERIMENTAL

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp> //core glm functionality
#include <glm/gtc/matrix_transform.hpp> //glm extension for generating common transformation matrices
#include <glm/gtc/matrix_inverse.hpp> //glm extension for computing inverse matrices
#include <glm/gtc/type_ptr.hpp> //glm extension for accessing the internal data structure of glm types
#include <glm/gtx/string_cast.hpp>

#include "tiny_obj_loader.h"
#include <iostream>
#include "Window.h"
#include "Shader.hpp"
#include "Camera.hpp"
#include "Model3D.hpp"
#include <vector>

#include "Bindings.h"
#include "CameraAnimation.h"
#include "CommonGroupings.h"
#include "pure_functions.h"
#include "GlobalObjects.h"
#include "Mouse.h"
#include "Scene.h"

// light parameters
glm::vec3 lightDir;
glm::vec3 lightColor;

// models
globals::Model3D teapot;

GLenum glCheckError_(const char* file, int line) {
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR) {
		std::string error;
		switch (errorCode) {
		case GL_INVALID_ENUM:
			error = "INVALID_ENUM";
			break;
		case GL_INVALID_VALUE:
			error = "INVALID_VALUE";
			break;
		case GL_INVALID_OPERATION:
			error = "INVALID_OPERATION";
			break;
		case GL_OUT_OF_MEMORY:
			error = "OUT_OF_MEMORY";
			break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			error = "INVALID_FRAMEBUFFER_OPERATION";
			break;
		}
		std::cout << error << " | " << file << " (" << line << ")" << std::endl;
	}
	return errorCode;
}

#define glCheckError() glCheckError_(__FILE__, __LINE__)

glm::mat3 normalMatrix;
glm::mat4 model;

void processCameraMovement() {
	globals::Camera& myCamera = globals::getCamera();
	float cameraSpeed = globals_configs::getCameraSpeed();

	if (globals::isKeyPressed(GLFW_KEY_W)) {
		myCamera.move(globals::MOVE_FORWARD, cameraSpeed);
		setMatrixViewBasicShader();
		scene::recalculateNormals();
		// glm::mat4 view = myCamera.getViewMatrix();
		// normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
	}

	if (globals::isKeyPressed(GLFW_KEY_S)) {
		myCamera.move(globals::MOVE_BACKWARD, cameraSpeed);
		setMatrixViewBasicShader();
		scene::recalculateNormals();
	}

	if (globals::isKeyPressed(GLFW_KEY_A)) {
		myCamera.move(globals::MOVE_LEFT, cameraSpeed);
		setMatrixViewBasicShader();
		scene::recalculateNormals();
	}

	if (globals::isKeyPressed(GLFW_KEY_D)) {
		myCamera.move(globals::MOVE_RIGHT, cameraSpeed);
		setMatrixViewBasicShader();
		scene::recalculateNormals();
	}
}

void initGlobalObjects() {
	// window
	globals::Window window = globals::Window();
	window.Create(1024, 768, "OpenGL Project");
	globals::setWindow(window);

	// camera
	globals::Camera camera = globals::Camera();
	camera.initializeCamera(
		glm::vec3(0.0f, 0.0f, 3.0f),
		glm::vec3(0.0f, 0.0f, -10.0f),
		glm::vec3(0.0f, 1.0f, 0.0f));
	globals::setCamera(camera);


	// common matrices for shader (projection and view)
	globals_structs::MatricesShaderCommon matrices = globals_structs::MatricesShaderCommon();
	globals::setMatrices(matrices);

	// locations for shader GPU matrices
	globals_structs::ShaderLocations shaderLocations = globals_structs::ShaderLocations();
	globals::setBasicShaderLocations(shaderLocations);

	// shader
	globals::Shader myBasicShader = globals::Shader();
	globals::setBasicShader(myBasicShader);
}

void initOpenGLState() {
	globals::Window& myWindow = globals::getWindow();
	glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
	glViewport(0, 0, myWindow.getWindowDimensions().width, myWindow.getWindowDimensions().height);

	glEnable(GL_FRAMEBUFFER_SRGB);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	glDisable(GL_CULL_FACE);
}

void loadModels() {
	teapot.LoadModel("models/teapot/teapot20segUT.obj");
	scene::registerModel(teapot);
}

void loadShaders() {
	globals::Shader& myBasicShader = globals::getBasicShader();
	myBasicShader.loadShader(
		"shaders/basic.vert",
		"shaders/basic.frag");
}

void initBasicShaderPipeline() {
	globals::Shader& myBasicShader = globals::getBasicShader();
	myBasicShader.useShaderProgram();
	GLuint shaderProgram = myBasicShader.shaderProgram;

	globals_structs::MatricesShaderCommon& matrices = globals::getMatrices();
	globals_structs::ShaderLocations& shaderLocations = globals::getBasicShaderLocations();

	// Retrieve uniform locations
	shaderLocations.viewLoc = glGetUniformLocation(shaderProgram, "view");
	shaderLocations.modelLoc = glGetUniformLocation(shaderProgram, "model");
	shaderLocations.normalMatrixLoc = glGetUniformLocation(shaderProgram, "normalMatrix");
	shaderLocations.projectionLoc = glGetUniformLocation(shaderProgram, "projection");
	shaderLocations.lightDirLoc = glGetUniformLocation(shaderProgram, "lightDir");

	// Validate uniform locations
	if (shaderLocations.modelLoc == -1) {
		std::cerr << "Invalid uniform location for 'model'" << std::endl;
	}

	if (shaderLocations.viewLoc == -1) {
		std::cerr << "Invalid uniform location for 'view'" << std::endl;
	}

	if (shaderLocations.projectionLoc == -1) {
		std::cerr << "Invalid uniform location for 'projection'" << std::endl;
	}

	if (shaderLocations.normalMatrixLoc == -1) {
		std::cerr << "Invalid uniform location for 'normalMatrix'" << std::endl;
	}

	if (shaderLocations.lightDirLoc == -1) {
		std::cerr << "Invalid uniform location for 'lightDir'" << std::endl;
	}

	setMatrixViewBasicShader();
	setMatrixProjectionBasicShader();
}

void initLights() {
	globals::Shader& myBasicShader = globals::getBasicShader();
	globals_structs::ShaderLocations& shaderLocations = globals::getBasicShaderLocations();

	lightDir = glm::vec3(0.0f, 1.0f, 1.0f);
	glUniform3fv(shaderLocations.lightDirLoc, 1, glm::value_ptr(lightDir));
	lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
	shaderLocations.lightColorLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightColor");
	glUniform3fv(shaderLocations.lightColorLoc, 1, glm::value_ptr(lightColor));
}

void initObjectsScene() {
	GLfloat angle = 90.0f;

	glm::mat4 model_rotation = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
	teapot.setModelMatrix(model_rotation);

	scene::recalculateNormals();
}


void renderTeapot() {
	globals::Shader& shader = globals::getBasicShader();
	globals_structs::ShaderLocations& shaderLocations = globals::getBasicShaderLocations();

	shader.useShaderProgram();

	float teapotScale = 1.0f;
	float time = glfwGetTime() * 10.0f;

	glm::mat4 transformation_scaling = glm::scale(glm::mat4(1.0f), glm::vec3(teapotScale));
	glm::mat4 transformation_rotation = glm::rotate(glm::mat4(1.0f), glm::radians(time), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 transformation = transformation_rotation * transformation_scaling;
	teapot.setModelMatrix(transformation);

	glUniformMatrix4fv(shaderLocations.modelLoc, 1, GL_FALSE, glm::value_ptr(teapot.getModelMatrix()));
	glUniformMatrix3fv(shaderLocations.normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(teapot.getNormalMatrix()));


	teapot.Draw(shader);
}

void renderScene() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	renderTeapot();
	scene::recalculateNormals();
}


void cleanup() {
	globals::getWindow().Delete();
	//cleanup code for your own data
}

int main(int argc, const char* argv[]) {
	try {
		initGlobalObjects();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	initOpenGLState();
	loadModels();
	loadShaders();
	initBasicShaderPipeline();
	initLights();
	initObjectsScene();
	bindWindowCallbacks();

	glCheckError();

	float lastFrameTime = glfwGetTime();
	globals::Window& myWindow = globals::getWindow();

	while (!glfwWindowShouldClose(myWindow.getWindow())) {
		float currentFrameTime = glfwGetTime();
		float deltaTime = currentFrameTime - lastFrameTime;
		lastFrameTime = currentFrameTime;

		processCameraMovement();
		cameraAnimation::updateCameraAnimation(deltaTime);
		renderScene();

		glfwPollEvents();
		glfwSwapBuffers(myWindow.getWindow());

		glCheckError();
	}

	// Cleanup
	cleanup();

	return EXIT_SUCCESS;
}

