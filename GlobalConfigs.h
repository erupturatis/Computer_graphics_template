#ifndef GLOBAL_CONFIGS_H
#define GLOBAL_CONFIGS_H

#include <GL/glew.h>

namespace globals_configs
{
    bool getShowBoundingBoxes();
    void setShowBoundingBoxes(bool show);

    GLfloat getCameraSpeed();
    void setCameraSpeed(GLfloat speed);
}

#endif // GLOBAL_CONFIGS_H
