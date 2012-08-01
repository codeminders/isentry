#ifndef __FRAME_PROCESSOR_HXX__
#define __FRAME_PROCESSOR_HXX__

#include <vector>
#include <cv.h>

namespace ISentry
{
    class FrameProcessor
    {
    public:

        FrameProcessor():running(false) {}
        virtual ~FrameProcessor() {}

        virtual void start();
        virtual void stop();

        virtual void addFrame(std::pair<cv::Mat,time_t> &m);
        
        // Managing processing tree
        virtual void addChild(FrameProcessor *c);
        virtual void removeChild(FrameProcessor *c);
        virtual void clearChildren();

    protected:
        bool running;
        std::vector<FrameProcessor *> children;

    };
};
#endif //__FRAME_PROCESSOR_HXX__
