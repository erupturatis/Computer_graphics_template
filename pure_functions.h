#ifndef PURE_FUNCTIONS_H
#define PURE_FUNCTIONS_H

#include "BoundingBox.h"
#include "glm/glm.hpp"

namespace globals
{
	std::pair<glm::vec3, glm::vec3> calculateBoundingBox(const std::vector<globals::Vertex>& vertices);

	float calculateVerticalAngle(const glm::vec3& forwardDir);
}

#endif
