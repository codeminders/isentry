#ifndef __MOTION_DETECTOR_HXX__
#define __MOTION_DETECTOR_HXX__

#include <deque>

#include <libconfig.h++>

#include "MotionState.hxx"
#include "FrameProcessor.hxx"

namespace ISentry
{
    /**
     * Detects motion and forwards frame to children only if detected.
     * Additionally calls start/stop methods on children depending on
     * motion state.
     */
    class MotionDetector:public FrameProcessor
    {
    public:
        
        MotionDetector(MotionState *tap, const libconfig::Setting& cfg);
        virtual ~MotionDetector();
        
        void setThreshold(float th) { threshold=th; }
        
        // overloaded from FrameProcessor
        virtual void addFrame(std::pair<cv::Mat,time_t> &m);

    protected:
        MotionState *tap;
        float threshold;

    private:
        // pre-motion 
        int backup_frames;
        std::deque<std::pair<cv::Mat,time_t> > buf;
        bool in_motion;

    };

};
#endif // __MOTION_DETECTOR_HXX__
