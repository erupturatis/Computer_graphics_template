#include "BoundingBoxes.h"

namespace gps
{
	void BoundingBox3D::Draw(gps::Shader shaderProgram) {
          // TODO
	}

    void BoundingBox3D::setBoundingBox(glm::vec3 min, glm::vec3 max) {
		this->min.Position = min;
		this->max.Position = max;
	}

	BoundingBox3D::~BoundingBox3D() {
          // nothing
    }
}
