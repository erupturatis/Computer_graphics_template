#include "Camera.hpp"

namespace gps
{
    Camera::Camera(glm::vec3 cameraPosition, glm::vec3 cameraTarget, glm::vec3 cameraUp) {
        this->cameraPosition = cameraPosition;
        this->cameraTarget = cameraTarget;
        this->cameraUpDirection = cameraUp;

        currentPitch = 0.0f;
        currentYaw = -90.0f;
    }

    glm::mat4 Camera::getViewMatrix() {
        //TODO
        return glm::lookAt(cameraPosition, cameraTarget, this->cameraUpDirection);
    }

    void Camera::move(MOVE_DIRECTION direction, float speed) {
        glm::vec3 cameraDirection = glm::normalize(cameraTarget - cameraPosition);
        glm::vec3 cameraRight = glm::normalize(glm::cross(cameraDirection, cameraUpDirection));

        switch (direction) {
        case MOVE_FORWARD:
            cameraPosition += cameraDirection * speed;
            cameraTarget += cameraDirection * speed;
            break;
        case MOVE_BACKWARD:
            cameraPosition -= cameraDirection * speed;
            cameraTarget -= cameraDirection * speed;
            break;
        case MOVE_LEFT:
            cameraPosition -= cameraRight * speed;
            cameraTarget -= cameraRight * speed;
            break;
        case MOVE_RIGHT:
            cameraPosition += cameraRight * speed;
            cameraTarget += cameraRight * speed;
            break;
        }
    }

    void Camera::updatePitchYawFromTarget() {
        glm::vec3 direction = glm::normalize(cameraTarget - cameraPosition);
        currentYaw = glm::degrees(atan2(direction.z, direction.x));
        currentPitch = glm::degrees(asin(direction.y));
    }

    //update the camera internal parameters following a camera rotate event
    //yaw - camera rotation around the y axis
    //pitch - camera rotation around the x axis
    void Camera::rotate(float pitch, float yaw) {
        updatePitchYawFromTarget();
        currentPitch += pitch;
        currentYaw += yaw;

        // Constrain the pitch to prevent screen flip
        currentPitch = glm::clamp(currentPitch, -89.0f, 89.0f);

        glm::vec3 direction;
        direction.x = cos(glm::radians(currentPitch)) * cos(glm::radians(currentYaw));
        direction.y = sin(glm::radians(currentPitch));
        direction.z = cos(glm::radians(currentPitch)) * sin(glm::radians(currentYaw));

        cameraFrontDirection = glm::normalize(direction);
        cameraTarget = cameraPosition + cameraFrontDirection;
    }

    void Camera::setPosition(const glm::vec3& position, const glm::vec3& target) {
        cameraPosition = position;
        cameraTarget = target;
        // updatePitchYawFromTarget();
    }

    glm::vec3 Camera::getTarget() {
        return cameraTarget;
    }

    glm::vec3 Camera::getPosition() {
        return cameraPosition;
    }
}
