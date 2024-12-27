#ifndef SCENE_H
#define SCENE_H
#include "Model3D.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <vector>
#include "glm/glm.hpp"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp> //core glm functionality
#include <glm/gtc/matrix_transform.hpp> //glm extension for generating common transformation matrices
#include <glm/gtc/matrix_inverse.hpp> //glm extension for computing inverse matrices
#include <glm/gtc/type_ptr.hpp> //glm extension for accessing the internal data structure of glm types
#include <glm/gtx/string_cast.hpp>

namespace scene
{
	std::vector<globals::Model3D*>& getModels3D();

	void loadBook();
	void loadTerrain();
	void loadShrooms();
	void loadHouse1();

	void initTerrain();
	void initShrooms();
	void initBook();
	void initHouse1();

	void renderTerrain(bool depth = false);
	void renderShrooms(bool depth = false);
	void renderBook(bool depth = false);
	void renderHouse1(bool depth = false);

	glm::mat4 calculateLightSpaceMatrix();
}

#endif //SCENE_H
