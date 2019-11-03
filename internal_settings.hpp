#include "settings.hpp"
#include <string>


struct InternalSettings {
    //Maybe use promises here?
    bool locked;
    RenderSettings render_vars;
    CameraSettings camera_vars;
};