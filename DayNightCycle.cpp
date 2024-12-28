//
// Created by eugen on 12/27/2024.
//

#include "DayNightCycle.h"

#include "CommonGroupings.h"
#include "GlobalConfigs.h"
#include "GlobalObjects.h"
#include "pure_functions.h"


namespace day_night_cycle
{
	float dayNightSpeed = 0.01f;
	float currentTime = 0.0f;
	float lastglfwTime = 0.0f;

	void handleLightsDayNightCycle() {
		globals_structs::ShaderLocationsBasic& shaderLocations = globals::getBasicShaderLocations();

		if (lastglfwTime == 0.0f) {
			lastglfwTime = glfwGetTime();
		}

		float time = glfwGetTime();
		float deltaTime = time - lastglfwTime;
		currentTime += deltaTime * (globals_configs::getAcceleratedDayNightCycle()
			                            ? dayNightSpeed * 25
			                            : dayNightSpeed);
		lastglfwTime = time;
		float normalizedTime = fmod(currentTime, 1.5f);

		float middayDuration = 0.5f;
		float middayToNightDuration = 0.25f;
		float nightDuration = 0.5f;
		float nightToMiddayDuration = 0.25f;

		float middayEnd = middayDuration;
		float nightStart = middayEnd + middayToNightDuration;
		float nightEnd = nightStart + nightDuration;
		float cycleEnd = nightEnd + nightToMiddayDuration;

		glm::vec3 middayColor(1.0f, 1.0f, 0.8f);
		glm::vec3 transitionColor(0.4f, 0.2f, 0.6f);
		glm::vec3 nightColor(0.1f, 0.1f, 0.3f);

		glm::vec3 backgroundDayColor(0.9f, 0.9f, 1.0f);
		glm::vec3 backgroundNightColor(0.05f, 0.05f, 0.2f);

		glm::vec3 lightDirColor;
		glm::vec3 backgroundColor;

		if (normalizedTime < middayEnd) {
			lightDirColor = middayColor;
			backgroundColor = backgroundDayColor;
		}
		else if (normalizedTime < nightStart) {
			float t = (normalizedTime - middayEnd) / middayToNightDuration;
			lightDirColor = glm::mix(middayColor, nightColor, t);
			backgroundColor = glm::mix(backgroundDayColor, backgroundNightColor, t);
		}
		else if (normalizedTime < nightEnd) {
			lightDirColor = nightColor;
			backgroundColor = backgroundNightColor;
		}
		else {
			float t = (normalizedTime - nightEnd) / nightToMiddayDuration;
			lightDirColor = glm::mix(nightColor, middayColor, t);
			backgroundColor = glm::mix(backgroundNightColor, backgroundDayColor, t);
		}

		float cycle = normalizedTime / 1.5f * 360.0f;

		globals::setLightDirColor(lightDirColor);
		globals::setLightDirRotationAngle(cycle + 90.0f);

		glClearColor(backgroundColor.x, backgroundColor.y, backgroundColor.z, 1.0f);
	}
}
