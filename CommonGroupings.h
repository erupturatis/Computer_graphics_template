#ifndef COMMONGROUPINGS_H
#define COMMONGROUPINGS_H

#include <GL/glew.h>

namespace globals_structs
{
	struct MatricesShaderCommon
	{
		glm::mat4 view;
		glm::mat4 projection;
	};

	struct ShaderLocationsDepth
	{
		GLint lightSpaceMatrix;
		GLint model;
	};

	struct terrainBinding
	{
		int i;
		int j;
	};


	struct ShaderLocationsBasic
	{
		GLint modelLoc;

		GLint viewLoc;
		GLint projectionLoc;
		GLint normalMatrixLoc;

		GLint lightDirDir;
		GLint lightDirColor;

		GLint lightPointLoc;
		GLint lightPointColor;

		GLint lightSpaceMatrix;
		GLint shadowMap;
	};
}

#endif //COMMONGROUPINGS_H
