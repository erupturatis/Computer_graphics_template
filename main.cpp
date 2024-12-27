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

globals::Shader depthShader;
globals_structs::ShaderLocationsDepth depthShaderLocations;

globals::Shader depthMapViewShader;

void loadModels() {
	scene::loadBook();
	scene::loadTerrain();
	scene::loadShrooms();
	scene::loadHouse1();
}

void loadShaders() {
	globals::Shader& myBasicShader = globals::getBasicShader();
	myBasicShader.loadShader(
		"shaders/basic.vert",
		"shaders/basic.frag");

	depthShader.loadShader(
		"shaders/simpleDepth.vert",
		"shaders/simpleDepth.frag");
	depthMapViewShader.loadShader("shaders/depthMapView.vert", "shaders/depthMapView.frag");
}

void initBasicShaderMatrices() {
	setMatrixViewBasicShader();
	setMatrixProjectionBasicShader();
}

void initSimpleDepthShader() {
	depthShader.useShaderProgram();
	float near_plane = 0.1f, far_plane = 500.0f;
	glm::mat4 lightProjection = glm::ortho(-500.0f, 500.0f,
	                                       -500.0f, 500.0f,
	                                       near_plane, far_plane);

	glm::vec3 normalizedLightDir = glm::normalize(globals::getLightDirDir());
	glm::vec3 lightPosition = -normalizedLightDir * 100.0f;

	glm::mat4 lightView = glm::lookAt(
		lightPosition, // Light position
		glm::vec3(0.0f, 0.0f, 0.0f), // Look at origin
		glm::vec3(0.0f, 1.0f, 0.0f) // Up vector
	);

	glm::mat4 lightSpaceMatrix = lightProjection * lightView;

	GLint lightSpaceMatrixLocation = glGetUniformLocation(depthShader.shaderProgram, "lightSpaceMatrix");
	GLint modelLocation = glGetUniformLocation(depthShader.shaderProgram, "model");

	depthShaderLocations.lightSpaceMatrix = lightSpaceMatrixLocation;
	depthShaderLocations.model = modelLocation;

	glUniformMatrix4fv(lightSpaceMatrixLocation, 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));

	// Store lightSpaceMatrix for basicShader
	globals::Shader& basicShader = globals::getBasicShader();
	basicShader.useShaderProgram();
	GLint lightSpaceMatrixBasicLoc = glGetUniformLocation(basicShader.shaderProgram, "lightSpaceMatrix");
	glUniformMatrix4fv(lightSpaceMatrixBasicLoc, 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
}

void initShadows() {
	glGenFramebuffers(1, &depthMapFBO);

	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
	             globals::getShadowWidth(), globals::getShadowHeight(), 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void initObjectsScene() {
	scene::initBook();
	scene::initShrooms();
	scene::initTerrain();
	scene::initHouse1();
}


void renderSceneDepth() {
	scene::renderHouse1(true);
	scene::renderBook(true);
	scene::renderTerrain(true);
	scene::renderShrooms(true);
}

void renderSceneNormal() {
	scene::renderHouse1(false);
	scene::renderBook(false);
	scene::renderTerrain(false);
	scene::renderShrooms(false);

	if (globals_configs::getWireframeMode())
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void renderSceneWithShadows() {
	// render scene for depth
	glViewport(0, 0, globals::getShadowWidth(), globals::getShadowHeight());
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	initSimpleDepthShader();
	renderSceneDepth();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// render scene normally
	glViewport(0, 0, globals::getWindowWidth(), globals::getWindowHeight());
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	initBasicShaderMatrices();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glUniform1i(glGetUniformLocation(globals::getBasicShader().shaderProgram, "shadowMap"), 0);

	renderSceneNormal();
}


void cleanup() {
	globals::getWindow().Delete();
	//cleanup code for your own data
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
	globals::initBasicShaderLocations();
	initBasicShaderMatrices();
	lights::initLights();
	initShadows();
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

		// renderSceneNormal();
		renderSceneWithShadows();

		glfwPollEvents();
		glfwSwapBuffers(myWindow.getWindow());

		glCheckError();
	}

	// Cleanup
	cleanup();

	return EXIT_SUCCESS;
}

//
// unsigned int quadVAO = 0, quadVBO = 0;
//
// void renderQuad() {
// 	if (quadVAO == 0) {
// 		std::cout << "renderQuad" << std::endl;
// 		float quadVertices[] = {
// 			-1.0f, 1.0f, 0.0f, 1.0f,
// 			-1.0f, -1.0f, 0.0f, 0.0f,
// 			1.0f, -1.0f, 1.0f, 0.0f,
//
// 			-1.0f, 1.0f, 0.0f, 1.0f,
// 			1.0f, -1.0f, 1.0f, 0.0f,
// 			1.0f, 1.0f, 1.0f, 1.0f
// 		};
//
// 		glGenVertexArrays(1, &quadVAO);
// 		glGenBuffers(1, &quadVBO);
// 		glBindVertexArray(quadVAO);
//
// 		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
// 		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
//
// 		glEnableVertexAttribArray(0);
// 		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
// 		glEnableVertexAttribArray(1);
// 		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
// 	}
//
// 	glBindVertexArray(quadVAO);
// 	glDrawArrays(GL_TRIANGLES, 0, 6);
// 	glBindVertexArray(0);
// }
//
//
// void renderDepthMap() {
// 	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
// 	depthMapViewShader.useShaderProgram();
//
// 	// Set the uniform for the depth map texture
// 	GLint depthMapLocation = glGetUniformLocation(depthMapViewShader.shaderProgram, "depthMap");
// 	glUniform1i(depthMapLocation, 0); // Texture unit 0
//
// 	// Bind the depth map texture
// 	glActiveTexture(GL_TEXTURE0);
// 	glBindTexture(GL_TEXTURE_2D, depthMap);
//
// 	// Render the quad
// 	renderQuad();
// }
//
