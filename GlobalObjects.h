#ifndef GLOBALOBJECTS_H
#define GLOBALOBJECTS_H

#include "Camera.hpp"
#include "CommonGroupings.h"
#include "Shader.hpp"
#include "Window.h"

namespace globals
{
    globals::Camera& getCamera();
    void setCamera(globals::Camera camera);

    globals::Window& getWindow();
    void setWindow(globals::Window window);

    globals::Shader& getBasicShader();
    void setBasicShader(globals::Shader shader);

    globals_structs::ShaderLocations& getBasicShaderLocations();
    void setBasicShaderLocations(globals_structs::ShaderLocations locations);

    globals_structs::MatricesShaderCommon& getMatrices();
    void setMatrices(globals_structs::MatricesShaderCommon matrices);

    void registerKeyPress(int key, int action);
    bool isKeyPressed(int key);
}

#endif // GLOBALOBJECTS_H
