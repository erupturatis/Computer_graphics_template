#ifndef BOUNDINGBOXES_H
#define BOUNDINGBOXES_H

#include "Mesh.hpp"

namespace globals
{
	class BoundingBox3D
	{
	public:
		~BoundingBox3D();

		void initializeBoundingBoxGPU();
		void setBoundingBox(glm::vec3 min, glm::vec3 max);
		void setBoundingBoxCorners(std::vector<glm::vec3> vertices);
		bool checkCollision(glm::vec3);

		void Draw();

	private:
		globals::Vertex min, max;
		unsigned int boundingBoxVAO, boundingBoxVBO, boundingBoxEBO;

		const unsigned int boxIndices[24] = {
			0, 1, 1, 2, 2, 3, 3, 0,
			4, 5, 5, 6, 6, 7, 7, 4,
			0, 4, 1, 5, 2, 6, 3, 7
		};

		glm::vec3 corners[8];
	};
}

#endif
