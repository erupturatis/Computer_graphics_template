#include "BoundingBoxes.h"
#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "GlobalObjects.h"

namespace globals
{
	unsigned int boundingBoxVAO, boundingBoxVBO, boundingBoxEBO;

	// Bounding box edges
	const unsigned int boxIndices[] = {
		0, 1, 1, 2, 2, 3, 3, 0,
		4, 5, 5, 6, 6, 7, 7, 4,
		0, 4, 1, 5, 2, 6, 3, 7
	};

	void initializeBoundingBoxGPU() {
		glGenVertexArrays(1, &boundingBoxVAO);
		glGenBuffers(1, &boundingBoxVBO);
		glGenBuffers(1, &boundingBoxEBO);

		glBindVertexArray(boundingBoxVAO);

		// Setup VBO for vertices
		glBindBuffer(GL_ARRAY_BUFFER, boundingBoxVBO);
		glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(glm::vec3), nullptr, GL_DYNAMIC_DRAW); // Placeholder for vertices

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
		glm::mat4 translation = glm::translate(modelMatrix, (min.Position + max.Position) / 2.0f);
		glm::mat4 scaling = glm::scale(modelMatrix, max.Position - min.Position);

		modelMatrix = translation * scaling;

		glUniformMatrix4fv(globals::getBasicShaderLocations().modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));

		std::cout << "drawing box";

		glBindVertexArray(boundingBoxVAO);
		glDrawElements(GL_LINES, sizeof(boxIndices) / sizeof(unsigned int), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}

	void BoundingBox3D::setBoundingBox(glm::vec3 min, glm::vec3 max) {
		this->min.Position = min;
		this->max.Position = max;

		glm::vec3 vertices[] = {
			{min.x, min.y, min.z},
			{max.x, min.y, min.z},
			{max.x, max.y, min.z},
			{min.x, max.y, min.z},
			{min.x, min.y, max.z},
			{max.x, min.y, max.z},
			{max.x, max.y, max.z},
			{min.x, max.y, max.z}
		};

		glBindBuffer(GL_ARRAY_BUFFER, boundingBoxVBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	BoundingBox3D::~BoundingBox3D() {
		glDeleteVertexArrays(1, &boundingBoxVAO);
		glDeleteBuffers(1, &boundingBoxVBO);
		glDeleteBuffers(1, &boundingBoxEBO);
	}
}

// float cubeVertices[] = {
// 	-0.5f, -0.5f, -0.5f,
// 	0.5f, -0.5f, -0.5f,
// 	0.5f, 0.5f, -0.5f,
// 	-0.5f, 0.5f, -0.5f,
// 	-0.5f, -0.5f, 0.5f,
// 	0.5f, -0.5f, 0.5f,
// 	0.5f, 0.5f, 0.5f,
// 	-0.5f, 0.5f, 0.5f
// };
//
// unsigned int cubeIndices[] = {
// 	0, 1, 1, 2, 2, 3, 3, 0, // Bottom square
// 	4, 5, 5, 6, 6, 7, 7, 4, // Top square
// 	0, 4, 1, 5, 2, 6, 3, 7 // Connecting edges
// };
//
// unsigned int cubeVAO, cubeVBO, cubeEBO;
// unsigned int cubeShaderProgram;
//
// void CubeDataInitializeAndBind() {
// 	glGenVertexArrays(1, &cubeVAO);
// 	glGenBuffers(1, &cubeVBO);
// 	glGenBuffers(1, &cubeEBO);
//
// 	glBindVertexArray(cubeVAO);
//
// 	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
// 	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
//
// 	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);
// 	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), cubeIndices, GL_STATIC_DRAW);
//
// 	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
// 	glEnableVertexAttribArray(0);
//
// 	glBindVertexArray(0);
// }
//
// void renderCube(pure::Shader shader) {
// 	shader.useShaderProgram();
//
// 	float time = glfwGetTime();
// 	glm::mat4 transformation_rotate = glm::rotate(glm::mat4(1.0f), time, glm::vec3(0.5f, 1.0f, 0.0f));
// 	glUniformMatrix4fv(shaderLocations.modelLoc, 1, GL_FALSE, glm::value_ptr(transformation_rotate));
//
// 	glBindVertexArray(cubeVAO);
// 	glDrawElements(GL_LINES, sizeof(cubeIndices) / sizeof(unsigned int), GL_UNSIGNED_INT, 0);
// 	glBindVertexArray(0);
// }
