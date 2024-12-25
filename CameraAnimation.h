#ifndef CAMERAANIMATION_H
#define CAMERAANIMATION_H

#include "glm/gtc/matrix_transform.hpp"
#include "Camera.hpp"

namespace cameraAnimation {
    void updateCameraAnimation(float deltaTime);
    void startCameraAnimation(glm::vec3 initialPosition, glm::vec3 focusPoint);
    void stopCameraAnimation();
}

#endif
