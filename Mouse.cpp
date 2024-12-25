#include "Bindings.h"
#include "Mouse.h"
#include "GlobalObjects.h"

namespace mouse
{
	float lastX = 400, lastY = 300;
	bool firstMouse = true;

	void callback(GLFWwindow* window, double xpos, double ypos) {
		if (firstMouse) {
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
		}

		globals::Camera& myCamera = globals::getCamera();

		float xOffset = xpos - lastX;
		float yOffset = lastY - ypos;

		lastX = xpos;
		lastY = ypos;

		float sensitivity = 0.1f;
		xOffset *= sensitivity;
		yOffset *= sensitivity;

		myCamera.rotate(yOffset, xOffset);

		globals::getBasicShader().useShaderProgram();
		setMatrixViewBasicShader();
	}
}
