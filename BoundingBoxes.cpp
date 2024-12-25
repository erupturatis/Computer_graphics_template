#include "BoundingBoxes.h"

namespace globals
{
	void BoundingBox3D::Draw(globals::Shader shaderProgram) {
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
