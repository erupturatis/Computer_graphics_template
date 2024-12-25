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
		GLint lightDirLoc;
		GLint lightColorLoc;
	};
}

#endif //COMMONGROUPINGS_H
