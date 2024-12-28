#include "Lights.h"
#include "GlobalObjects.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/glm.hpp>

namespace lights
{
	void initLights() {
		globals::Shader& myBasicShader = globals::getBasicShader();
		globals_structs::ShaderLocationsBasic& shaderLocations = globals::getBasicShaderLocations();

		glm::vec3 lightDirDir = glm::normalize(glm::vec3(0.5f, -1.0f, 0.2f));
		glm::vec3 rotationAxis = glm::normalize(glm::vec3(0.0f, 0.398f, 0.917f));
		glm::vec3 lightDirColor = glm::vec3(1.0f, 1.0f, 1.0f);


		glm::vec3 lightPointLoc = glm::vec3(10.0f, 1.0f, 16.0f);
		glm::vec3 lightPointColor = glm::vec3(1.0f, 1.0f, 1.0f);

		globals::setLightPointLoc(lightPointLoc);
		globals::setLightPointColor(lightPointColor);
		globals::setLightDirColor(lightDirColor);
		globals::setLightDirDir(lightDirDir);
		globals::setLightDirRotationAxis(rotationAxis);

		globals::Camera& myCamera = globals::getCamera();
		glm::mat4 view = myCamera.getViewMatrix();
		glUniform3fv(shaderLocations.lightDirDir, 1,
		             glm::value_ptr(lightDirDir));
		glUniform3fv(shaderLocations.lightDirColor, 1, glm::value_ptr(lightDirColor));

		glUniform3fv(shaderLocations.lightPointLoc, 1, glm::value_ptr(lightPointLoc));
		glUniform3fv(shaderLocations.lightPointColor, 1, glm::value_ptr(lightPointColor));
	}
}
