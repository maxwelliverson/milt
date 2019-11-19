#ifndef __CONTEXT__
    #define __CONTEXT__

    #include "internal_settings.hpp"

    class RenderContext {
        RenderScene *scene;
        InternalSettings vars;
        ExternalSettings user_settings;

        bool changeSetting(std::string var_name);
        temp_float* render(){
            vars.locked = true;
            vars.render_vars.algorithm->run(this);
            vars.camera_vars = user_settings.cameraSettings;
            vars.render_vars = user_settings.renderSettings;
        }
        temp_float* stop();
    };
#endif

