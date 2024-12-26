#ifndef SCENE_H
#define SCENE_H
#include "Model3D.hpp"

namespace scene
{
    void registerModel(globals::Model3D& model);
    void recalculateNormals();
    std::vector<globals::Model3D*>& getModels3D();
}

#endif //SCENE_H
