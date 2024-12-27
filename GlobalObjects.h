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

    globals::Shader& getDepthShader();
    void setDepthShader(globals::Shader shader);

    globals::Shader& getBasicShader();
    void setBasicShader(globals::Shader shader);

    globals_structs::ShaderLocationsBasic& getBasicShaderLocations();
    void setBasicShaderLocations(globals_structs::ShaderLocationsBasic locations);

    globals_structs::ShaderLocationsDepth& getDepthShaderLocations();
    void setDepthShaderLocations(globals_structs::ShaderLocationsDepth locations);

    globals_structs::MatricesShaderCommon& getMatrices();
    void setMatrices(globals_structs::MatricesShaderCommon matrices);

    void registerKeyPress(int key, int action);
    bool isKeyPressed(int key);

    int getShadowWidth();
    int getShadowHeight();
    int getWindowWidth();
    int getWindowHeight();

    void initGlobalObjects();

    glm::vec3 getLightDirDir();
    void setLightDirDir(glm::vec3 newLightDirDir);
    glm::vec3 getLightDirColor();
    void setLightDirColor(glm::vec3 newLightDirColor);
    glm::vec3 getLightPointLoc();
    void setLightPointLoc(glm::vec3 newLightPointLoc);
    glm::vec3 getLightPointColor();
    void setLightPointColor(glm::vec3 newLightPointColor);
    glm::vec3 getBackgroundColor();
    void setBackgroundColor(glm::vec3 newBackgroundColor);
}

#endif // GLOBALOBJECTS_H
