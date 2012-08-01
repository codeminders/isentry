#include "FrameProcessor.hxx"

using namespace std;

void ISentry::FrameProcessor::start() 
{ 
    running=true;
    for(vector<FrameProcessor*>::const_iterator i = children.begin();
        i!=children.end();
        i++)
    {
        (*i)->start();
    }
}

void ISentry::FrameProcessor::stop() 
{ 
    running=false;
    for(vector<FrameProcessor*>::const_iterator i = children.begin();
        i!=children.end();
        i++)
    {
        (*i)->stop();
    }
}

void ISentry::FrameProcessor::addFrame(std::pair<cv::Mat,time_t> &m)
{
    if(!running)
        return;

    for(vector<FrameProcessor*>::const_iterator i = children.begin();
        i!=children.end();
        i++)
    {
        (*i)->addFrame(m);
    }
}

void ISentry::FrameProcessor::addChild(FrameProcessor *c)
{
    children.push_back(c);
}

void ISentry::FrameProcessor::removeChild(FrameProcessor *c)
{
    for(vector<FrameProcessor*>::iterator i = children.begin();
        i!=children.end();)
    {
        if(*i == c)
            i = children.erase(i);
        else
            i++;
    }
}

void ISentry::FrameProcessor::clearChildren()
{
    children.clear();
}
