#include "CameraAnimation.h"

#include "Bindings.h"
#include "GlobalObjects.h"
#include "pure_functions.h"

namespace cameraAnimation
{
	glm::vec3 focusPoint(0.0f, 0.0f, 0.0f);
	glm::vec3 initialPosition;
	glm::vec3 forwardDirection;

	bool isAnimating = false;
	float animationProgress = 0.0f;
	float duration = 2.0f;

	void startCameraAnimation(glm::vec3 initialPosition, glm::vec3 focusPoint) {
		cameraAnimation::isAnimating = true;
		cameraAnimation::animationProgress = 0.0f;
		cameraAnimation::initialPosition = initialPosition;
		cameraAnimation::focusPoint = focusPoint;

		cameraAnimation::forwardDirection = glm::normalize(focusPoint - initialPosition);
	}

	void updateCameraAnimation(float deltaTime) {
		if (!isAnimating) return;
		globals::Camera& myCamera = globals::getCamera();

		animationProgress += deltaTime / duration;

		if (animationProgress >= 1.0f) {
			animationProgress = 1.0f;
			isAnimating = false;
		}

		float distance = sin(glm::radians(180.0f * animationProgress)) * 10.0f;
		glm::vec3 newPosition;
		float distance_x = distance * -forwardDirection.x;
		float distance_z = distance * -forwardDirection.z;
		float distance_y = distance * -forwardDirection.y;

		newPosition.x = initialPosition.x + distance_x;
		newPosition.z = initialPosition.z + distance_z;
		newPosition.y = initialPosition.y + distance_y;

		myCamera.setPosition(newPosition, focusPoint);

		setMatrixViewBasicShader();
	}
}
