#ifndef __MOTION_STATE_HXX__
#define __MOTION_STATE_HXX__

#include <vector>

#include <opencv2/opencv.hpp>

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
