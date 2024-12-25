#ifndef PURE_FUNCTIONS_H
#define PURE_FUNCTIONS_H

#include "BoundingBoxes.h"
#include "glm/glm.hpp"

namespace globals
{
	BoundingBox3D computeBoundingBox(const std::vector<globals::Vertex>& vertices);

	float calculateVerticalAngle(const glm::vec3& forwardDir);
}

#endif
