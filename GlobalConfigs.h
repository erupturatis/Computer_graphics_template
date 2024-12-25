#ifndef GLOBAL_CONFIGS_H
#define GLOBAL_CONFIGS_H

class GlobalConfigs {
public:
    static GlobalConfigs& getInstance();

    bool getShowBoundingBoxes() const;
    void setShowBoundingBoxes(bool show);

private:
    GlobalConfigs() : showBoundingBoxes(false) {}
    GlobalConfigs(const GlobalConfigs&) = delete;
    GlobalConfigs& operator=(const GlobalConfigs&) = delete;

    bool showBoundingBoxes;
    static GlobalConfigs* instance;
};

#endif // GLOBAL_CONFIGS_H
