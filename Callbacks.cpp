#include "Callbacks.h"
#include <cstdio>
#include "CameraAnimation.h"
#include "GlobalObjects.h"


void windowResizeCallback(GLFWwindow* window, int width, int height) {
	fprintf(stdout, "Window resized! New width: %d , and height: %d\n", width, height);
	//TODO
}

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
	globals::Camera myCamera = globals::getCamera();

	if (key == GLFW_KEY_V && action == GLFW_PRESS) {
		cameraAnimation::startCameraAnimation(myCamera.getPosition(), myCamera.getTarget());
	}

	globals::registerKeyPress(key, action);
}

