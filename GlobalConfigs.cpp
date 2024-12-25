#include "GlobalConfigs.h"

GlobalConfigs* GlobalConfigs::instance = nullptr;

GlobalConfigs& GlobalConfigs::getInstance() {
    if (instance == nullptr) {
        instance = new GlobalConfigs();
    }
    return *instance;
}

bool GlobalConfigs::getShowBoundingBoxes() const {
    return showBoundingBoxes;
}

void GlobalConfigs::setShowBoundingBoxes(bool show) {
    showBoundingBoxes = show;
}
