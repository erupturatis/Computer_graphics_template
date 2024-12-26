#include "pure_functions.h"
#include <bits/stdc++.h>

namespace globals
{
    std::pair<glm::vec3, glm::vec3> calculateBoundingBox(const std::vector<globals::Vertex>& vertices) {
        if (vertices.empty()) {
            std::cerr << "No vertices to compute bounding box" << std::endl;
            return std::make_pair(glm::vec3(0.0f), glm::vec3(0.0f));
        }

        glm::vec3 min = vertices[0].Position;
        glm::vec3 max = vertices[0].Position;

        for (size_t i = 1; i < vertices.size(); ++i) {
            glm::vec3 vertex = vertices[i].Position;
            min = glm::vec3(
                std::min(min.x, vertex.x),
                std::min(min.y, vertex.y),
                std::min(min.z, vertex.z)
            );
            max = glm::vec3(
                std::max(max.x, vertex.x),
                std::max(max.y, vertex.y),
                std::max(max.z, vertex.z)
            );
        }

        return std::make_pair(min, max);
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
