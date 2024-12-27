#include "Lights.h"
#include "GlobalObjects.h"
#include <glm/gtc/type_ptr.hpp>

namespace lights
{
	glm::vec3 lightDirDir;
	glm::vec3 lightDirColor;

	glm::vec3 lightPointLoc;
	glm::vec3 lightPointColor;

	void initLights() {
		globals::Shader& myBasicShader = globals::getBasicShader();
		globals_structs::ShaderLocationsBasic& shaderLocations = globals::getBasicShaderLocations();

		lightDirDir = glm::vec3(0.0f, 1.0f, 1.0f);
		lightDirColor = glm::vec3(1.0f, 1.0f, 1.0f);

		lightPointLoc = glm::vec3(10.0f, 1.0f, 16.0f);
		lightPointColor = glm::vec3(1.0f, 1.0f, 1.0f);

		glUniform3fv(shaderLocations.lightDirDir, 1, glm::value_ptr(lightDirDir));
		glUniform3fv(shaderLocations.lightDirColor, 1, glm::value_ptr(lightDirColor));

		glUniform3fv(shaderLocations.lightPointLoc, 1, glm::value_ptr(lightPointLoc));
		glUniform3fv(shaderLocations.lightPointColor, 1, glm::value_ptr(lightPointColor));
	}
}
