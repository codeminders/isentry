#ifndef __IMAGE_RECORDER_HXX__
#define __IMAGE_RECORDER_HXX__

#include <time.h>

#include <libconfig.h++>

#include "FrameProcessor.hxx"
#include "MediaRecorder.hxx"

namespace ISentry
{
    class ImageRecorder:public FrameProcessor, public MediaRecorder
    {
    public:

        ImageRecorder(const libconfig::Setting& cfg);

        // overloaded from FrameProcessor
        virtual void addFrame(std::pair<cv::Mat,time_t> &m);
        virtual void start();        

        virtual const std::string getExt() const { return ext;}

    private:

        static std::string ext;
        int record_every_n;
        int n;

    };

};
#endif // __IMAGE_RECORDER_HXX__
