#include "Bindings.h"
#include "Callbacks.h"
#include "Mouse.h"
#include "glm/glm.hpp"
#include <glm/gtc/type_ptr.hpp>

void bindWindowCallbacks() {
	globals::Window& myWindow = globals::getWindow();

	glfwSetWindowSizeCallback(myWindow.getWindow(), windowResizeCallback);
	glfwSetKeyCallback(myWindow.getWindow(), keyboardCallback);
	glfwSetCursorPosCallback(myWindow.getWindow(), mouse::callback);
	glfwSetInputMode(myWindow.getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void setMatrixViewBasicShader() {
	globals::getBasicShader().useShaderProgram();

	globals::Camera& myCamera = globals::getCamera();
	globals_structs::ShaderLocations& shaderLocations = globals::getBasicShaderLocations();

	glm::mat4 view = myCamera.getViewMatrix();
	glUniformMatrix4fv(shaderLocations.viewLoc, 1, GL_FALSE, glm::value_ptr(view));
}

void setMatrixProjectionBasicShader() {
	globals::getBasicShader().useShaderProgram();

	globals_structs::MatricesShaderCommon& matrices = globals::getMatrices();
	globals_structs::ShaderLocations& shaderLocations = globals::getBasicShaderLocations();
	globals::Window& window = globals::getWindow();
	glm::mat4 projection = glm::perspective(glm::radians(45.0f),
	                                        (float)window.getWindowDimensions().width / (float)window.
	                                        getWindowDimensions().height,
	                                        0.01f, 100.0f);

	glUniformMatrix4fv(shaderLocations.projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
}
