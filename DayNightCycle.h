//
// Created by eugen on 12/27/2024.
//

#ifndef DAYNIGHTCYCLE_H
#define DAYNIGHTCYCLE_H
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/glm.hpp"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp> //core glm functionality
#include <glm/gtc/matrix_transform.hpp> //glm extension for generating common transformation matrices
#include <glm/gtc/matrix_inverse.hpp> //glm extension for computing inverse matrices
#include <glm/gtc/type_ptr.hpp> //glm extension for accessing the internal data structure of glm types
#include <glm/gtx/string_cast.hpp>

namespace day_night_cycle
{
	void handleLightsDayNightCycle();
}

#endif //DAYNIGHTCYCLE_H
