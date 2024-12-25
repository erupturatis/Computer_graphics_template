#include "GlobalObjects.h"
#include "CommonGroupings.h"

namespace globals
{
    Camera camera;
    Window window;
    Shader basicShader;
    globals_structs::ShaderLocations basicShaderLocations;
    globals_structs::MatricesShaderCommon matrices;
    GLboolean pressedKeys[1024];

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

    globals_structs::ShaderLocations& getBasicShaderLocations() {
        return basicShaderLocations;
    }

    void setBasicShaderLocations(globals_structs::ShaderLocations newLocations) {
        basicShaderLocations = newLocations;
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


