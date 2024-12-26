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
#include <random>

struct terrainBinding
{
	int i;
	int j;
};

// light parameters
glm::vec3 lightDir;
glm::vec3 lightColor;

unsigned int depthMapFBO;
const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
unsigned int depthMap;


// MODELS
globals::Model3D teapot;
// terrain
std::vector<globals::Model3D> terrain_blocks;
int terrain_blocks_count = 5;
float terrain_block_distance_x_axis_j_coord = 5.0f;
float terrain_block_distance_z_axis_i_coord = 5.5f;
float terrain_block_scale = 0.5f;

// environment

// shrooms
std::vector<globals::Model3D> shrooms;
std::vector<terrainBinding> shrooms_bindings;
std::vector<glm::vec3> shrooms_random_noise;
std::vector<glm::vec3> shrooms_random_rotations;
std::vector<float> shrooms_random_scales;
float shroom_default_y = 0.65f;
float shroom_scale = 1.0f;
int shrooms_count = 30;
int shrooms_types_start = 1;
int shrooms_types_end = 3;

// house
globals::Model3D house1;
float house1_scale = 0.1f;
float house1_default_y = 0.5f;
terrainBinding terrain_binding_house1;

// others
globals::Model3D book;
float book_scale = 10.0f;
float book_default_y = 7.0f;
float book_float_variation = 3.0f;
terrainBinding terrain_binding_book;

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

bool checkCollision(globals::MOVE_DIRECTION direction) {
	std::vector<globals::Model3D*>& models = scene::getModels3D();

	globals::Camera& myCamera = globals::getCamera();

	glm::vec3 expectedNextPosition = myCamera.getNextPosition(direction, globals_configs::getCameraSpeed());

	for (globals::Model3D* model : models) {
		std::vector<globals::BoundingBox3D>& boundingBoxes = model->getBoundingBoxes();

		for (globals::BoundingBox3D& boundingBox : boundingBoxes) {
			if (boundingBox.checkCollision(expectedNextPosition)) {
				return true;
			}
		}
	}

	return false;
}

void processCameraMovement() {
	globals::Camera& myCamera = globals::getCamera();
	float cameraSpeed = globals_configs::getCameraSpeed();

	if (globals::isKeyPressed(GLFW_KEY_W) && checkCollision(globals::MOVE_FORWARD) == false) {
		myCamera.move(globals::MOVE_FORWARD, cameraSpeed);
		setMatrixViewBasicShader();
		scene::recalculateNormals();
	}

	if (globals::isKeyPressed(GLFW_KEY_S) && checkCollision(globals::MOVE_BACKWARD) == false) {
		myCamera.move(globals::MOVE_BACKWARD, cameraSpeed);
		setMatrixViewBasicShader();
		scene::recalculateNormals();
	}

	if (globals::isKeyPressed(GLFW_KEY_A) && checkCollision(globals::MOVE_LEFT) == false) {
		myCamera.move(globals::MOVE_LEFT, cameraSpeed);
		setMatrixViewBasicShader();
		scene::recalculateNormals();
	}

	if (globals::isKeyPressed(GLFW_KEY_D) && checkCollision(globals::MOVE_RIGHT) == false) {
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
		glm::vec3(0.0f, 5.0f, 3.0f),
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
	glClearColor(0.53f, 0.81f, 0.92f, 1.0f);
	glViewport(0, 0, myWindow.getWindowDimensions().width, myWindow.getWindowDimensions().height);

	glEnable(GL_FRAMEBUFFER_SRGB);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	glDisable(GL_CULL_FACE);
}

void initShadows() {
	glGenFramebuffers(1, &depthMapFBO);
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
	             SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
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

int get_terrain_index(int i, int j) {
	return i * terrain_blocks_count + j;
}

void loadBook() {
	book.LoadModel("models/book/book.obj");
	scene::registerModel(book);
}

void loadTerrain() {
	for (int i = 0; i < terrain_blocks_count; i++) {
		for (int j = 0; j < terrain_blocks_count; j++) {
			globals::Model3D terrain_block = globals::Model3D();
			terrain_blocks.push_back(terrain_block);
		}
	}

	for (int i = 0; i < terrain_blocks_count; i++) {
		for (int j = 0; j < terrain_blocks_count; j++) {
			int index = get_terrain_index(i, j);
			globals::Model3D& terrain_block = terrain_blocks[index];
			terrain_block.LoadModel("models/terrain/terrain.obj");
			scene::registerModel(terrain_block);
		}
	}
}

void loadShrooms() {
	for (int i = 0; i < shrooms_count; i++) {
		globals::Model3D shroom = globals::Model3D();
		shrooms.push_back(shroom);
	}
	for (int i = 0; i < shrooms_count; i++) {
		globals::Model3D& shroom = shrooms[i];
		int random_shroom_type = shrooms_types_start + rand() % (shrooms_types_end - shrooms_types_start + 1);
		shroom.LoadModel("models/environment/shroom" + std::to_string(random_shroom_type) + ".obj");
		scene::registerModel(shroom);
	}
}

void loadHouse1() {
	house1.LoadModel("models/environment/house1.obj");
	scene::registerModel(house1);
}

void loadModels() {
	loadBook();
	loadTerrain();
	loadShrooms();
	loadHouse1();
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

void initBook() {
	glm::mat4 book_model_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, book_default_y, 0.0f));
	book.setModelMatrix(book_model_matrix);
	book.initializeBoundingBoxes();
	book.bindBoundingBoxesGPU();
	book.calculateBoundingBoxes();
	terrain_binding_book.i = 2;
	terrain_binding_book.j = 2;
}

void initTerrain() {
	for (int i = 0; i < terrain_blocks_count; i++) {
		for (int j = 0; j < terrain_blocks_count; j++) {
			int index = get_terrain_index(i, j);
			globals::Model3D& terrain_block = terrain_blocks[index];

			float terrain_block_y = 0.0f;
			float terrain_block_z = i * terrain_block_distance_z_axis_i_coord;
			float terrain_block_x = j * terrain_block_distance_x_axis_j_coord;

			glm::mat4 terrain_block_model_matrix = glm::translate(glm::mat4(1.0f),
			                                                      glm::vec3(terrain_block_x, terrain_block_y,
			                                                                terrain_block_z)) *

				glm::scale(glm::mat4(1.0f), glm::vec3(terrain_block_scale));

			terrain_block.setModelMatrix(terrain_block_model_matrix);
			terrain_block.initializeBoundingBoxes();
			terrain_block.bindBoundingBoxesGPU();
			terrain_block.calculateBoundingBoxes();
		}
	}
}

float getRandomFloat(float min, float max) {
	std::random_device rd; // Seed for randomness
	std::mt19937 gen(rd()); // Mersenne Twister engine
	std::uniform_real_distribution<float> dist(min, max);
	return dist(gen);
}

void initShrooms() {
	for (int i = 0; i < shrooms_count; i++) {
		globals::Model3D& shroom = shrooms[i];
		glm::mat4 shroom_model_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 2.0f, 0.0f));
		shroom.setModelMatrix(shroom_model_matrix);
		shroom.initializeBoundingBoxes();
		shroom.bindBoundingBoxesGPU();
		shroom.calculateBoundingBoxes();

		terrainBinding terrain_binding_shroom;
		terrain_binding_shroom.i = rand() % terrain_blocks_count;
		terrain_binding_shroom.j = rand() % terrain_blocks_count;
		shrooms_bindings.push_back(terrain_binding_shroom);


		float z_noise = getRandomFloat(-0.5f * terrain_block_distance_z_axis_i_coord,
		                               0.5f * terrain_block_distance_z_axis_i_coord);
		float x_noise = getRandomFloat(-0.5f * terrain_block_distance_x_axis_j_coord,
		                               0.5f * terrain_block_distance_x_axis_j_coord);
		glm::vec3 noise = glm::vec3(x_noise, 0.0f, z_noise);
		shrooms_random_noise.push_back(noise);

		float y_rotation = getRandomFloat(0.0f, 360.0f);
		float x_rotation = getRandomFloat(0.0f, 7.0f);
		float z_rotation = getRandomFloat(0.0f, 7.0f);
		glm::vec3 rotation = glm::vec3(x_rotation, y_rotation, z_rotation);
		shrooms_random_rotations.push_back(rotation);

		float scale = getRandomFloat(0.75f * shroom_scale, 1.25f * shroom_scale);
		shrooms_random_scales.push_back(scale);
	}
}

void initHouse1() {
	glm::mat4 house1_model_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, house1_default_y, 0.0f));
	house1.setModelMatrix(house1_model_matrix);
	house1.initializeBoundingBoxes();
	house1.bindBoundingBoxesGPU();
	house1.calculateBoundingBoxes();
	terrain_binding_house1.i = 3;
	terrain_binding_house1.j = 2;
}

void initObjectsScene() {
	initBook();
	initShrooms();
	initTerrain();
	initHouse1();

	scene::recalculateNormals();
}


void renderTerrain() {
	globals::Shader& shader = globals::getBasicShader();
	globals_structs::ShaderLocations& shaderLocations = globals::getBasicShaderLocations();
	shader.useShaderProgram();

	for (int i = 0; i < terrain_blocks_count; i++) {
		for (int j = 0; j < terrain_blocks_count; j++) {
			int index = get_terrain_index(i, j);
			globals::Model3D& terrain_block = terrain_blocks[index];

			float terrain_block_y = 0.0f;
			float terrain_block_z = i * terrain_block_distance_z_axis_i_coord;
			float terrain_block_x = j * terrain_block_distance_x_axis_j_coord;

			glm::mat4 terrain_block_model_matrix = glm::translate(glm::mat4(1.0f),
			                                                      glm::vec3(terrain_block_x, terrain_block_y,
			                                                                terrain_block_z)) *
				glm::scale(glm::mat4(1.0f), glm::vec3(terrain_block_scale));

			terrain_block.setModelMatrix(terrain_block_model_matrix);

			// update matrices for this object
			glUniformMatrix4fv(shaderLocations.modelLoc, 1, GL_FALSE,
			                   glm::value_ptr(terrain_block.getModelMatrix()));
			glUniformMatrix3fv(shaderLocations.normalMatrixLoc, 1, GL_FALSE,
			                   glm::value_ptr(terrain_block.getNormalMatrix()));

			terrain_block.calculateBoundingBoxes();
			terrain_block.Draw(shader);
		}
	}
}

void renderBook() {
	globals::Shader& shader = globals::getBasicShader();
	globals_structs::ShaderLocations& shaderLocations = globals::getBasicShaderLocations();
	shader.useShaderProgram();
	float time = glfwGetTime();

	// whatever transformations we want to apply to the teapot
	glm::mat4 transformation_base = glm::scale(glm::mat4(1.0f), glm::vec3(book_scale));
	transformation_base = glm::rotate(glm::mat4(1.0f), glm::radians(time * 10), glm::vec3(0.0f, 1.0f, 0.0f)) *
		transformation_base;

	transformation_base = glm::translate(glm::mat4(1.0f),
	                                     glm::vec3(terrain_block_distance_x_axis_j_coord * terrain_binding_book.j, 0.0f,
	                                               terrain_block_distance_z_axis_i_coord * terrain_binding_book.i)) *
		transformation_base;

	transformation_base = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, book_default_y, 0.0f)) *
		transformation_base;

	transformation_base = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, book_float_variation * sin(time), 0.0f))
		*
		transformation_base;


	book.setModelMatrix(transformation_base);

	// update matrices for this object
	glUniformMatrix4fv(shaderLocations.modelLoc, 1, GL_FALSE, glm::value_ptr(book.getModelMatrix()));
	glUniformMatrix3fv(shaderLocations.normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(book.getNormalMatrix()));

	book.calculateBoundingBoxes();
	book.Draw(shader);
}

void renderShrooms() {
	globals::Shader& shader = globals::getBasicShader();
	globals_structs::ShaderLocations& shaderLocations = globals::getBasicShaderLocations();
	shader.useShaderProgram();

	for (int i = 0; i < shrooms_count; i++) {
		globals::Model3D& shroom = shrooms[i];
		terrainBinding& terrain_binding_shroom = shrooms_bindings[i];
		glm::vec3& noise = shrooms_random_noise[i];
		glm::vec3& rotation = shrooms_random_rotations[i];

		float terrain_block_y = 0.0f;

		float terrain_binding_z = terrain_binding_shroom.i * terrain_block_distance_z_axis_i_coord;
		float terrain_binding_x = terrain_binding_shroom.j * terrain_block_distance_x_axis_j_coord;

		float terrain_block_z = terrain_binding_z + noise.z;
		float terrain_block_x = terrain_binding_x + noise.x;

		float scale = shrooms_random_scales[i];

		// whatever transformations we want to apply to the teapot
		glm::mat4 transformation_base = glm::scale(glm::mat4(1.0f), glm::vec3(scale));
		transformation_base = glm::rotate(glm::mat4(1.0f), glm::radians(rotation.x), glm::vec3(1.0f, 0, 0)) *
			transformation_base;
		transformation_base = glm::rotate(glm::mat4(1.0f), glm::radians(rotation.y), glm::vec3(0, 1.0f, 0)) *
			transformation_base;
		transformation_base = glm::rotate(glm::mat4(1.0f), glm::radians(rotation.z), glm::vec3(0, 0, 1.0f)) *
			transformation_base;
		transformation_base = glm::translate(glm::mat4(1.0f),
		                                     glm::vec3(terrain_block_x, shroom_default_y * scale, terrain_block_z)) *
			transformation_base;

		shroom.setModelMatrix(transformation_base);

		// update matrices for this object
		glUniformMatrix4fv(shaderLocations.modelLoc, 1, GL_FALSE, glm::value_ptr(shroom.getModelMatrix()));
		glUniformMatrix3fv(shaderLocations.normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(shroom.getNormalMatrix()));

		shroom.calculateBoundingBoxes();
		shroom.Draw(shader);
	}
}

void renderHouse1() {
	globals::Shader& shader = globals::getBasicShader();
	globals_structs::ShaderLocations& shaderLocations = globals::getBasicShaderLocations();
	shader.useShaderProgram();

	// whatever transformations we want to apply to the teapot
	glm::mat4 transformation_base = glm::scale(glm::mat4(1.0f), glm::vec3(house1_scale));
	transformation_base = glm::rotate(glm::mat4(1.0f), glm::radians(135.0f), glm::vec3(0.0f, 1.0f, 0.0f)) *
		transformation_base;
	transformation_base = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, house1_default_y, 0.0f)) *
		transformation_base;
	transformation_base = glm::translate(glm::mat4(1.0f),
	                                     glm::vec3(terrain_block_distance_x_axis_j_coord * terrain_binding_house1.j,
	                                               0.0f,
	                                               terrain_block_distance_z_axis_i_coord * terrain_binding_house1.i)) *
		transformation_base;

	house1.setModelMatrix(transformation_base);

	// update matrices for this object
	glUniformMatrix4fv(shaderLocations.modelLoc, 1, GL_FALSE, glm::value_ptr(house1.getModelMatrix()));
	glUniformMatrix3fv(shaderLocations.normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(house1.getNormalMatrix()));

	house1.calculateBoundingBoxes();
	house1.Draw(shader);
}

void renderSceneOriginal() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	renderTerrain();
	renderShrooms();
	renderBook();
	renderHouse1();

	if (globals_configs::getWireframeMode())
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

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
	bindWindowAndKeyboardCallbacks();

	glCheckError();

	float lastFrameTime = glfwGetTime();
	globals::Window& myWindow = globals::getWindow();

	while (!glfwWindowShouldClose(myWindow.getWindow())) {
		float currentFrameTime = glfwGetTime();
		float deltaTime = currentFrameTime - lastFrameTime;
		lastFrameTime = currentFrameTime;

		processCameraMovement();
		cameraAnimation::updateCameraAnimation(deltaTime);
		renderSceneOriginal();

		glfwPollEvents();
		glfwSwapBuffers(myWindow.getWindow());

		glCheckError();
	}

	// Cleanup
	cleanup();

	return EXIT_SUCCESS;
}

