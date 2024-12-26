#include "BoundingBox.h"
#include "Model3D.hpp"
#include "Camera.hpp"
#include "GlobalObjects.h"
#include "pure_functions.h"

#include <glm/gtx/string_cast.hpp>

#include "Bindings.h"

namespace globals
{
	void Model3D::LoadModel(std::string fileName) {
		std::string basePath = fileName.substr(0, fileName.find_last_of('/')) + "/";
		ReadOBJ(fileName, basePath);
	}

	void Model3D::LoadModel(std::string fileName, std::string basePath) {
		ReadOBJ(fileName, basePath);
	}

	std::vector<BoundingBox3D>& Model3D::getBoundingBoxes() {
		return bounding_boxes;
	}

	void Model3D::initializeBoundingBoxes() {
		// We use only once to fill the array
		if (bounding_boxes.size() != 0) {
			std::cerr << "Bounding boxes already calculated" << std::endl;
			return;
		}

		for (int i = 0; i < meshes.size(); i++) {
			std::vector<globals::Vertex> vertices = meshes[i].getVertices();
			globals::BoundingBox3D boundingBox = BoundingBox3D();
			bounding_boxes.push_back(boundingBox);
		}
	}

	void Model3D::calculateBoundingBoxes() {
		for (int i = 0; i < bounding_boxes.size(); i++) {
			std::vector<globals::Vertex> vertices = meshes[i].getVertices();
			glm::mat4 modelMatrix = getModelMatrix();

			// used for first type of box fitting
			// for (globals::Vertex& vertex : vertices) {
			// 	glm::vec4 transformedPosition = modelMatrix * glm::vec4(vertex.Position, 1.0f);
			// 	vertex.Position = glm::vec3(transformedPosition);
			// }

			std::pair<glm::vec3, glm::vec3> min_max = calculateBoundingBox(vertices);

			glm::vec3 localMin = min_max.first;
			glm::vec3 localMax = min_max.second;

			glm::vec3 corners[] = {
				{localMin.x, localMin.y, localMin.z},
				{localMax.x, localMin.y, localMin.z},
				{localMax.x, localMax.y, localMin.z},
				{localMin.x, localMax.y, localMin.z},
				{localMin.x, localMin.y, localMax.z},
				{localMax.x, localMin.y, localMax.z},
				{localMax.x, localMax.y, localMax.z},
				{localMin.x, localMax.y, localMax.z}
			};

			std::vector<glm::vec3> transformedCorners;

			float collisionBoxScale = 1.1f;
			// contraction for bounding box
			glm::mat4 transformation_scaling = glm::scale(glm::mat4(1.0f), glm::vec3(collisionBoxScale));

			for (glm::vec3 corner : corners) {
				transformedCorners.push_back(glm::vec3(modelMatrix * transformation_scaling * glm::vec4(corner, 1.0f)));
			}
			bounding_boxes[i].setBoundingBoxCorners(transformedCorners);
		}
	}

	void Model3D::bindBoundingBoxesGPU() {
		if (bounding_boxes.size() == 0) {
			std::cerr << "Bounding boxes not calculated" << std::endl;
			return;
		}
		for (int i = 0; i < bounding_boxes.size(); i++) {
			bounding_boxes[i].initializeBoundingBoxGPU();
		}
	}

	void Model3D::setBoundingBox(glm::vec3 min, glm::vec3 max, int index) {
		std::cerr << "DEPRECATED SET BOUNDING BOX";
		// bounding_boxes[index].setBoundingBox(min, max);
	}

	// Draw each mesh from the model
	void Model3D::Draw(globals::Shader shaderProgram) {
		for (int i = 0; i < meshes.size(); i++)
			meshes[i].Draw(shaderProgram);

		if (globals_configs::getShowBoundingBoxes()) {
			for (auto& bounding_box : bounding_boxes)
				bounding_box.Draw();
		}
	}

	void Model3D::setModelMatrix(glm::mat4 modelMatrix) {
		this->modelMatrix = modelMatrix;
	}

	glm::mat4 Model3D::getModelMatrix() {
		return modelMatrix;
	}

	void Model3D::recalculateNormal() {
		Camera& myCamera = getCamera();
		glm::mat4 view = myCamera.getViewMatrix();
		this->normalMatrix = glm::mat3(glm::inverseTranspose(view * modelMatrix));
	}

	glm::mat3 Model3D::getNormalMatrix() {
		return normalMatrix;
	}

	// Does the parsing of the .obj file and fills in the data structure
	void Model3D::ReadOBJ(std::string fileName, std::string basePath) {
		std::cout << "Loading : " << fileName << std::endl;
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		int materialId;

		std::string err;
		bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, fileName.c_str(), basePath.c_str(), GL_TRUE);

		if (!err.empty()) {
			// `err` may contain warning message.
			std::cerr << err << std::endl;
		}

		if (!ret) {
			exit(1);
		}

		std::cout << "# of shapes    : " << shapes.size() << std::endl;
		std::cout << "# of materials : " << materials.size() << std::endl;

		// Loop over shapes
		for (size_t s = 0; s < shapes.size(); s++) {
			std::vector<globals::Vertex> vertices;
			std::vector<GLuint> indices;
			std::vector<globals::Texture> textures;

			// Loop over faces(polygon)
			size_t index_offset = 0;
			for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
				int fv = shapes[s].mesh.num_face_vertices[f];

				//gps::Texture currentTexture = LoadTexture("index1.png", "ambientTexture");
				//textures.push_back(currentTexture);

				// Loop over vertices in the face.
				for (size_t v = 0; v < fv; v++) {
					// access to vertex
					tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];

					float vx = attrib.vertices[3 * idx.vertex_index + 0];
					float vy = attrib.vertices[3 * idx.vertex_index + 1];
					float vz = attrib.vertices[3 * idx.vertex_index + 2];
					float nx = attrib.normals[3 * idx.normal_index + 0];
					float ny = attrib.normals[3 * idx.normal_index + 1];
					float nz = attrib.normals[3 * idx.normal_index + 2];
					float tx = 0.0f;
					float ty = 0.0f;

					if (idx.texcoord_index != -1) {
						tx = attrib.texcoords[2 * idx.texcoord_index + 0];
						ty = attrib.texcoords[2 * idx.texcoord_index + 1];
					}

					glm::vec3 vertexPosition(vx, vy, vz);
					glm::vec3 vertexNormal(nx, ny, nz);
					glm::vec2 vertexTexCoords(tx, ty);

					globals::Vertex currentVertex;
					currentVertex.Position = vertexPosition;
					currentVertex.Normal = vertexNormal;
					currentVertex.TexCoords = vertexTexCoords;

					vertices.push_back(currentVertex);

					indices.push_back((GLuint)(index_offset + v));
				}

				index_offset += fv;
			}

			// get material id
			// Only try to read materials if the .mtl file is present
			size_t a = shapes[s].mesh.material_ids.size();

			if (a > 0 && materials.size() > 0) {
				materialId = shapes[s].mesh.material_ids[0];
				if (materialId != -1) {
					globals::Material currentMaterial;
					currentMaterial.ambient = glm::vec3(materials[materialId].ambient[0],
					                                    materials[materialId].ambient[1],
					                                    materials[materialId].ambient[2]);
					currentMaterial.diffuse = glm::vec3(materials[materialId].diffuse[0],
					                                    materials[materialId].diffuse[1],
					                                    materials[materialId].diffuse[2]);
					currentMaterial.specular = glm::vec3(materials[materialId].specular[0],
					                                     materials[materialId].specular[1],
					                                     materials[materialId].specular[2]);

					//ambient texture
					std::string ambientTexturePath = materials[materialId].ambient_texname;

					if (!ambientTexturePath.empty()) {
						globals::Texture currentTexture;
						currentTexture = LoadTexture(basePath + ambientTexturePath, "ambientTexture");
						textures.push_back(currentTexture);
					}

					//diffuse texture
					std::string diffuseTexturePath = materials[materialId].diffuse_texname;

					if (!diffuseTexturePath.empty()) {
						globals::Texture currentTexture;
						currentTexture = LoadTexture(basePath + diffuseTexturePath, "diffuseTexture");
						textures.push_back(currentTexture);
					}

					//specular texture
					std::string specularTexturePath = materials[materialId].specular_texname;

					if (!specularTexturePath.empty()) {
						globals::Texture currentTexture;
						currentTexture = LoadTexture(basePath + specularTexturePath, "specularTexture");
						textures.push_back(currentTexture);
					}
				}
			}

			meshes.push_back(globals::Mesh(vertices, indices, textures));
		}
	}

	// Retrieves a texture associated with the object - by its name and type
	globals::Texture Model3D::LoadTexture(std::string path, std::string type) {
		for (int i = 0; i < loadedTextures.size(); i++) {
			if (loadedTextures[i].path == path) {
				//already loaded texture
				return loadedTextures[i];
			}
		}

		globals::Texture currentTexture;
		currentTexture.id = ReadTextureFromFile(path.c_str());
		currentTexture.type = std::string(type);
		currentTexture.path = path;

		loadedTextures.push_back(currentTexture);

		return currentTexture;
	}

	// Reads the pixel data from an image file and loads it into the video memory
	GLuint Model3D::ReadTextureFromFile(const char* file_name) {
		int x, y, n;
		int force_channels = 4;
		unsigned char* image_data = stbi_load(file_name, &x, &y, &n, force_channels);

		if (!image_data) {
			fprintf(stderr, "ERROR: could not load %s\n", file_name);
			return false;
		}
		// NPOT check
		if ((x & (x - 1)) != 0 || (y & (y - 1)) != 0) {
			fprintf(
				stderr, "WARNING: texture %s is not power-of-2 dimensions\n", file_name
			);
		}

		int width_in_bytes = x * 4;
		unsigned char* top = NULL;
		unsigned char* bottom = NULL;
		unsigned char temp = 0;
		int half_height = y / 2;

		for (int row = 0; row < half_height; row++) {
			top = image_data + row * width_in_bytes;
			bottom = image_data + (y - row - 1) * width_in_bytes;

			for (int col = 0; col < width_in_bytes; col++) {
				temp = *top;
				*top = *bottom;
				*bottom = temp;
				top++;
				bottom++;
			}
		}

		GLuint textureID;
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_SRGB, //GL_SRGB,//GL_RGBA,
			x,
			y,
			0,
			GL_RGBA,
			GL_UNSIGNED_BYTE,
			image_data
		);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);

		return textureID;
	}

	Model3D::~Model3D() {
		for (size_t i = 0; i < loadedTextures.size(); i++) {
			glDeleteTextures(1, &loadedTextures.at(i).id);
		}

		for (size_t i = 0; i < meshes.size(); i++) {
			GLuint VBO = meshes.at(i).getBuffers().VBO;
			GLuint EBO = meshes.at(i).getBuffers().EBO;
			GLuint VAO = meshes.at(i).getBuffers().VAO;
			glDeleteBuffers(1, &VBO);
			glDeleteBuffers(1, &EBO);
			glDeleteVertexArrays(1, &VAO);
		}
	}
}
