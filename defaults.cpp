#include "algorithm.hpp"
#include "internal_settings.hpp"
#include "rendercontext.hpp"
#include "render_types.hpp"

template<size_t... dims>
class Matrix {

};

class IversonMLT : public Algorithm {
    public:
        size_t sample_count;
        size_t node_count;
        size_t max_memory;
        size_t octree_overflow;
        size_t pixel_samples;
        size_t roulette_start;
        int run(RenderContext* context){
            
        }
    private:
        temp_float roulette(size_t count){
            return (count / roulette_start) * 1/temp_float(6);
        }



};