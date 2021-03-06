// Matlab style plot functions for OpenCV by Changbo (zoccob@gmail).

#include <opencv2/imgproc.hpp>

#include "cvplot.hxx"

namespace CvPlot
{

//  use anonymous namespace to hide global variables.
namespace
{
        const cv::Scalar CV_BLACK = CV_RGB(0,0,0);
        const cv::Scalar CV_WHITE = CV_RGB(255,255,255);
        const cv::Scalar CV_GREY = CV_RGB(150,150,150);

	PlotManager pm;
}


Series::Series(void)
{
	data = NULL;
	label = "";
	Clear();
}

Series::Series(const Series& s):count(s.count), label(s.label), auto_color(s.auto_color), color(s.color)
{
	data = new float[count];
	memcpy(data, s.data, count * sizeof(float));
}


Series::~Series(void)
{
	Clear();
}

void Series::Clear(void)
{
	if (data != NULL)
		delete [] data;
	data = NULL;

	count = 0;
	color = CV_BLACK;
	auto_color = true;
	label = "";
}

void Series::SetData(int n, float *p)
{
	Clear();

	count = n;
	data = p;
}

void Series::SetColor(int R, int G, int B, bool auto_color)
{
	R = R > 0 ? R : 0;
	G = G > 0 ? G : 0;
	B = B > 0 ? B : 0;
	color = CV_RGB(R, G, B);
	SetColor(color, auto_color);
}

void Series::SetColor(cv::Scalar color, bool auto_color)
{
	this->color = color;
	this->auto_color = auto_color;
}

Figure::Figure(const string name)
{
	figure_name = name;

	custom_range_y = false;
	custom_range_x = false;
	backgroud_color = CV_WHITE;
	axis_color = CV_BLACK;
	text_color = CV_BLACK;

	figure_size = cv::Size(600, 200);
	border_size = 30;

	plots.reserve(10);
}

Figure::~Figure(void)
{
}

string Figure::GetFigureName(void)
{
	return figure_name;
}

Series* Figure::Add(const Series &s)
{
	plots.push_back(s);
	return &(plots.back());
}

    
void Figure::Clear()
{
    plots.clear();
}

void Figure::Initialize()
{
	color_index = 0;

	// size of the figure
	if (figure_size.width <= border_size * 2 + 100)
		figure_size.width = border_size * 2 + 100;
	if (figure_size.height <= border_size * 2 + 200)
		figure_size.height = border_size * 2 + 200;

    if(!custom_range_y)
    {
        y_max = FLT_MIN;
        y_min = FLT_MAX;
    }

	x_max = 0;
	x_min = 0;

	// find maximum/minimum of axes
	for (vector<Series>::iterator iter = plots.begin();
		iter != plots.end();
		iter++)
	{
        if(!custom_range_y)
        {
            float *p = iter->data;
            for (unsigned int i=0; i < iter->count; i++)
            {
                float v = p[i];
                if (v < y_min)
                    y_min = v;
                if (v > y_max)
                    y_max = v;
            }
        }

		if (x_max < iter->count)
			x_max = iter->count;
	}

	// calculate zoom scale
	// set to 2 if y range is too small
	float y_range = y_max - y_min;
	float eps = 1e-9f;
	if (y_range <= eps)
	{
		y_range = 1;
		y_min = y_max / 2;
		y_max = y_max * 3 / 2;
	}

	x_scale = 1.0f;
	if (x_max - x_min > 1)
		x_scale = (float)(figure_size.width - border_size * 2) / (x_max - x_min);

	y_scale = (float)(figure_size.height - border_size * 2) / y_range;
}

cv::Scalar Figure::GetAutoColor()
{
	// 	change color for each curve.
	cv::Scalar col;

	switch (color_index)
	{
	case 1:
		col = CV_RGB(60,60,255);	// light-blue
		break;
	case 2:
		col = CV_RGB(60,255,60);	// light-green
		break;
	case 3:	
		col = CV_RGB(255,60,40);	// light-red
		break;
	case 4:
		col = CV_RGB(0,210,210);	// blue-green
		break;
	case 5:
		col = CV_RGB(180,210,0);	// red-green
		break;
	case 6:
		col = CV_RGB(210,0,180);	// red-blue
		break;
	case 7:
		col = CV_RGB(0,0,185);		// dark-blue
		break;
	case 8:
		col = CV_RGB(0,185,0);		// dark-green
		break;
	case 9:
		col = CV_RGB(185,0,0);		// dark-red
		break;
	default:
		col =  CV_RGB(200,200,200);	// grey
		color_index = 0;
	}
	color_index++;
	return col;
}

void Figure::DrawAxis(cv::Mat &output)
{
	int bs = border_size;		
	int h = figure_size.height;
	int w = figure_size.width;

	// size of graph
	int gh = h - bs * 2;
	//int gw = w - bs * 2;

	// draw the horizontal and vertical axis
	// let x, y axies cross at zero if possible.
	float y_ref = y_min;
	if ((y_max > 0) && (y_min <= 0))
		y_ref = 0;

	int x_axis_pos = h - bs - cvRound((y_ref - y_min) * y_scale);
        
        cv::line(output,
               cv::Point2i(bs,     x_axis_pos), 
               cv::Point2i(w - bs, x_axis_pos),
               axis_color);
        cv::line(output,
               cv::Point2i(bs, h - bs), 
               cv::Point2i(bs, h - bs - gh),
               axis_color);

	int chw = 6, chh = 10;
	char text[16];

	// y max
	if ((y_max - y_ref) > 0.05 * (y_max - y_min))
	{
		snprintf(text, sizeof(text)-1, "%.1f", y_max);
                cv::putText(output, text,
                            cv::Point(bs / 5, bs - chh / 2),
                            cv::FONT_HERSHEY_PLAIN,
                            0.55,
                            text_color,
                            1
                );
	}
	// y min
	if ((y_ref - y_min) > 0.05 * (y_max - y_min))
	{
		snprintf(text, sizeof(text)-1, "%.1f", y_min);
		cv::putText(output, text, cv::Point(bs / 5, h - bs + chh),
                            cv::FONT_HERSHEY_PLAIN,
                            0.55,
                            text_color,
                            1
                );
	}

	// x axis
	snprintf(text, sizeof(text)-1, "%.1f", y_ref);
        cv::putText(output, text, cv::Point(bs / 5, x_axis_pos + chh / 2),
                            cv::FONT_HERSHEY_PLAIN,
                            0.55,
                            text_color,
                            1
                );

	// Write the scale of the x axis
	snprintf(text, sizeof(text)-1, "%.0f", x_max );
        cv::putText(output, text, cv::Point(w - bs - strlen(text) * chw, x_axis_pos + chh), 
                            cv::FONT_HERSHEY_PLAIN,
                            0.55,
                            text_color,
                            1
                );

	// x min
	snprintf(text, sizeof(text)-1, "%.0f", x_min );
        cv::putText(output, text, cv::Point(bs, x_axis_pos + chh), 
                            cv::FONT_HERSHEY_PLAIN,
                            0.55,
                            text_color,
                            1
                );


}
void Figure::DrawPlots(cv::Mat &output)
{
	int bs = border_size;		
	int h = figure_size.height;
	//int w = figure_size.width;

	// draw the curves
	for (vector<Series>::iterator iter = plots.begin();
		iter != plots.end();
		iter++)
	{
		float *p = iter->data;

		// automatically change curve color
		if (iter->auto_color == true)
			iter->SetColor(GetAutoColor());

                cv::Point prev_point;
		for (unsigned int i=0; i<iter->count; i++)
		{
			int y = cvRound(( p[i] - y_min) * y_scale);
			int x = cvRound((   i  - x_min) * x_scale);
                        cv::Point next_point = cv::Point(bs + x, h - (bs + y));
                        cv::circle(output, next_point, 1, iter->color, 1);
			
			// draw a line between two points
			if (i >= 1)
                            cv::line(output, prev_point, next_point, iter->color, 1, LINE_AA);
			prev_point = next_point;
		}
	}

}

void Figure::DrawLabels(cv::Mat &output, int posx, int posy)
{

	// character size
	//int chw = 6;
        int chh = 8;

	for (vector<Series>::iterator iter = plots.begin();
		iter != plots.end();
		iter++)
	{
		string lbl = iter->label;
		// draw label if one is available
		if (lbl.length() > 0)
		{
                    cv::line(output, cv::Point(posx, posy - chh / 2), cv::Point(posx + 15, posy - chh / 2),
                             iter->color, 2, LINE_AA);

                    cv::putText(output, lbl.c_str(), cv::Point(posx + 20, posy),
                                cv::FONT_HERSHEY_PLAIN,
                                0.55,
                                iter->color,
                                1
                    );

                    posy += int(chh * 1.5);
		}
	}

}

// whole process of draw a figure.
void Figure::Show()
{
	Initialize();

        cv::Mat output(figure_size, CV_8UC3, backgroud_color);

	DrawAxis(output);

	DrawPlots(output);

	DrawLabels(output, figure_size.width - 100, 10);

        cv::namedWindow(figure_name, 1);
        cv::imshow(figure_name, output);

        cv::waitKey(1);
}



bool PlotManager::HasFigure(string wnd)
{
	return false;	
}

// search a named window, return null if not found.
Figure* PlotManager::FindFigure(string wnd)
{
	for(vector<Figure>::iterator iter = figure_list.begin();
		iter != figure_list.end();
		iter++)
	{
		if (iter->GetFigureName() == wnd)
			return &(*iter);
	}
	return NULL;
}

// plot a new curve, if a figure of the specified figure name already exists,
// the curve will be plot on that figure; if not, a new figure will be created.
void PlotManager::Plot(const string figure_name, const float *p, int count, int step,
					   int R, int G, int B)
{
	if (count < 1)
		return;

	if (step <= 0)
		step = 1;

	// copy data and create a series format.
	float *data_copy = new float[count];

	for (int i = 0; i < count; i++)
		*(data_copy + i) = *(p + step * i);

	Series s;
	s.SetData(count, data_copy);

	if ((R > 0) || (G > 0) || (B > 0))
		s.SetColor(R, G, B, false);

	// search the named window and create one if none was found
	active_figure = FindFigure(figure_name);
	if ( active_figure == NULL)
	{
		active_figure = AddFigure(figure_name);
		if (active_figure == NULL)
			exit(-1);
	}

	active_series = active_figure->Add(s);
	active_figure->Show();

}

// add a label to the most recently added curve
void PlotManager::Label(string lbl)
{
	if((active_series!=NULL) && (active_figure != NULL))
	{
		active_series->label = lbl;
		active_figure->Show();
	}
}

// plot a new curve, if a figure of the specified figure name already exists,
// the curve will be plot on that figure; if not, a new figure will be created.
// static method
template<typename T>
void plot(const string figure_name, const T* p, int count, int step,
		  int R, int G, int B)
{
	if (step <= 0)
		step = 1;

	float  *data_copy = new float[count * step];

	float   *dst = data_copy;
	const T *src = p;

	for (int i = 0; i < count * step; i++)
	{
		*dst = (float)(*src);
		dst++;
		src++;
	}

	pm.Plot(figure_name, data_copy, count, step, R, G, B);

	delete [] data_copy;
}


// add a label to the most recently added curve
// static method
void label(string lbl)
{
	pm.Label(lbl);
}

PlotManager *getPlotManager() { return &pm; }

template
void plot(const string figure_name, const unsigned char* p, int count, int step,
		  int R, int G, int B);

template
void plot(const string figure_name, const int* p, int count, int step,
		  int R, int G, int B);

template
void plot(const string figure_name, const short* p, int count, int step,
		  int R, int G, int B);

template
void plot(const string figure_name, const float* p, int count, int step,
		  int R, int G, int B);

template
void plot(const string figure_name, const double* p, int count, int step,
		  int R, int G, int B);

};
