#ifndef __MOTION_STATE_HXX__
#define __MOTION_STATE_HXX__

#include <vector>

#include <opencv/cv.h>

namespace ISentry
{
    class MotionState
    {
    public:
        virtual std::vector<float> getSignals() = 0;
        virtual std::vector<cv::Mat> getFrames() = 0;
    };
};

#endif //__MOTION_STATE_HXX__
