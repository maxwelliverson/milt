#ifndef __RENDER_TYPES__
    #define __RENDER_TYPES__

    #include "math_base.hpp"

    struct Ray {
        vec3 position;
        vec3 direction;
        temp_float wavelength;
    };

    class Material {
        public:
            virtual temp_float wavelenPercent(const temp_float* const wavelength) = 0;
            virtual Ray* reflectRay(const Ray* const inRay, const vec3* const norm) = 0;
    };

    struct Node {
        vec3 pos;
        Material* const material;
    };

    class RenderScene {
        public:
            virtual Ray* getBackRay(const size_t* const pixel) = 0;
            virtual Ray* getBackRay(const size_t* const pixel, const temp_float* const wavelength) = 0;
            virtual Ray* getForwardsRay() = 0;
            virtual Ray* getForwardsRay(const temp_float* const wavelength) = 0;
            virtual Node** getNodes() = 0;

            virtual vec3* traverse(const Ray* const ray) = 0;
            virtual Ray* nextRay(const Ray* const ray, const Node* const node) = 0;

    };
#endif