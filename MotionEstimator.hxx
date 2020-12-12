#ifndef __MOTION_ESTIMATOR_HXX__
#define __MOTION_ESTIMATOR_HXX__

#include <iostream>
#include <deque>

#include <libconfig.h++>

#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>

#include "FrameProcessor.hxx"
#include "MotionState.hxx"

namespace ISentry
{
    class MotionEstimator:public FrameProcessor, public MotionState
    {
    public:

        MotionEstimator(const libconfig::Setting& cfg);

        void reset();
        virtual void addFrame(std::pair<cv::Mat,time_t> &m);

        virtual std::vector<float> getSignals();
        virtual std::vector<cv::Mat> getFrames();


    protected:

        const cv::Mat &getLastScaledFrame() const { return small;}
        const cv::Mat &getLastDiff() const { return diff;}

        float getLastSmoothedMotion() const;
        float getLastRawMotion() const;
    
        void saveFrame(cv::Mat &f);
        void saveRawMotion(float v);
    
        cv::Mat diff;
        cv::Mat small;

        size_t detection_window_size;
        int internal_frame_width;
        size_t smoothing_window_size;

        std::deque<cv::Mat> buf;
        std::vector<float> raw_motion;

    private:
    };

};

#endif //__MOTION_ESTIMATOR_HXX__
