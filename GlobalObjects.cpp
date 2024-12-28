#include "GlobalObjects.h"
#include "CommonGroupings.h"
#include "pure_functions.h"

namespace globals
{
    Camera camera;
    Window window;

    Shader basicShader;
    Shader depthShader;

    globals_structs::ShaderLocationsBasic basicShaderLocations;
    globals_structs::ShaderLocationsDepth depthShaderLocations;

    globals_structs::MatricesShaderCommon matrices;
    GLboolean pressedKeys[1024];

    const unsigned int SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;
    const unsigned int WINDOW_WIDTH = 1024, WINDOW_HEIGHT = 768;

    glm::vec3 lightDirDir;
    glm::vec3 lightDirColor;
    float lightDirRot;

    glm::vec3 backgroundColor;
    glm::vec3 lightPointLoc;
    glm::vec3 lightPointColor;

    float getLightDirRot() {
        return lightDirRot;
    }

    void setLightDirRot(float newLightDirRot) {
        lightDirRot = newLightDirRot;
    }

    void setDepthShader(globals::Shader newShader) {
        depthShader = newShader;
    }

    Shader& getDepthShader() {
        return depthShader;
    }


    glm::vec3 getLightDirDir() {
        return lightDirDir;
    }

    void setLightDirDir(glm::vec3 newLightDirDir) {
        lightDirDir = newLightDirDir;
    }

    glm::vec3 getLightDirColor() {
        return lightDirColor;
    }

    void setLightDirColor(glm::vec3 newLightDirColor) {
        lightDirColor = newLightDirColor;
    }

    glm::vec3 getBackgroundColor() {
        return backgroundColor;
    }

    void setBackgroundColor(glm::vec3 newBackgroundColor) {
        backgroundColor = newBackgroundColor;
    }

    glm::vec3 getLightPointLoc() {
        return lightPointLoc;
    }

    void setLightPointLoc(glm::vec3 newLightPointLoc) {
        lightPointLoc = newLightPointLoc;
    }

    glm::vec3 getLightPointColor() {
        return lightPointColor;
    }

    void setLightPointColor(glm::vec3 newLightPointColor) {
        lightPointColor = newLightPointColor;
    }

    int getShadowWidth() {
        return SHADOW_WIDTH;
    }

    int getShadowHeight() {
        return SHADOW_HEIGHT;
    }

    int getWindowWidth() {
        return WINDOW_WIDTH;
    }

    int getWindowHeight() {
        return WINDOW_HEIGHT;
    }

    Camera& getCamera() {
        return camera;
    }

    void setCamera(globals::Camera newCamera) {
        camera = newCamera;
    }

    globals::Window& getWindow() {
        return window;
    }

    void setWindow(globals::Window newWindow) {
        window = newWindow;
    }

    Shader& getBasicShader() {
        return basicShader;
    }

    void setBasicShader(globals::Shader newShader) {
        basicShader = newShader;
    }

    globals_structs::ShaderLocationsBasic& getBasicShaderLocations() {
        return basicShaderLocations;
    }

    void setBasicShaderLocations(globals_structs::ShaderLocationsBasic newLocations) {
        basicShaderLocations = newLocations;
    }

    globals_structs::ShaderLocationsDepth& getDepthShaderLocations() {
        return depthShaderLocations;
    }

    void setDepthShaderLocations(globals_structs::ShaderLocationsDepth newLocations) {
        depthShaderLocations = newLocations;
    }


    void initGlobalObjects() {
        // window
        globals::Window window = globals::Window();
        window.Create(globals::getWindowWidth(), globals::getWindowHeight(), "OpenGL Project");
        globals::setWindow(window);

        // camera
        globals::Camera camera = globals::Camera();
        camera.initializeCamera(
            glm::vec3(0.0f, 5.0f, 3.0f),
            glm::vec3(10.0f, 3.0f, 10.0f),
            glm::vec3(0.0f, 1.0f, 0.0f));
        globals::setCamera(camera);

        // common matrices for shader (projection and view)
        globals_structs::MatricesShaderCommon matrices = globals_structs::MatricesShaderCommon();
        globals::setMatrices(matrices);

        // locations for shader GPU matrices
        globals_structs::ShaderLocationsBasic shaderLocations = globals_structs::ShaderLocationsBasic();
        globals::setBasicShaderLocations(shaderLocations);

        // shader
        globals::Shader myBasicShader = globals::Shader();
        globals::setBasicShader(myBasicShader);
    }


    globals_structs::MatricesShaderCommon& getMatrices() {
        return matrices;
    }

    void setMatrices(globals_structs::MatricesShaderCommon newMatrices) {
        matrices = newMatrices;
    }

    void registerKeyPress(int key, int action) {
        if (key >= 0 && key < 1024) {
            if (action == GLFW_PRESS) {
                pressedKeys[key] = true;
            }
            else if (action == GLFW_RELEASE) {
                pressedKeys[key] = false;
            }
        }
    }

    bool isKeyPressed(int key) {
        return pressedKeys[key];
    }
}


