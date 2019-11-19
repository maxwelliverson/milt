#ifndef __ALGORITHM__
    #define __ALGORITHM__

    class RenderContext;

    class Algorithm {
        public:
            virtual int run(RenderContext* context) = 0;
    };

#endif

