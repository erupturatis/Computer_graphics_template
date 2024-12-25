#ifndef BOUNDINGBOXES_H
#define BOUNDINGBOXES_H

#include "Mesh.hpp"
#include "tiny_obj_loader.h"
#include "stb_image.h"

#include <string>
#include <vector>

namespace gps
{
	class BoundingBox3D
	{
	public:
		~BoundingBox3D();

		void setBoundingBox(glm::vec3 min, glm::vec3 max);

		void Draw(gps::Shader shaderProgram);

	private:
		gps::Vertex min, max;
	};
}

#endif
