#include "BoundingBox.h"
#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

#include "GlobalObjects.h"

namespace globals
{
	void BoundingBox3D::initializeBoundingBoxGPU() {
		glGenVertexArrays(1, &boundingBoxVAO);
		glGenBuffers(1, &boundingBoxVBO);
		glGenBuffers(1, &boundingBoxEBO);

		glBindVertexArray(boundingBoxVAO);

		// Setup VBO for vertices
		glBindBuffer(GL_ARRAY_BUFFER, boundingBoxVBO);
		glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(glm::vec3), nullptr, GL_DYNAMIC_DRAW);

		// Setup EBO for indices
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, boundingBoxEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(boxIndices), boxIndices, GL_STATIC_DRAW);

		// Configure vertex attributes
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
		glEnableVertexAttribArray(0);

		glBindVertexArray(0);
	}

	void BoundingBox3D::Draw() {
		Shader& shader = globals::getBasicShader();
		shader.useShaderProgram();

		glm::mat4 modelMatrix = glm::mat4(1.0f);

		glUniformMatrix4fv(globals::getBasicShaderLocations().modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));

		glBindVertexArray(boundingBoxVAO);
		glDrawElements(GL_LINES, sizeof(boxIndices) / sizeof(unsigned int), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}

	void BoundingBox3D::setBoundingBoxCorners(std::vector<glm::vec3> cube_vertices) {
		if (cube_vertices.size() != 8) {
			std::cerr << "Invalid number of corners for bounding box" << std::endl;
			return;
		}
		glm::vec3 vertices[] = {
			{cube_vertices[0].x, cube_vertices[0].y, cube_vertices[0].z},
			{cube_vertices[1].x, cube_vertices[1].y, cube_vertices[1].z},
			{cube_vertices[2].x, cube_vertices[2].y, cube_vertices[2].z},
			{cube_vertices[3].x, cube_vertices[3].y, cube_vertices[3].z},
			{cube_vertices[4].x, cube_vertices[4].y, cube_vertices[4].z},
			{cube_vertices[5].x, cube_vertices[5].y, cube_vertices[5].z},
			{cube_vertices[6].x, cube_vertices[6].y, cube_vertices[6].z},
			{cube_vertices[7].x, cube_vertices[7].y, cube_vertices[7].z}
		};
		for (int i = 0; i < 8; i++) {
			corners[i] = vertices[i];
		}

		glBindBuffer(GL_ARRAY_BUFFER, boundingBoxVBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	bool BoundingBox3D::checkCollision(glm::vec3 point) {
		glm::vec3 localPoint = point;

		glm::vec3 axes[3] = {
			glm::normalize(corners[1] - corners[0]),
			glm::normalize(corners[3] - corners[0]),
			glm::normalize(corners[4] - corners[0])
		};

		for (int i = 0; i < 3; ++i) {
			float minProj = std::numeric_limits<float>::max();
			float maxProj = std::numeric_limits<float>::lowest();
			for (int j = 0; j < 8; ++j) {
				float proj = glm::dot(axes[i], corners[j]);
				minProj = std::min(minProj, proj);
				maxProj = std::max(maxProj, proj);
			}

			float pointProj = glm::dot(axes[i], localPoint);

			if (pointProj < minProj || pointProj > maxProj) {
				return false;
			}
		}

		return true;
	}

	BoundingBox3D::~BoundingBox3D() {
		glDeleteVertexArrays(1, &boundingBoxVAO);
		glDeleteBuffers(1, &boundingBoxVBO);
		glDeleteBuffers(1, &boundingBoxEBO);
	}
}
