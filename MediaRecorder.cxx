#include "MediaRecorder.hxx"

#include <opencv/cv.h>
#include <opencv2/highgui/highgui.hpp>

#include "ImageRecorder.hxx"

using namespace std;
using namespace cv;

ISentry::MediaRecorder::MediaRecorder()
    :enabled(FALSE),last_time(0),seq(0),files_recorded(0)
{
}

void ISentry::MediaRecorder::rescale(const cv::Mat &m,cv::Mat &m_s)
{
    if(max_width==-1 || m.cols<=max_width)
    {
        m_s=m;
        return;
    }

    float scaler = m.cols/max_width;
    Size newSize(m.cols/scaler, m.rows/scaler);
    resize(m, m_s, newSize, 0, 0, CV_INTER_LINEAR);
}

std::string ISentry::MediaRecorder::getFilename(time_t rawtime)
{
    struct tm * timeinfo = localtime (&rawtime);
    
    char buf[80];
    if(rawtime==last_time)
    {
        size_t p = strftime(buf,sizeof(buf),"%Y-%m-%d-%H-%M-%S",timeinfo);
        sprintf(buf+p,"-%d.", seq+1);
        seq++;
    } else
    {
        strftime(buf,sizeof(buf),"%Y-%m-%d-%H-%M-%S.", timeinfo);
        last_time = rawtime;
        seq=0;
    }
    if(dir.empty())
        return buf+getExt();
    else
        return dir+"/"+buf+getExt();
}
