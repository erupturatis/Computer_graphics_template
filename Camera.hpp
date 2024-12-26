#ifndef Camera_hpp
#define Camera_hpp

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

namespace globals
{
    enum MOVE_DIRECTION { MOVE_FORWARD, MOVE_BACKWARD, MOVE_RIGHT, MOVE_LEFT };

    class Camera
    {
    public:
        Camera() = default;

        void initializeCamera(glm::vec3 cameraPosition, glm::vec3 cameraTarget, glm::vec3 cameraUp);
        glm::mat4 getViewMatrix();
        void move(MOVE_DIRECTION direction, float speed);
        void rotate(float pitch, float yaw);
        void setPosition(const glm::vec3& position, const glm::vec3& target);
        void setTarget(const glm::vec3& target);
        glm::vec3 getTarget();

        void updatePitchYawFromTarget();
        glm::vec3 getPosition();
        glm::vec3 getNextPosition(MOVE_DIRECTION direction, float speed);

    private:
        glm::vec3 cameraPosition;
        glm::vec3 cameraTarget;
        glm::vec3 cameraFrontDirection;
        glm::vec3 cameraRightDirection;
        glm::vec3 cameraUpDirection;
        float currentPitch;
        float currentYaw;
    };
}

#endif /* Camera_hpp */
