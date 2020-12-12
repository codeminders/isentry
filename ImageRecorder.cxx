
#include <time.h>
#include <iostream>

#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>

#include "ImageRecorder.hxx"

using namespace std;
using namespace cv;

std::string ISentry::ImageRecorder::ext("jpg");

ISentry::ImageRecorder::ImageRecorder(const libconfig::Setting& cfg):MediaRecorder(),n(0)
{
    enabled = cfg["enabled"];
    dir=(const char *)(cfg["dir"]);
    record_every_n=cfg["record_every_n"];

    if(!cfg.lookupValue("max_width",max_width))
        max_width=-1;
    if(!cfg.lookupValue("max_photos",max_files))
        max_files=-1;
    running = false;
}

void ISentry::ImageRecorder::addFrame(std::pair<cv::Mat,time_t> &m)
{
    if(!running || !enabled)
        return;

    cv::Mat m_s;
    rescale(m.first, m_s);
    if(n==0)
    {
        if(max_files > 0 && files_recorded++ >= max_files)
        {
            std::cerr << "NOT writing image. Limit of " << max_files << " reached." << std::endl;
            enabled = false; // Disable self
        } else {
            string fname = getFilename(m.second);
            std::cerr << "Writing " << fname << std::endl;
            if(!imwrite(fname, m_s))
            {
                std::cerr << "Error saving to " << fname << std::endl;
            }
        }
    }
    if(++n==record_every_n)
        n=0;
}

void ISentry::ImageRecorder::start()
{
    n=0;
    FrameProcessor::start();
}


