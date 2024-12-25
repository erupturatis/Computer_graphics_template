#include "Scene.h"
#include <vector>

namespace scene
{
    std::vector<globals::Model3D*> models;

    void registerModel(globals::Model3D& model) {
        models.push_back(&model);
    }

    void recalculateNormals() {
        for (auto* model : models) {
            model->recalculateNormal();
        }
    }
}
