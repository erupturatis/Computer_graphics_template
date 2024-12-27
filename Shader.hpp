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
#include <glm/glm.hpp>


namespace globals
{
    class Shader
    {
    public:
        GLuint shaderProgram;
        void loadShader(std::string vertexShaderFileName, std::string fragmentShaderFileName);
        void useShaderProgram();
        void setMat4(const std::string& name, const glm::mat4& value);
        void setInt(const std::string& name, int value);

    private:
        std::string readShaderFile(std::string fileName);
        void shaderCompileLog(GLuint shaderId);
        void shaderLinkLog(GLuint shaderProgramId);
    };

    void initShadersLocations();
}

#endif /* Shader_hpp */
