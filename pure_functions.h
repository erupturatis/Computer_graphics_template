#ifndef PURE_FUNCTIONS_H
#define PURE_FUNCTIONS_H

#include "BoundingBoxes.h"
#include "tiny_obj_loader.h"

namespace gps{
    gps::BoundingBox3D computeBoundingBox(const std::vector<gps::Vertex>& vertices);
}

#endif
