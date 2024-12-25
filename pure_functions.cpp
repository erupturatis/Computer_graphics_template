#include "pure_functions.h"

gps::BoundingBox3D computeBoundingBox(const std::vector<gps::Vertex>& vertices) {
    if (vertices.empty()) {
        gps::BoundingBox3D box =  gps::BoundingBox3D();
        box.setBoundingBox(glm::vec3(0.0f), glm::vec3(0.0f));
    }

    glm::vec3 min = vertices[0].Position;
    glm::vec3 max = min;

    for (size_t i = 1; i < vertices.size(); ++i) {
        glm::vec3 vertex = vertices[i].Position;
        min = glm::min(min, vertex);
        max = glm::max(max, vertex);
    }

    gps::BoundingBox3D box =  gps::BoundingBox3D();
    box.setBoundingBox(min, max);
    return box;
}
