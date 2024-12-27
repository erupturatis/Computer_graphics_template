#ifndef COLLISIONS_H
#define COLLISIONS_H

#include <glm/glm.hpp>

#include "Camera.hpp"
#include "GlobalObjects.h"
#include "GlobalConfigs.h"
#include "Scene.h"
#include "Model3D.hpp"


bool checkCollision(globals::MOVE_DIRECTION direction);

#endif //COLLISIONS_H
