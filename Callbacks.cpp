#include "Callbacks.h"
#include <cstdio>

#include "Bindings.h"
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

	if (key == GLFW_KEY_B && action == GLFW_PRESS) {
		bool currentSetting = globals_configs::getShowBoundingBoxes();
		globals_configs::setShowBoundingBoxes(!currentSetting);
	}

	if (key == GLFW_KEY_C && action == GLFW_PRESS) {
		bool currentSetting = globals_configs::getWireframeMode();
		globals_configs::setWireframeMode(!currentSetting);
	}

	if (key == GLFW_KEY_X && action == GLFW_PRESS) {
		bool currentSetting = globals_configs::getAcceleratedDayNightCycle();
		globals_configs::setAcceleratedDayNightCycle(!currentSetting);
	}

	if (key == GLFW_KEY_F && action == GLFW_PRESS) {
		bool currentSetting = globals_configs::getApplyFog();
		globals_configs::setApplyFog(!currentSetting);
	}

	globals::registerKeyPress(key, action);
}

