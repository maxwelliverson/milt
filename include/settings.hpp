#include "math_base.hpp"
#include "algorithm.hpp"
#include <functional>

#ifndef __SETTINGS__
    #define __SETTINGS__

    struct RenderSettings {
        size_t threads;
        bool enable_gpu;
        size_t pixel_samples;
        //RNG rngType;
        //Distribution samplingDistribution;

        Algorithm *algorithm;

        floatPrecision f_precision;
    };


    struct CameraSettings {
        size_t im_width;
        size_t im_height;
        temp_float* wavelengths;

        vec3 position;
        vec3 direction;
        temp_float rotation;

        temp_float aperture;
        temp_float focal_length;
        temp_float fov;
        //Lens lens;

    };

    class ViewerSettings {
        size_t width;
        size_t height;
        size_t bitdepth;
        //function<int, temp_float...> composing_function;
        temp_float min_luminosity;
        temp_float max_luminosity;
    };

    struct ExternalSettings {
        ViewerSettings viewerSettings;
        RenderSettings renderSettings;
        CameraSettings cameraSettings;
    };
#endif