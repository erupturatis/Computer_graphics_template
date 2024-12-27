#ifndef PURE_FUNCTIONS_H
#define PURE_FUNCTIONS_H

#include "BoundingBox.h"
#include "glm/glm.hpp"

namespace globals
{
	std::pair<glm::vec3, glm::vec3> calculateBoundingBox(const std::vector<globals::Vertex>& vertices);

	float calculateVerticalAngle(const glm::vec3& forwardDir);

	float getRandomFloat(float min, float max);

	int get_terrain_index(int i, int j, int terrain_blocks_count);

	bool hasNaN(const glm::vec3& vec);
}

#endif
