#ifndef Shader_hpp
#define Shader_hpp

#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif

#include <iostream>
#include <string>
#include <sstream>
#include <GL/glew.h>
#include <fstream>


namespace globals
{
    class Shader
    {
    public:
        GLuint shaderProgram;
        void loadShader(std::string vertexShaderFileName, std::string fragmentShaderFileName);
        void useShaderProgram();

    private:
        std::string readShaderFile(std::string fileName);
        void shaderCompileLog(GLuint shaderId);
        void shaderLinkLog(GLuint shaderProgramId);
    };
}

#endif /* Shader_hpp */
