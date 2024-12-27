#include "GlobalConfigs.h"
#include <GL/glew.h>

namespace globals_configs
{
    bool showBoundingBoxes = true;
    GLfloat cameraSpeed = 0.1f;
    bool wireframeMode = false;
    bool acceleratedDayNightCycle = false;

    bool getWireframeMode() {
        return globals_configs::wireframeMode;
    }

    void setWireframeMode(bool wireframe) {
        globals_configs::wireframeMode = wireframe;
    }

    bool getAcceleratedDayNightCycle() {
        return globals_configs::acceleratedDayNightCycle;
    }

    void setAcceleratedDayNightCycle(bool accelerated) {
        globals_configs::acceleratedDayNightCycle = accelerated;
    }

    bool getShowBoundingBoxes() {
        return globals_configs::showBoundingBoxes;
    }

    void setShowBoundingBoxes(bool show) {
        globals_configs::showBoundingBoxes = show;
    }

    GLfloat getCameraSpeed() {
        return globals_configs::cameraSpeed;
    }

    void setCameraSpeed(GLfloat speed) {
        globals_configs::cameraSpeed = speed;
    }
}
