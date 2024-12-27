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

	struct ShaderLocations
	{
		GLint modelLoc;

		GLint viewLoc;
		GLint projectionLoc;
		GLint normalMatrixLoc;

		GLint lightDirDir;
		GLint lightDirColor;

		GLint lightPointLoc;
		GLint lightPointColor;
	};
}

#endif //COMMONGROUPINGS_H
