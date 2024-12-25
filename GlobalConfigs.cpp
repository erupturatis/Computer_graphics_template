#include "GlobalConfigs.h"
#include <GL/glew.h>

namespace globals_configs
{
    bool showBoundingBoxes;
    GLfloat cameraSpeed = 0.1f;

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
