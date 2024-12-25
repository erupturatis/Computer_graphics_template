#include "pure_functions.h"

namespace globals
{
    BoundingBox3D computeBoundingBox(const std::vector<globals::Vertex>& vertices) {
        if (vertices.empty()) {
            globals::BoundingBox3D box = globals::BoundingBox3D();
            box.setBoundingBox(glm::vec3(0.0f), glm::vec3(0.0f));
        }

        glm::vec3 min = vertices[0].Position;
        glm::vec3 max = min;

        for (size_t i = 1; i < vertices.size(); ++i) {
            glm::vec3 vertex = vertices[i].Position;
            min = glm::min(min, vertex);
            max = glm::max(max, vertex);
        }

        globals::BoundingBox3D box = globals::BoundingBox3D();
        box.setBoundingBox(min, max);
        return box;
    }

    float calculateVerticalAngle(const glm::vec3& forwardDir) {
        glm::vec3 normalizedForward = glm::normalize(forwardDir);
        glm::vec3 forwardXZ = glm::normalize(glm::vec3(forwardDir.x, 0.0f, forwardDir.z));
        float angle = glm::degrees(glm::acos(glm::dot(normalizedForward, forwardXZ)));
        if (forwardDir.y < 0) {
            angle = -angle;
        }
        return angle;
    }
}
