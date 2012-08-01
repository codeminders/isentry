#include <cv.h>
#include <highgui.h>
#include <iostream>

#include "iSentryConfig.hxx"
#include "MotionDetector.hxx"
#include "MotionEstimator.hxx"
#include "ImageRecorder.hxx"
#include "VideoRecorder.hxx"
#include "cvplot.hxx"

using namespace cv;
using namespace std;
using namespace ISentry;

#define CONFIG_FILE_NAME "isentry.cfg"

class SimplePreview:public FrameProcessor
{

public:

    SimplePreview(MotionState *_ms, const libconfig::Setting& cfg) : ms(_ms)
    {
        nframes = cfg["frames"];
        reset();
        namedWindow("preview",1);
        namedWindow("diff",1);
        CvPlot::Figure *f = CvPlot::getPlotManager()->AddFigure("motion");
        f->setCustomYRange(0,1);
        float threshold;
        if(cfg.lookupValue("threshold",threshold))
        {
            for(int i=0;i<nframes;i++)
                threshold_points.push_back(threshold);
        }
    }

    virtual void stop()
    {
        FrameProcessor::stop();
        reset();
    }

    virtual void addFrame(std::pair<cv::Mat,time_t> &m)
    {
        if(!ms || !running)
            return;
         
        std::vector<float> signals = ms->getSignals();
        std::vector<cv::Mat> frames = ms->getFrames();
        
        while(motion.size()>=nframes)
            motion.pop_back();
        motion.insert(motion.begin(),signals[0]);

        while(raw_motion.size()>=nframes)
            raw_motion.pop_back();
        raw_motion.insert(raw_motion.begin(),signals[1]);

        if(!motion.empty())
        {
            CvPlot::Figure *f = CvPlot::getPlotManager()->FindFigure("motion");
            f->Clear();
            CvPlot::plot("motion", &(*(motion.begin())), motion.size());
            CvPlot::plot("motion", &(*(raw_motion.begin())), raw_motion.size());
            if(!threshold_points.empty())
                CvPlot::plot("motion", &(*(threshold_points.begin())), threshold_points.size());
        }
        imshow("preview", frames[0]);
        if(signals[0]>0)
        {
            const Mat &diff = frames[1];
            Mat idiff;
            diff.convertTo(idiff,m.first.type());
            imshow("diff", idiff);
        }
    }

private:

    void reset()
    {
        motion.clear();
        raw_motion.clear();
        for(int i=0;i<nframes;i++)
        {
            motion.insert(motion.begin(),0);
            raw_motion.insert(raw_motion.begin(),0);
        }
    }

    vector<float> motion;
    vector<float> raw_motion;
    vector<float> threshold_points;
    MotionState *ms;    
    int nframes;
};

int main(int ac, char**av)
{
    try
    {
        if(ac==1)
            iSentryConfig::getInstance()->readFile(CONFIG_FILE_NAME);
        else if(ac==2)
            iSentryConfig::getInstance()->readFile(av[1]);
        else
        {
            std::cerr << "Usage: ./iSentry [config_filename]" << std::endl;
            exit(1);
        }
    } catch(const libconfig::FileIOException &fioex)
    {
        std::cerr << "Error reading config file." << std::endl;
        exit(2);
    }

    const libconfig::Setting& cfg = iSentryConfig::getInstance()->getRoot();

    VideoCapture cap((int)(cfg["application"]["camera_device_id"])); // open the default camera
    if(!cap.isOpened())  // check if we succeeded
        return -1;

    MotionEstimator me(cfg["motion_detection"]);
    MotionDetector md(&me, cfg["motion_detection"]);

    ImageRecorder irec(cfg["image_recording"]);
    if(cfg["image_recording"]["enabled"])
        md.addChild(&irec);

    VideoRecorder vrec(cfg["video_recording"]);
    if(cfg["video_recording"]["enabled"])
        md.addChild(&vrec);

    FrameProcessor isentry;
    isentry.addChild(&me);
    isentry.addChild(&md);

    SimplePreview *preview;
    bool show_preview = false;
    cfg["application"].lookupValue("show_GUI",show_preview);
    if(show_preview)
    {
        preview = new SimplePreview(&me,cfg["preview"]);
        isentry.addChild(preview);
    } else
        preview = NULL;

    isentry.start();

    for(;;)
    {
        Mat frame;
        cap >> frame; // get a new frame from camera
        pair<cv::Mat,time_t> tframe = make_pair(frame,time(NULL));
        isentry.addFrame(tframe);

        int key = waitKey(30);
        if(key >= 0)
        {
            if(key=='q')
                isentry.stop();
            else if(key=='a')
                isentry.start();
            else
                break;
        }
    }

    isentry.clearChildren();
    delete preview;

    // the camera will be deinitialized automatically in VideoCapture destructor
    return 0;
}

