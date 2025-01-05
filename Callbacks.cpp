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
		bool wireframe = globals_configs::getWireframeMode();
		bool vertex = globals_configs::getVertexMode();

		if (wireframe == false && vertex == false) {
			globals_configs::setWireframeMode(true);
			globals_configs::setVertexMode(false);
		}
		else if (wireframe == true && vertex == false) {
			globals_configs::setWireframeMode(false);
			globals_configs::setVertexMode(true);
		}
		else if (wireframe == false && vertex == true) {
			globals_configs::setWireframeMode(false);
			globals_configs::setVertexMode(false);
		}
	}

	if (key == GLFW_KEY_X && action == GLFW_PRESS) {
		bool currentSetting = globals_configs::getAcceleratedDayNightCycle();
		globals_configs::setAcceleratedDayNightCycle(!currentSetting);
	}

	if (key == GLFW_KEY_F && action == GLFW_PRESS) {
		bool currentSetting = globals_configs::getApplyFog();
		globals_configs::setApplyFog(!currentSetting);
	}
	if (key == GLFW_KEY_Z && action == GLFW_PRESS) {
		bool currentSetting = globals_configs::getApplyCollisions();
		globals_configs::setApplyCollisions(!currentSetting);
	}

	globals::registerKeyPress(key, action);
}

