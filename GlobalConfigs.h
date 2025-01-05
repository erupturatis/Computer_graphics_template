#ifndef GLOBAL_CONFIGS_H
#define GLOBAL_CONFIGS_H

#include <GL/glew.h>

namespace globals_configs
{
    bool getShowBoundingBoxes();
    void setShowBoundingBoxes(bool show);

    GLfloat getCameraSpeed();
    void setCameraSpeed(GLfloat speed);

    bool getWireframeMode();
    void setWireframeMode(bool wireframe);

    bool getPolygonalMode();
    void setPolygonalMode(bool polygonal);

    bool getVertexMode();
    void setVertexMode(bool wireframe);

    bool getAcceleratedDayNightCycle();
    void setAcceleratedDayNightCycle(bool accelerated);

    bool getApplyFog();
    void setApplyFog(bool fog);

    bool getApplyCollisions();
    void setApplyCollisions(bool collisions);
}

#endif // GLOBAL_CONFIGS_H
