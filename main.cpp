#if defined (__APPLE__)
    #define GLFW_INCLUDE_GLCOREARB
    #define GL_SILENCE_DEPRECATION
#else
#define GLEW_STATIC
#include <GL/glew.h>
#endif


#include <GLFW/glfw3.h>

#define GLM_ENABLE_EXPERIMENTAL

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

// window
gps::Window myWindow;

// matrices
glm::mat4 model;
glm::mat4 view;
glm::mat4 projection;
glm::mat3 normalMatrix;

// light parameters
glm::vec3 lightDir;
glm::vec3 lightColor;

struct ShaderLocations
{
	GLint modelLoc;
	GLint viewLoc;
	GLint projectionLoc;
	GLint normalMatrixLoc;
	GLint lightDirLoc;
	GLint lightColorLoc;
};

ShaderLocations shaderLocations;

// camera
gps::Camera myCamera(
	glm::vec3(0.0f, 0.0f, 3.0f),
	glm::vec3(0.0f, 0.0f, -10.0f),
	glm::vec3(0.0f, 1.0f, 0.0f));
GLfloat cameraSpeed = 0.1f;

GLboolean pressedKeys[1024];

// models
gps::Model3D teapot;
GLfloat angle;

// shaders
gps::Shader myBasicShader;

struct AABB
{
	glm::vec3 min;
	glm::vec3 max;
};

AABB cameraAABB;
std::vector<AABB> sceneObjectsAABBs;
std::vector<glm::mat4> sceneObjectsModelMatrices;

float boundingBoxVertices[] = {
	-0.5f, -0.5f, -0.5f, // Bottom-left front
	0.5f, -0.5f, -0.5f, // Bottom-right front
	0.5f, 0.5f, -0.5f, // Top-right front
	-0.5f, 0.5f, -0.5f, // Top-left front
	-0.5f, -0.5f, 0.5f, // Bottom-left back
	0.5f, -0.5f, 0.5f, // Bottom-right back
	0.5f, 0.5f, 0.5f, // Top-right back
	-0.5f, 0.5f, 0.5f // Top-left back
};

unsigned int boundingBoxIndices[] = {
	0, 1, 1, 2, 2, 3, 3, 0, // Front face edges
	4, 5, 5, 6, 6, 7, 7, 4, // Back face edges
	0, 4, 1, 5, 2, 6, 3, 7 // Connecting edges
};


float cubeVertices[] = {
	-0.5f, -0.5f, -0.5f,
	0.5f, -0.5f, -0.5f,
	0.5f, 0.5f, -0.5f,
	-0.5f, 0.5f, -0.5f,
	-0.5f, -0.5f, 0.5f,
	0.5f, -0.5f, 0.5f,
	0.5f, 0.5f, 0.5f,
	-0.5f, 0.5f, 0.5f
};

unsigned int cubeIndices[] = {
	0, 1, 1, 2, 2, 3, 3, 0, // Bottom square
	4, 5, 5, 6, 6, 7, 7, 4, // Top square
	0, 4, 1, 5, 2, 6, 3, 7 // Connecting edges
};

unsigned int cubeVAO, cubeVBO, cubeEBO;
unsigned int cubeShaderProgram;

void CubeDataInitializeAndBind() {
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &cubeVBO);
	glGenBuffers(1, &cubeEBO);

	glBindVertexArray(cubeVAO);

	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), cubeIndices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
}

void mutateVertex(int index, glm::vec3 newPosition) {
	cubeVertices[index] = newPosition.x;

	glBindVertexArray(cubeVAO);

	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferSubData(GL_ARRAY_BUFFER, index * sizeof(glm::vec3), sizeof(glm::vec3), glm::value_ptr(newPosition));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void renderCube(gps::Shader shader) {
	shader.useShaderProgram();

	float time = glfwGetTime();
	glm::mat4 transformation_rotate = glm::rotate(glm::mat4(1.0f), time, glm::vec3(0.5f, 1.0f, 0.0f));
	glUniformMatrix4fv(shaderLocations.modelLoc, 1, GL_FALSE, glm::value_ptr(transformation_rotate));

	glBindVertexArray(cubeVAO);
	glDrawElements(GL_LINES, sizeof(cubeIndices) / sizeof(unsigned int), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void cleanupCube() {
	glDeleteVertexArrays(1, &cubeVAO);
	glDeleteBuffers(1, &cubeVBO);
	glDeleteBuffers(1, &cubeEBO);
	glDeleteProgram(cubeShaderProgram);
}

AABB computeAABB(const std::vector<gps::Vertex>& vertices) {
	if (vertices.empty()) {
		return AABB{glm::vec3(0.0f), glm::vec3(0.0f)};
	}

	glm::vec3 min = vertices[0].Position;
	glm::vec3 max = min;

	for (size_t i = 1; i < vertices.size(); ++i) {
		glm::vec3 vertex = vertices[i].Position;
		min = glm::min(min, vertex);
		max = glm::max(max, vertex);
	}

	return {min, max};
}

AABB createCameraAABB(const glm::vec3& position, float size = 1.0f) {
	glm::vec3 halfSize(size / 2.0f);
	return {
		position - halfSize,
		position + halfSize
	};
}


AABB transformAABB(const AABB& aabb, const glm::mat4& modelMatrix) {
	glm::vec3 corners[8] = {
		glm::vec3(aabb.min.x, aabb.min.y, aabb.min.z),
		glm::vec3(aabb.min.x, aabb.min.y, aabb.max.z),
		glm::vec3(aabb.min.x, aabb.max.y, aabb.min.z),
		glm::vec3(aabb.min.x, aabb.max.y, aabb.max.z),
		glm::vec3(aabb.max.x, aabb.min.y, aabb.min.z),
		glm::vec3(aabb.max.x, aabb.min.y, aabb.max.z),
		glm::vec3(aabb.max.x, aabb.max.y, aabb.min.z),
		glm::vec3(aabb.max.x, aabb.max.y, aabb.max.z),
	};

	glm::vec3 transformedMin = glm::vec3(modelMatrix * glm::vec4(corners[0], 1.0f));
	glm::vec3 transformedMax = transformedMin;

	for (int i = 1; i < 8; i++) {
		glm::vec3 transformed = glm::vec3(modelMatrix * glm::vec4(corners[i], 1.0f));
		transformedMin = glm::min(transformedMin, transformed);
		transformedMax = glm::max(transformedMax, transformed);
	}

	return {transformedMin, transformedMax};
}

bool checkCollision(const AABB& a, const AABB& b) {
	return (a.max.x >= b.min.x && a.min.x <= b.max.x) &&
		(a.max.y >= b.min.y && a.min.y <= b.max.y) &&
		(a.max.z >= b.min.z && a.min.z <= b.max.z);
}


// camera animation variables
glm::vec3 focusPoint(0.0f, 0.0f, 0.0f);
glm::vec3 initialPosition;
glm::vec3 forwardDirection;
bool isAnimating = false;
float animationProgress = 0.0f;

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

void windowResizeCallback(GLFWwindow* window, int width, int height) {
	fprintf(stdout, "Window resized! New width: %d , and height: %d\n", width, height);
	//TODO
}

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	if (key == GLFW_KEY_V && action == GLFW_PRESS) {
		isAnimating = true;
		animationProgress = 0.0f;

		initialPosition = myCamera.getPosition();
		focusPoint = myCamera.getTarget();

		forwardDirection = glm::normalize(myCamera.getTarget() - myCamera.getPosition());
	}

	if (key >= 0 && key < 1024) {
		if (action == GLFW_PRESS) {
			pressedKeys[key] = true;
		}
		else if (action == GLFW_RELEASE) {
			pressedKeys[key] = false;
		}
	}
}

float lastX = 400, lastY = 300;
bool firstMouse = true;

void reset_lastX_lastY() {
	lastX = 400;
	lastY = 300;
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
	if (firstMouse) {
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xOffset = xpos - lastX;
	float yOffset = lastY - ypos;

	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.1f;
	xOffset *= sensitivity;
	yOffset *= sensitivity;

	myCamera.rotate(yOffset, xOffset);

	view = myCamera.getViewMatrix();
	myBasicShader.useShaderProgram();
	glUniformMatrix4fv(shaderLocations.viewLoc, 1, GL_FALSE, glm::value_ptr(view));
}


void processMovement() {
	if (pressedKeys[GLFW_KEY_W]) {
		myCamera.move(gps::MOVE_FORWARD, cameraSpeed);
		//update view matrix
		view = myCamera.getViewMatrix();
		myBasicShader.useShaderProgram();
		glUniformMatrix4fv(shaderLocations.viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		// compute normal matrix for teapot
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
	}

	if (pressedKeys[GLFW_KEY_S]) {
		myCamera.move(gps::MOVE_BACKWARD, cameraSpeed);
		//update view matrix
		view = myCamera.getViewMatrix();
		myBasicShader.useShaderProgram();
		glUniformMatrix4fv(shaderLocations.viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		// compute normal matrix for teapot
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
	}

	if (pressedKeys[GLFW_KEY_A]) {
		myCamera.move(gps::MOVE_LEFT, cameraSpeed);
		//update view matrix
		view = myCamera.getViewMatrix();
		myBasicShader.useShaderProgram();
		glUniformMatrix4fv(shaderLocations.viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		// compute normal matrix for teapot
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
	}

	if (pressedKeys[GLFW_KEY_D]) {
		myCamera.move(gps::MOVE_RIGHT, cameraSpeed);
		//update view matrix
		view = myCamera.getViewMatrix();
		myBasicShader.useShaderProgram();
		glUniformMatrix4fv(shaderLocations.viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		// compute normal matrix for teapot
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
	}

	if (pressedKeys[GLFW_KEY_Q]) {
		angle -= 1.0f;
		// update model matrix for teapot
		model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
		// update normal matrix for teapot
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
	}

	if (pressedKeys[GLFW_KEY_E]) {
		angle += 1.0f;
		// update model matrix for teapot
		model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
		// update normal matrix for teapot
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
	}

	cameraAABB = createCameraAABB(myCamera.getPosition());
}

void checkCollisionsWithObjects() {
	AABB cameraAABB = createCameraAABB(myCamera.getPosition());
	glm::vec3 cameraPosition = myCamera.getPosition();
	// std::cout << cameraPosition.x << " " << cameraPosition.z << " " << cameraPosition.y << "\n";

	for (size_t i = 0; i < sceneObjectsAABBs.size(); ++i) {
		AABB transformedVillageAABB = transformAABB(sceneObjectsAABBs[i], sceneObjectsModelMatrices[i]);
		// std::cout << transformedVillageAABB.max.x << transformedVillageAABB.max.z << transformedVillageAABB.max.y <<
		// 	"\n";
		// std::cout << transformedVillageAABB.min.x << transformedVillageAABB.min.z << transformedVillageAABB.min.y <<
		// 	"\n";

		if (checkCollision(cameraAABB, transformedVillageAABB)) {
			std::cout << "Collison detected" << "\n";
			// myCamera.setPosition(initialPosition, focusPoint);
			// view = myCamera.getViewMatrix();
			// myBasicShader.useShaderProgram();
			// glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
			// break; // Exit after handling collision
		}
	}
}

float calculateVerticalAngle(const glm::vec3& forwardDir) {
	glm::vec3 normalizedForward = glm::normalize(forwardDir);
	glm::vec3 forwardXZ = glm::normalize(glm::vec3(forwardDir.x, 0.0f, forwardDir.z));
	float angle = glm::degrees(glm::acos(glm::dot(normalizedForward, forwardXZ)));
	if (forwardDir.y < 0) {
		angle = -angle;
	}
	return angle;
}

void updateCameraAnimation(float deltaTime) {
	if (!isAnimating) return;

	float duration = 2.0f;
	animationProgress += deltaTime / duration;

	if (animationProgress >= 1.0f) {
		animationProgress = 1.0f;
		isAnimating = false;
	}

	float distance = sin(glm::radians(180.0f * animationProgress)) * 30.0f;
	glm::vec3 newPosition;
	float distance_x = distance * -forwardDirection.x;
	float distance_z = distance * -forwardDirection.z;
	float distance_y = distance * -forwardDirection.y;

	newPosition.x = initialPosition.x + distance_x;
	newPosition.z = initialPosition.z + distance_z;
	newPosition.y = initialPosition.y + distance_y;

	myCamera.setPosition(newPosition, focusPoint);

	// Update the view matrix
	view = myCamera.getViewMatrix();
	myBasicShader.useShaderProgram();
	glUniformMatrix4fv(shaderLocations.viewLoc, 1, GL_FALSE, glm::value_ptr(view));
}


void initOpenGLWindow() {
	myWindow.Create(1024, 768, "OpenGL Project Core");
}

void setWindowCallbacks() {
	glfwSetWindowSizeCallback(myWindow.getWindow(), windowResizeCallback);
	glfwSetKeyCallback(myWindow.getWindow(), keyboardCallback);
	glfwSetCursorPosCallback(myWindow.getWindow(), mouseCallback);

	glfwSetInputMode(myWindow.getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void initOpenGLState() {
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
}

void initBoundingBoxes() {
	gps::Mesh teapot_mesh = teapot.getSubObjects()[0];
	AABB teapotAABB = computeAABB(teapot_mesh.getVertices());

	sceneObjectsAABBs.push_back(teapotAABB);
	sceneObjectsModelMatrices.push_back(glm::mat4(1.0f));

	cameraAABB = createCameraAABB(myCamera.getPosition());
}

gps::Shader boundingBoxShader;

void loadShaders() {
	myBasicShader.loadShader(
		"shaders/basic.vert",
		"shaders/basic.frag");

	boundingBoxShader.loadShader("shaders/boundingBox.vert", "shaders/boundingBox.frag");
}

void initBasicShaderPipeline() {
	myBasicShader.useShaderProgram();

	model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
	shaderLocations.modelLoc = glGetUniformLocation(myBasicShader.shaderProgram, "model");

	view = myCamera.getViewMatrix();
	shaderLocations.viewLoc = glGetUniformLocation(myBasicShader.shaderProgram, "view");
	glUniformMatrix4fv(shaderLocations.viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
	shaderLocations.normalMatrixLoc = glGetUniformLocation(myBasicShader.shaderProgram, "normalMatrix");

	projection = glm::perspective(glm::radians(45.0f),
	                              (float)myWindow.getWindowDimensions().width / (float)myWindow.getWindowDimensions().
	                              height,
	                              0.01f, 100.0f);

	shaderLocations.projectionLoc = glGetUniformLocation(myBasicShader.shaderProgram, "projection");
	glUniformMatrix4fv(shaderLocations.projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	lightDir = glm::vec3(0.0f, 1.0f, 1.0f);
	shaderLocations.lightDirLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightDir");

	glUniform3fv(shaderLocations.lightDirLoc, 1, glm::value_ptr(lightDir));

	lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
	shaderLocations.lightColorLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightColor");

	glUniform3fv(shaderLocations.lightColorLoc, 1, glm::value_ptr(lightColor));
}

void renderTeapot(gps::Shader shader) {
	shader.useShaderProgram();

	float teapotScale = 1.0f;
	glm::mat4 transformation_scaling = glm::scale(glm::mat4(1.0f), glm::vec3(teapotScale));
	glUniformMatrix4fv(shaderLocations.modelLoc, 1, GL_FALSE, glm::value_ptr(transformation_scaling));

	// used for lights and such
	glm::mat3 teapotNormalMatrix = glm::mat3(glm::inverseTranspose(view * transformation_scaling));
	glUniformMatrix3fv(shaderLocations.normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(teapotNormalMatrix));

	teapot.Draw(shader);
}

void renderScene() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	renderTeapot(myBasicShader);
	renderCube(myBasicShader);
}


// void renderScene() {
// 	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
// 	// render the teapot
// 	renderTeapot(myBasicShader);
// 	// render village
// 	// renderVillage(myBasicShader);
//
// 	// Render Bounding Boxes for the Village
//
// 	boundingBoxShader.useShaderProgram();
// 	for (size_t i = 0; i < sceneObjectsAABBs.size(); ++i) {
// 		AABB transformedAABB = transformAABB(sceneObjectsAABBs[i], sceneObjectsModelMatrices[i]);
// 		renderAABB(transformedAABB, view, projection);
// 	}
// }

void cleanup() {
	myWindow.Delete();
	//cleanup code for your own data
}

int main(int argc, const char* argv[]) {
	try {
		initOpenGLWindow();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	initOpenGLState();
	loadModels();
	loadShaders();
	initBasicShaderPipeline();
	setWindowCallbacks();
	CubeDataInitializeAndBind();
	glCheckError();

	float lastFrameTime = glfwGetTime();
	while (!glfwWindowShouldClose(myWindow.getWindow())) {
		float currentFrameTime = glfwGetTime();
		float deltaTime = currentFrameTime - lastFrameTime;
		lastFrameTime = currentFrameTime;

		processMovement();
		// checkCollisionsWithObjects();
		updateCameraAnimation(deltaTime);

		renderScene();

		glfwPollEvents();
		glfwSwapBuffers(myWindow.getWindow());

		glCheckError();
	}

	// Cleanup
	cleanupCube();
	cleanup();

	return EXIT_SUCCESS;
}

