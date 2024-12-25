#ifndef BOUNDINGBOXES_H
#define BOUNDINGBOXES_H

#include "Mesh.hpp"

namespace globals
{
	void initializeBoundingBoxGPU();

	class BoundingBox3D
	{
	public:
		~BoundingBox3D();

		void setBoundingBox(glm::vec3 min, glm::vec3 max);

		void Draw();

	private:
		globals::Vertex min, max;
	};
}

#endif
