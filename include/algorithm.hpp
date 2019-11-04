#ifndef __ALGORITHM__
    #define __ALGORITHM__

    #include "rendercontext.hpp"

    class Algorithm {
        public:
            virtual int run(RenderContext* context) = 0;
    };

#endif

