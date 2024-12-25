#ifndef BOUNDINGBOXES_H
#define BOUNDINGBOXES_H

#include "Mesh.hpp"

namespace globals
{
	class BoundingBox3D
	{
	public:
		~BoundingBox3D();

		void setBoundingBox(glm::vec3 min, glm::vec3 max);

		void Draw(globals::Shader shaderProgram);

	private:
		globals::Vertex min, max;
	};
}

#endif
