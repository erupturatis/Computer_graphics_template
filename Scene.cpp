#include "Scene.h"
#include <vector>
#include "CommonGroupings.h"
#include "GlobalObjects.h"
#include "pure_functions.h"

namespace scene
{
	std::vector<globals::Model3D*> models;

	std::vector<globals::Model3D*>& getModels3D() {
		return models;
	};

	void registerModel3d(globals::Model3D& model) {
		models.push_back(&model);
	}

	// MODELS
	globals::Model3D teapot;
	// terrain
	std::vector<globals::Model3D> terrain_blocks;
	int terrain_blocks_count = 5;
	float terrain_block_distance_x_axis_j_coord = 5.0f;
	float terrain_block_distance_z_axis_i_coord = 5.5f;
	float terrain_block_scale = 0.5f;

	// shrooms
	std::vector<globals::Model3D> shrooms;
	std::vector<globals_structs::terrainBinding> shrooms_bindings;
	std::vector<glm::vec3> shrooms_random_noise;
	std::vector<glm::vec3> shrooms_random_rotations;
	std::vector<float> shrooms_random_scales;
	float shroom_default_y = 0.65f;
	float shroom_scale = 1.0f;
	int shrooms_count = 30;
	int shrooms_types_start = 1;
	int shrooms_types_end = 3;

	// house
	globals::Model3D house1;
	float house1_scale = 0.1f;
	float house1_default_y = 0.5f;
	globals_structs::terrainBinding terrain_binding_house1;

	// others
	globals::Model3D book;
	float book_scale = 10.0f;
	float book_default_y = 7.0f;
	float book_float_variation = 3.0f;
	globals_structs::terrainBinding terrain_binding_book;

	void loadBook() {
		book.LoadModel("models/book/book.obj");
		registerModel3d(book);
	}

	void loadTerrain() {
		for (int i = 0; i < terrain_blocks_count; i++) {
			for (int j = 0; j < terrain_blocks_count; j++) {
				globals::Model3D terrain_block = globals::Model3D();
				terrain_blocks.push_back(terrain_block);
			}
		}

		for (int i = 0; i < terrain_blocks_count; i++) {
			for (int j = 0; j < terrain_blocks_count; j++) {
				int index = globals::get_terrain_index(i, j, terrain_blocks_count);
				globals::Model3D& terrain_block = terrain_blocks[index];
				terrain_block.LoadModel("models/terrain/terrain.obj");
				registerModel3d(terrain_block);
			}
		}
	}

	void loadShrooms() {
		for (int i = 0; i < shrooms_count; i++) {
			globals::Model3D shroom = globals::Model3D();
			shrooms.push_back(shroom);
		}

		for (int i = 0; i < shrooms_count; i++) {
			globals::Model3D& shroom = shrooms[i];
			int random_shroom_type = shrooms_types_start + rand() % (shrooms_types_end - shrooms_types_start + 1);
			shroom.LoadModel("models/environment/shroom" + std::to_string(random_shroom_type) + ".obj");
			registerModel3d(shroom);
		}
	}

	void loadHouse1() {
		house1.LoadModel("models/environment/house1.obj");
		registerModel3d(house1);
	}


	void initBook() {
		glm::mat4 book_model_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, book_default_y, 0.0f));
		book.setModelMatrix(book_model_matrix);
		book.initializeBoundingBoxes();
		book.bindBoundingBoxesGPU();
		book.calculateBoundingBoxes();
		terrain_binding_book.i = 2;
		terrain_binding_book.j = 2;
	}

	void initTerrain() {
		for (int i = 0; i < terrain_blocks_count; i++) {
			for (int j = 0; j < terrain_blocks_count; j++) {
				int index = globals::get_terrain_index(i, j, terrain_blocks_count);
				globals::Model3D& terrain_block = terrain_blocks[index];

				float terrain_block_y = 0.0f;
				float terrain_block_z = i * terrain_block_distance_z_axis_i_coord;
				float terrain_block_x = j * terrain_block_distance_x_axis_j_coord;

				glm::mat4 terrain_block_model_matrix = glm::translate(glm::mat4(1.0f),
				                                                      glm::vec3(terrain_block_x, terrain_block_y,
					                                                      terrain_block_z)) *

					glm::scale(glm::mat4(1.0f), glm::vec3(terrain_block_scale));

				terrain_block.setModelMatrix(terrain_block_model_matrix);
				terrain_block.initializeBoundingBoxes();
				terrain_block.bindBoundingBoxesGPU();
				terrain_block.calculateBoundingBoxes();
			}
		}
	}

	void initShrooms() {
		for (int i = 0; i < shrooms_count; i++) {
			globals::Model3D& shroom = shrooms[i];
			glm::mat4 shroom_model_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 2.0f, 0.0f));
			shroom.setModelMatrix(shroom_model_matrix);
			shroom.initializeBoundingBoxes();
			shroom.bindBoundingBoxesGPU();
			shroom.calculateBoundingBoxes();

			globals_structs::terrainBinding terrain_binding_shroom;
			terrain_binding_shroom.i = rand() % terrain_blocks_count;
			terrain_binding_shroom.j = rand() % terrain_blocks_count;
			shrooms_bindings.push_back(terrain_binding_shroom);


			float z_noise = globals::getRandomFloat(-0.5f * terrain_block_distance_z_axis_i_coord,
			                                        0.5f * terrain_block_distance_z_axis_i_coord);
			float x_noise = globals::getRandomFloat(-0.5f * terrain_block_distance_x_axis_j_coord,
			                                        0.5f * terrain_block_distance_x_axis_j_coord);
			glm::vec3 noise = glm::vec3(x_noise, 0.0f, z_noise);
			shrooms_random_noise.push_back(noise);

			float y_rotation = globals::getRandomFloat(0.0f, 360.0f);
			float x_rotation = globals::getRandomFloat(0.0f, 7.0f);
			float z_rotation = globals::getRandomFloat(0.0f, 7.0f);
			glm::vec3 rotation = glm::vec3(x_rotation, y_rotation, z_rotation);
			shrooms_random_rotations.push_back(rotation);

			float scale = globals::getRandomFloat(0.75f * shroom_scale, 1.25f * shroom_scale);
			shrooms_random_scales.push_back(scale);
		}
	}

	void initHouse1() {
		glm::mat4 house1_model_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, house1_default_y, 0.0f));
		house1.setModelMatrix(house1_model_matrix);
		house1.initializeBoundingBoxes();
		house1.bindBoundingBoxesGPU();
		house1.calculateBoundingBoxes();
		terrain_binding_house1.i = 3;
		terrain_binding_house1.j = 2;
	}


	void renderTerrain(bool depth) {
		globals::Shader& shader = globals::getBasicShader();
		globals::Shader& depthShader = globals::getDepthShader();
		globals_structs::ShaderLocationsBasic& shaderLocations = globals::getBasicShaderLocations();
		globals_structs::ShaderLocationsDepth& depthShaderLocations = globals::getDepthShaderLocations();

		if (depth == false) {
			shader.useShaderProgram();
		}
		else {
			depthShader.useShaderProgram();
		}

		for (int i = 0; i < terrain_blocks_count; i++) {
			for (int j = 0; j < terrain_blocks_count; j++) {
				int index = globals::get_terrain_index(i, j, terrain_blocks_count);
				globals::Model3D& terrain_block = terrain_blocks[index];

				float terrain_block_y = 0.0f;
				float terrain_block_z = i * terrain_block_distance_z_axis_i_coord;
				float terrain_block_x = j * terrain_block_distance_x_axis_j_coord;

				glm::mat4 terrain_block_model_matrix = glm::translate(glm::mat4(1.0f),
				                                                      glm::vec3(terrain_block_x, terrain_block_y,
					                                                      terrain_block_z)) *
					glm::scale(glm::mat4(1.0f), glm::vec3(terrain_block_scale));

				terrain_block.setModelMatrix(terrain_block_model_matrix);
				terrain_block.recalculateNormal();

				if (depth == false) {
					glUniformMatrix4fv(shaderLocations.modelLoc, 1, GL_FALSE,
					                   glm::value_ptr(terrain_block.getModelMatrix()));
					glUniformMatrix3fv(shaderLocations.normalMatrixLoc, 1, GL_FALSE,
					                   glm::value_ptr(terrain_block.getNormalMatrix()));

					terrain_block.calculateBoundingBoxes();
					terrain_block.Draw(shader);
				}
				else {
					glUniformMatrix4fv(depthShaderLocations.model, 1, GL_FALSE,
					                   glm::value_ptr(terrain_block.getModelMatrix()));

					terrain_block.DrawDepth(depthShader);
				}
			}
		}
	}

	void renderBook(bool depth) {
		globals::Shader& shader = globals::getBasicShader();
		globals::Shader& depthShader = globals::getDepthShader();
		globals_structs::ShaderLocationsBasic& shaderLocations = globals::getBasicShaderLocations();
		globals_structs::ShaderLocationsDepth& depthShaderLocations = globals::getDepthShaderLocations();

		float time = glfwGetTime();

		if (depth == false) {
			shader.useShaderProgram();
		}
		else {
			depthShader.useShaderProgram();
		}

		// whatever transformations we want to apply to the teapot
		glm::mat4 transformation_base = glm::scale(glm::mat4(1.0f), glm::vec3(book_scale));
		transformation_base = glm::rotate(glm::mat4(1.0f), glm::radians(time * 10), glm::vec3(0.0f, 1.0f, 0.0f)) *
			transformation_base;

		transformation_base = glm::translate(glm::mat4(1.0f),
		                                     glm::vec3(terrain_block_distance_x_axis_j_coord * terrain_binding_book.j,
		                                               0.0f,
		                                               terrain_block_distance_z_axis_i_coord * terrain_binding_book.i))
			*
			transformation_base;

		transformation_base = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, book_default_y, 0.0f)) *
			transformation_base;

		transformation_base = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, book_float_variation * sin(time), 0.0f))
			*
			transformation_base;

		if (depth == false) {
			globals::Camera& myCamera = globals::getCamera();
			glm::mat4 view = myCamera.getViewMatrix();

			glm::vec3 finalBookPosition = glm::vec3(transformation_base[3]);
			finalBookPosition.y = 1.0f;

			glm::vec4 pointLightPosWorld = glm::vec4(finalBookPosition, 1.0f);
			glm::vec4 pointLightPosEye = view * pointLightPosWorld;
			glm::vec3 pointLightPosEye3 = glm::vec3(pointLightPosEye);

			globals::setLightPointLoc(pointLightPosEye3);
			glUniform3fv(shaderLocations.lightPointLoc, 1, glm::value_ptr(globals::getLightPointLoc()));
		}

		book.setModelMatrix(transformation_base);
		book.recalculateNormal();

		if (depth == false) {
			// update matrices for this object
			glUniformMatrix4fv(shaderLocations.modelLoc, 1, GL_FALSE, glm::value_ptr(book.getModelMatrix()));
			glUniformMatrix3fv(shaderLocations.normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(book.getNormalMatrix()));

			book.calculateBoundingBoxes();
			book.Draw(shader);
		}
		else {
			glUniformMatrix4fv(depthShaderLocations.model, 1, GL_FALSE, glm::value_ptr(book.getModelMatrix()));
			book.DrawDepth(depthShader);
		}
	}


	void renderShrooms(bool depth) {
		globals::Shader& shader = globals::getBasicShader();
		globals::Shader& depthShader = globals::getDepthShader();
		globals_structs::ShaderLocationsBasic& shaderLocations = globals::getBasicShaderLocations();
		globals_structs::ShaderLocationsDepth& depthShaderLocations = globals::getDepthShaderLocations();

		if (depth == false) {
			shader.useShaderProgram();
		}
		else {
			depthShader.useShaderProgram();
		}

		for (int i = 0; i < shrooms_count; i++) {
			globals::Model3D& shroom = shrooms[i];
			globals_structs::terrainBinding& terrain_binding_shroom = shrooms_bindings[i];
			glm::vec3& noise = shrooms_random_noise[i];
			glm::vec3& rotation = shrooms_random_rotations[i];


			float terrain_binding_z = terrain_binding_shroom.i * terrain_block_distance_z_axis_i_coord;
			float terrain_binding_x = terrain_binding_shroom.j * terrain_block_distance_x_axis_j_coord;

			float terrain_block_x = terrain_binding_x + noise.x;
			float terrain_block_y = shroom_default_y * shrooms_random_scales[i];
			float terrain_block_z = terrain_binding_z + noise.z;

			float scale = shrooms_random_scales[i];
			glm::mat4 transformation_base = glm::scale(glm::mat4(1.0f), glm::vec3(scale));
			transformation_base = glm::rotate(glm::mat4(1.0f), glm::radians(rotation.x), glm::vec3(1.0f, 0, 0)) *
				transformation_base;
			transformation_base = glm::rotate(glm::mat4(1.0f), glm::radians(rotation.y), glm::vec3(0, 1.0f, 0)) *
				transformation_base;
			transformation_base = glm::rotate(glm::mat4(1.0f), glm::radians(rotation.z), glm::vec3(0, 0, 1.0f)) *
				transformation_base;
			transformation_base = glm::translate(glm::mat4(1.0f),
			                                     glm::vec3(terrain_block_x, terrain_block_y,
			                                               terrain_block_z)) *
				transformation_base;

			shroom.setModelMatrix(transformation_base);
			shroom.recalculateNormal();

			if (depth == false) {
				glUniformMatrix4fv(shaderLocations.modelLoc, 1, GL_FALSE, glm::value_ptr(shroom.getModelMatrix()));
				glUniformMatrix3fv(shaderLocations.normalMatrixLoc, 1, GL_FALSE,
				                   glm::value_ptr(shroom.getNormalMatrix()));

				shroom.calculateBoundingBoxes();
				shroom.Draw(shader);
			}
			else {
				glUniformMatrix4fv(depthShaderLocations.model, 1, GL_FALSE, glm::value_ptr(shroom.getModelMatrix()));
				shroom.DrawDepth(depthShader);
			}
		}
	}

	glm::mat4 calculateLightSpaceMatrix() {
		float distance_light = 20.0f;
		glm::vec3 offset = glm::vec3(10.0f, 0.0f, 10.0f);
		glm::mat4 lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(globals::getLightDirRotationAngle()),
		                                      globals::getLightDirRotationAxis());

		glm::mat4 lightView = glm::lookAt(
			glm::inverseTranspose(glm::mat3(lightRotation)) * globals::getLightDirDir() * distance_light + offset,
			glm::vec3(0.0f) + offset,
			glm::vec3(0.0f, 1.0f, 0.0f));

		const GLfloat near_plane = 0.1f, far_plane = 50.0f;
		float scaler = 25.0f;
		glm::mat4 lightProjection = glm::ortho(-1.0f * scaler, 1.0f * scaler, -1.0f * scaler, 1.0f * scaler, near_plane,
		                                       far_plane);
		glm::mat4 lightSpaceTrMatrix = lightProjection * lightView;

		return lightSpaceTrMatrix;
	}

	void renderHouse1(bool depth) {
		globals::Shader& shader = globals::getBasicShader();
		globals::Shader& depthShader = globals::getDepthShader();
		globals_structs::ShaderLocationsBasic& shaderLocations = globals::getBasicShaderLocations();
		globals_structs::ShaderLocationsDepth& depthShaderLocations = globals::getDepthShaderLocations();

		if (depth == false) {
			shader.useShaderProgram();
		}
		else {
			depthShader.useShaderProgram();
		}

		glm::mat4 transformation_base = glm::scale(glm::mat4(1.0f), glm::vec3(house1_scale));
		transformation_base = glm::rotate(glm::mat4(1.0f), glm::radians(135.0f), glm::vec3(0.0f, 1.0f, 0.0f)) *
			transformation_base;
		transformation_base = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, house1_default_y, 0.0f)) *
			transformation_base;
		transformation_base = glm::translate(glm::mat4(1.0f),
		                                     glm::vec3(terrain_block_distance_x_axis_j_coord * terrain_binding_house1.j,
		                                               0.0f,
		                                               terrain_block_distance_z_axis_i_coord * terrain_binding_house1.
		                                               i)) *
			transformation_base;

		house1.setModelMatrix(transformation_base);
		house1.recalculateNormal();

		if (depth == false) {
			glUniformMatrix4fv(shaderLocations.modelLoc, 1, GL_FALSE, glm::value_ptr(house1.getModelMatrix()));
			glUniformMatrix3fv(shaderLocations.normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(house1.getNormalMatrix()));

			house1.calculateBoundingBoxes();
			house1.Draw(shader);
		}
		else {
			glUniformMatrix4fv(depthShaderLocations.model, 1, GL_FALSE, glm::value_ptr(house1.getModelMatrix()));
			// glUniformMatrix4fv(depthShaderLocations.lightSpaceMatrix, 1, GL_FALSE,
			//                    glm::value_ptr(calculateLightSpaceMatrix()));

			house1.DrawDepth(depthShader);
		}
	}
}
