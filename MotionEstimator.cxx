
#include <cmath>
#include <limits>

#include "MotionEstimator.hxx"

#include <opencv2/imgproc.hpp>

using namespace cv;
using namespace std;

/**
 * Enable linear correction of brightness to handle "flicker" effect
 * due auto-exposure correction on some cameras
 */
#define LINEAR_CORRECTION 

ISentry::MotionEstimator::MotionEstimator(const libconfig::Setting& cfg)
{
    detection_window_size = (int)cfg["detection_window_size"];
    internal_frame_width  = cfg["internal_frame_width"];
    smoothing_window_size = (int)cfg["smoothing_window_size"];
}

void ISentry::MotionEstimator::reset()
{
    buf.clear();
}

void ISentry::MotionEstimator::addFrame(std::pair<cv::Mat,time_t> &tframe)
{
    if(!running)
        return;
    
    Mat &frame=tframe.first;

    Mat bw;
    cvtColor(frame, bw, CV_BGR2GRAY);
    
    float scaler = bw.cols/internal_frame_width;
    Size newSize(bw.cols/scaler, bw.rows/scaler);
    
    Mat smallf;
    resize(bw, small, newSize, 0, 0, CV_INTER_LINEAR);
    small.convertTo(smallf,DataType<float>::type);

    if(buf.empty())
        buf.push_back(smallf);

#ifdef LINEAR_CORRECTION 
    int smalllen = newSize.width*newSize.height;
    Mat smallv = smallf.reshape(1, smalllen);
    int n=0;
    Mat small1 = Mat::ones(smalllen,1, DataType<float>::type);
#endif
    
    Mat avg(newSize, DataType<float>::type, (float)0);
    for(std::deque<Mat>::const_iterator i=buf.begin(); i != buf.end(); i++)
    {
#ifdef LINEAR_CORRECTION 
        const Mat &fm = (*i);
        Mat fv = fm.reshape(1, smalllen);
        Mat fm1;
        hconcat(fv, small1, fm1);
        Mat fit;
        if(!solve(fm1, smallv, fit, DECOMP_SVD))
        {
            std::cerr  << "SVD error\n";
            // Add to average w/o linear transform
            add(avg, fm/buf.size(), (Mat&)avg);
        } else
        {
            float k=fit.at<float>(0);
            float b=fit.at<float>(1);
            
            if(n==0)
                std::cerr  << "SVD OK " << k << ", " << b <<"\n";
            Mat ft;
            // linear transofrm
            fm.convertTo(ft,-1,k,b);
            // Add to average 
            add(avg, ft/buf.size(), (Mat&)avg);
        }
        n++;
#else
        add(avg, (*i)/buf.size(), (Mat&)avg);
#endif
    }
    
    saveFrame(smallf);

    diff = avg-smallf;

    Scalar m = mean(smallf);
    if(!isnan(m[0]))
    {
        double a = smallf.rows*smallf.cols;
        Scalar mse = sum(diff.mul(diff))/a;
        float rm = sqrt(mse[0])/m[0];
        saveRawMotion(rm);
    } else
    {
        saveRawMotion(std::numeric_limits<float>::quiet_NaN());
    }
}


void ISentry::MotionEstimator::saveFrame(cv::Mat &f)
{
    while(buf.size()>=detection_window_size)
        buf.pop_front();
    buf.push_back(f);
}

void ISentry::MotionEstimator::saveRawMotion(float v)
{
    while(raw_motion.size()>=smoothing_window_size)
        raw_motion.pop_back();
    raw_motion.insert(raw_motion.begin(),v);
}

float ISentry::MotionEstimator::getLastRawMotion() const
{
    return raw_motion.front();
}

float ISentry::MotionEstimator::getLastSmoothedMotion() const
{
    int n = 0;
    float x,sumxx,sumyy,sumxy,sumx,sumy;
    x=sumxx=sumyy=sumxy=sumx=sumy=0;
    for(vector<float>::const_iterator i = raw_motion.begin();i!=raw_motion.end();i++)
    {
        float y=*i;
        x = x + 1;
        if(isnan(y))
            continue;
        sumx = sumx + x;
        sumy = sumy + y;
        sumxx = sumxx + x * x;
        sumyy = sumyy + y * y;
        sumxy = sumxy + x * y;
        n = n + 1;
    }

	if(n < 2)
    {
        //non enough data to smooth. return 0
        return 0;
    }

    float sxx = sumxx - sumx * sumx / n;
    float sxy = sumxy - sumx * sumy / n;
    float b = sxy / sxx;
    float a = (sumy - b * sumx) / n;

    float v = a + b * x;
    return v>0?v:0;
}

std::vector<float> ISentry::MotionEstimator::getSignals()
{
    vector<float> res;
    res.push_back(getLastRawMotion());
    res.push_back(getLastSmoothedMotion());
    return res;
}

std::vector<cv::Mat> ISentry::MotionEstimator::getFrames()
{
    vector<cv::Mat> res;
    res.push_back(getLastScaledFrame());
    res.push_back(getLastDiff());
    return res;
}

