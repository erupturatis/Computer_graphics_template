#define GLM_ENABLE_EXPERIMENTAL

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>
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
#include "CommonGroupings.h"
#include "Mouse.h"
#include "Scene.h"
#include <random>
#include "Collisions.h"
#include "DayNightCycle.h"
#include "Lights.h"
#include "Others.h"

// SHADOWS
unsigned int depthMapFBO;
unsigned int depthMap;

globals::Shader depthMapViewShader;
globals::Shader fragPosLightSpaceShader;

void loadModels() {
	scene::loadTerrain();
	scene::loadHouse1();
	scene::loadShrooms();
	scene::loadBook();
}


void loadShaders() {
	globals::Shader& myBasicShader = globals::getBasicShader();
	myBasicShader.loadShader(
		"shaders/basic.vert",
		"shaders/basic.frag");

	globals::Shader& depthShader = globals::getDepthShader();
	depthShader.loadShader(
		"shaders/simpleDepth.vert",
		"shaders/simpleDepth.frag");

	depthMapViewShader.loadShader("shaders/depthMapView.vert", "shaders/depthMapView.frag");
	fragPosLightSpaceShader.loadShader("shaders/fragPosLightSpaceView.vert", "shaders/fragPosLightSpaceView.frag");
}

void initBasicShaderMatrices() {
	setMatrixViewBasicShader();
	setMatrixProjectionBasicShader();
}

void initShadowMapTexture() {
	globals::Shader& basicShader = globals::getBasicShader();
	glUseProgram(basicShader.shaderProgram); // Ensure the shader program is in use
	GLint depthMapLocation = glGetUniformLocation(basicShader.shaderProgram, "shadowMap");
	if (depthMapLocation == -1) {
		std::cerr << "Error: shadowMap uniform not found in shader!" << std::endl;
	}
	else {
		glUniform1i(depthMapLocation, 3);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, depthMap);
	}
}

void initLightsMatrices() {
	globals::Camera& myCamera = globals::getCamera();
	globals::Shader& basicShader = globals::getBasicShader();
	basicShader.useShaderProgram();
	globals_structs::ShaderLocationsBasic& basicShaderLocations = globals::getBasicShaderLocations();
	glm::mat4 view = myCamera.getViewMatrix();
	glm::vec3 lightDir = globals::getLightDirDir();
	glm::mat4 lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(globals::getLightDirRotationAngle()),
	                                      globals::getLightDirRotationAxis());

	// set light direction adjusted
	glUniform3fv(basicShaderLocations.lightDirDir, 1,
	             glm::value_ptr(glm::inverseTranspose(glm::mat3(view * lightRotation)) * lightDir));
	// set light color
	glUniform3fv(basicShaderLocations.lightDirColor, 1, glm::value_ptr(globals::getLightDirColor()));
	// set lightSpaceMatrix
	glUniformMatrix4fv(basicShaderLocations.lightSpaceMatrix, 1, GL_FALSE,
	                   glm::value_ptr(scene::calculateLightSpaceMatrix()));
}

void initLightSpaceDepthShader() {
	glm::mat4 lightSpaceMatrix = scene::calculateLightSpaceMatrix();

	globals::Shader& depthShader = globals::getDepthShader();
	globals_structs::ShaderLocationsDepth& depthShaderLocations = globals::getDepthShaderLocations();
	depthShader.useShaderProgram();
	glUniformMatrix4fv(depthShaderLocations.lightSpaceMatrix, 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
}

void initShadowsSettings() {
	glGenFramebuffers(1, &depthMapFBO);
	//create depth texture for FBO
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
	             globals::getShadowWidth(), globals::getShadowHeight(), 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	float borderColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	//attach texture to FBO
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap,
	                       0);

	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void initObjectsScene() {
	scene::initTerrain();
	scene::initHouse1();
	scene::initShrooms();
	scene::initBook();
}


void renderSceneDepth() {
	scene::renderTerrain(true);
	scene::renderHouse1(true);
	scene::renderShrooms(true);
	scene::renderBook(true);
}

void renderSceneNormal() {
	scene::renderTerrain(false);
	scene::renderHouse1(false);
	scene::renderShrooms(false);
	scene::renderBook(false);

	if (globals_configs::getWireframeMode())
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

unsigned int quadVAO = 0, quadVBO = 0;

void renderQuad() {
	glViewport(0, 0, globals::getWindowWidth(), globals::getWindowHeight());
	if (quadVAO == 0) {
		float quadVertices[] = {
			// positions        // texture coords
			-1.0f, 1.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f,
			1.0f, -1.0f, 1.0f, 0.0f,

			-1.0f, 1.0f, 0.0f, 1.0f,
			1.0f, -1.0f, 1.0f, 0.0f,
			1.0f, 1.0f, 1.0f, 1.0f
		};

		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

		// Position attribute
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		// Texture coordinate attribute
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
		glEnableVertexAttribArray(1);

		glBindVertexArray(0);
	}

	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

void renderDepthMap() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	depthMapViewShader.useShaderProgram();

	GLint depthMapLocation = glGetUniformLocation(depthMapViewShader.shaderProgram, "depthMap");
	glUniform1i(depthMapLocation, 0); // Texture unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, depthMap);

	renderQuad();
}

void renderSceneWithShadows(bool renderDepth = false) {
	// render scene for depth

	glViewport(0, 0, globals::getShadowWidth(), globals::getShadowHeight());
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	initLightSpaceDepthShader();
	renderSceneDepth();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	if (renderDepth == false) {
		glViewport(0, 0, globals::getWindowWidth(), globals::getWindowHeight());
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		globals::getBasicShader().useShaderProgram();
		initBasicShaderMatrices();
		initLightsMatrices();
		initShadowMapTexture();
		// set boolean uniform for fog to true

		GLint fogLocation = glGetUniformLocation(globals::getBasicShader().shaderProgram, "applyFog");
		glUniform1i(fogLocation, globals_configs::getApplyFog());

		renderSceneNormal();
	}
	else {
		renderDepthMap();
	}
}


void cleanup() {
	globals::getWindow().Delete();
	//cleanup code for your own data
}

int countNonWhiteDepthValues(unsigned int depthMap, int width, int height, float epsilon = 1e-5f) {
	glBindTexture(GL_TEXTURE_2D, depthMap);
	std::vector<float> depthData(width * height);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, GL_FLOAT, depthData.data());
	GLenum err;
	while ((err = glGetError()) != GL_NO_ERROR) {
		std::cerr << "OpenGL Error while reading depth map: " << err << std::endl;
		return -1;
	}

	int count = 0;
	for (const float& depth : depthData) {
		if (depth < (1.0f - epsilon)) {
			count++;
		}
	}
	return count;
}


int main(int argc, const char* argv[]) {
	try {
		globals::initGlobalObjects();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	initOpenGLState();
	loadModels();
	loadShaders();
	globals::initShadersLocations();
	initBasicShaderMatrices();
	lights::initLights();
	initShadowsSettings();
	initObjectsScene();
	bindWindowAndKeyboardCallbacks();

	glCheckError();

	float lastFrameTime = glfwGetTime();
	globals::Window& myWindow = globals::getWindow();

	while (!glfwWindowShouldClose(myWindow.getWindow())) {
		float currentFrameTime = glfwGetTime();
		float deltaTime = currentFrameTime - lastFrameTime;
		lastFrameTime = currentFrameTime;

		globals::processCameraMovement();
		cameraAnimation::updateCameraAnimation(deltaTime);
		day_night_cycle::handleLightsDayNightCycle();

		renderSceneWithShadows(false);

		glfwPollEvents();
		glfwSwapBuffers(myWindow.getWindow());

		glCheckError();
	}

	// Cleanup
	cleanup();

	return EXIT_SUCCESS;
}

