// Matlab style plot functions for OpenCV by Changbo (zoccob@gmail).
// plot and label:
//
// template<typename T>
// void plot(const string figure_name, const T* p, int count, int step = 1,
//		     int R = -1, int G = -1, int B = -1);
//
// figure_name: required. multiple calls of this function with same figure_name
//              plots multiple curves on a single graph.
// p          : required. pointer to data.
// count      : required. number of data.
// step       : optional. step between data of two points, default 1.
// R, G,B     : optional. assign a color to the curve.
//              if not assigned, the curve will be assigned a unique color automatically.
//
// void label(string lbl):
//
// label the most recently added curve with lbl.
//
// 
//
//

#pragma once

#if WIN32 
#define snprintf sprintf_s 
#endif 

#include <vector>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>

using namespace cv;
using namespace std;

namespace CvPlot
{
	// A curve.
	class Series
	{
	public:

		// number of points
		unsigned int count;
		float *data;
		// name of the curve
		string label;

		// allow automatic curve color
		bool auto_color;
                cv::Scalar color;

		Series(void);
		Series(const Series& s);
		~Series(void);

		// release memory
		void Clear();

		void SetData(int n, float *p);

		void SetColor(cv::Scalar color, bool auto_color = true);
		void SetColor(int R, int G, int B, bool auto_color = true);
	};

	// a figure comprises of several curves
	class Figure
	{
	private:
		// window name
		string figure_name;
                cv::Size figure_size;

		// margin size
		int    border_size;

		cv::Scalar backgroud_color;
		cv::Scalar axis_color;
		cv::Scalar text_color;

		// several curves
		vector<Series> plots;

		// manual or automatic range
		bool custom_range_y;
		float y_max;
		float y_min;

		float y_scale;

		bool custom_range_x;
		float x_max;
		float x_min;

		float x_scale;

		// automatically change color for each curve
		int color_index;

	public:
		Figure(const string name);
		~Figure();

		string GetFigureName();
		Series* Add(const Series &s);
                void DrawLabels(cv::Mat &output, int posx, int posy);

		// show plot window
		void Show();

        void Clear();

        void setCustomYRange(float ymin, float ymax)
        {
            custom_range_y=true;
            y_max=ymax;
            y_min=ymin;
        }

        
	private:
		Figure();
		void DrawAxis(cv::Mat &output);
		void DrawPlots(cv::Mat &output);

		// call before plot
		void Initialize();
		cv::Scalar GetAutoColor();
		
	};

	// manage plot windows
	class PlotManager
	{
	private:
		vector<Figure> figure_list;
		Series *active_series;
		Figure *active_figure;

	public:

		// now useless
		bool HasFigure(string wnd);
		Figure* FindFigure(string wnd);
		Figure* AddFigure(string wnd)
        {
            Figure new_figure(wnd);
            figure_list.push_back(wnd);
            return FindFigure(wnd);
        }

		void Plot(const string figure_name, const float* p, int count, int step,
				  int R, int G, int B);

		void Label(string lbl);

	};

	// handle different data types; static mathods;

	template<typename T>
	void plot(const string figure_name, const T* p, int count, int step = 1,
			  int R = -1, int G = -1, int B = -1);

	void label(string lbl);

    PlotManager *getPlotManager();
};
