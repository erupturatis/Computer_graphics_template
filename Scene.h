#ifndef SCENE_H
#define SCENE_H
#include "Model3D.hpp"

namespace scene
{
    void registerModel(globals::Model3D& model);
    void recalculateNormals();
}

#endif //SCENE_H
