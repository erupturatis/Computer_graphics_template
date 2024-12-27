#include "Collisions.h"

bool checkCollision(globals::MOVE_DIRECTION direction) {
	std::vector<globals::Model3D*>& models = scene::getModels3D();
	globals::Camera& myCamera = globals::getCamera();
	glm::vec3 expectedNextPosition = myCamera.getNextPosition(direction, globals_configs::getCameraSpeed());

	for (globals::Model3D* model : models) {
		std::vector<globals::BoundingBox3D>& boundingBoxes = model->getBoundingBoxes();

		for (globals::BoundingBox3D& boundingBox : boundingBoxes) {
			if (boundingBox.checkCollision(expectedNextPosition)) {
				return true;
			}
		}
	}

	return false;
}

