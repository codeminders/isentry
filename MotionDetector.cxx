#include "MotionDetector.hxx"

using namespace std;

ISentry::MotionDetector::MotionDetector(MotionState *_tap, const libconfig::Setting& cfg):
    tap(_tap),in_motion(FALSE)
{
    threshold = cfg["threshold"];
    backup_frames = cfg["backup_frames"];
    running = false;
}

ISentry::MotionDetector::~MotionDetector()
{
    if(in_motion)
    {
        for(vector<FrameProcessor*>::const_iterator i = children.begin();
            i!=children.end();
            i++)
        {
            (*i)->stop();
        }
    }
}

void ISentry::MotionDetector::addFrame(std::pair<cv::Mat,time_t> &m)
{
    if(!running)
        return;
        
    if(threshold>0 && tap->getSignals()[0]>threshold)
    {
        std::cerr << "MOTION!\n";
        for(vector<FrameProcessor*>::const_iterator i = children.begin();
            i!=children.end();
            i++)
        {
            if(!in_motion)
                (*i)->start();
            // dump saved frames
            if(backup_frames>0)
            {
                for(deque<std::pair<cv::Mat,time_t> >::reverse_iterator j = buf.rbegin();
                    j!=buf.rend();
                    j++)
                {
                    (*i)->addFrame(*j);
                }
            } 
            buf.clear();
            // And finally current frame
            (*i)->addFrame(m);
        }
        in_motion = TRUE;
    } else
    {
        if(in_motion)
        {
            for(vector<FrameProcessor*>::const_iterator i = children.begin();
                i!=children.end();
                i++)
            {
                (*i)->stop();
            }
            in_motion = FALSE;
        }
        if(backup_frames>0)
        {
            while(buf.size()>=backup_frames)
                buf.pop_back();
            buf.insert(buf.begin(),m);
        }
    }
}

