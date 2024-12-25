#ifndef Model3D_hpp
#define Model3D_hpp

#include "Mesh.hpp"
#include "BoundingBoxes.h"

#include "tiny_obj_loader.h"
#include "stb_image.h"

#include <iostream>
#include <string>
#include <vector>
#include "glm/gtc/matrix_inverse.hpp"

namespace globals
{
	class Model3D
	{
	public:
		~Model3D();

		void LoadModel(std::string fileName);

		void LoadModel(std::string fileName, std::string basePath);

		void Draw(globals::Shader shaderProgram);

		const std::vector<globals::Mesh>& getSubObjects() const;

		void recalculateNormal();

		void setModelMatrix(glm::mat4);

		glm::mat4 getModelMatrix();

		glm::mat3 getNormalMatrix();

	private:
		std::vector<globals::Mesh> meshes;
		std::vector<globals::BoundingBox3D> bounding_boxes;

		std::vector<globals::Texture> loadedTextures;

		void ReadOBJ(std::string fileName, std::string basePath);

		globals::Texture LoadTexture(std::string path, std::string type);

		GLuint ReadTextureFromFile(const char* file_name);

		glm::mat4 modelMatrix;
		glm::mat3 normalMatrix;
	};
}

#endif /* Model3D_hpp */
