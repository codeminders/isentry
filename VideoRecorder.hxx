#ifndef __VIDEO_RECORDER_HXX__
#define __VIDEO_RECORDER_HXX__

#include <time.h>

#include <libconfig.h++>

#include <cv.h>
#include <highgui/highgui.hpp>

#include "FrameProcessor.hxx"
#include "MediaRecorder.hxx"

namespace ISentry
{
    class VideoRecorder:public FrameProcessor, public MediaRecorder
    {
    public:

        VideoRecorder(const libconfig::Setting& cfg);

        // overloaded from FrameProcessor
        virtual void addFrame(std::pair<cv::Mat,time_t> &m);
        virtual void start();
        virtual void stop();

        virtual const std::string getExt() const { return ext;}

    private:

        static std::string ext;
        int n;
        int max_duration;

        cv::VideoWriter *writer;

    };

};
#endif // __VIDEO_RECORDER_HXX__
