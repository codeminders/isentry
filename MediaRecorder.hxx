#ifndef __MEDIA_RECORDER_HXX__
#define __MEDIA_RECORDER_HXX__

#include <time.h>

#include <opencv2/opencv.hpp>

namespace ISentry
{
    class MediaRecorder
    {
       
    protected:
        MediaRecorder();

        void rescale(const cv::Mat &m,cv::Mat &m_s);

        virtual const std::string getExt() const = 0;
        
        std::string getFilename(time_t rawtime);
        

        std::string dir;
        int max_width;
        int max_files;
        bool enabled;

        time_t last_time;
        int seq;
        int files_recorded;
    };

};

#endif // __MEDIA_RECORDER_HXX__


