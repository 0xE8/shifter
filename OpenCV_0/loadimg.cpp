#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgproc/imgproc_c.h"

using namespace cv;
using namespace std;

int main(int, char* [])
{
	const char camaddr[]="rtsp://192.168.0.77:554/PSIA/streaming/channels/101?videoResolutionWidth=320&videoResolutionHeight=240";
	CvCapture *capture;
    Mat frame, curr, prev, curr64f, prev64f, hann;
    int key = 0;
	IplImage* fframe;
	IplImage* scaled;
    do
    {
		capture = cvCaptureFromFile(camaddr);
		frame = cvQueryFrame(capture); 

		//scaled = cvCreateImage(cvSize(320, 240), fframe->depth, fframe->nChannels); 
		//cvResize(fframe, scaled, 2); 
		//frame=fframe;
        cvtColor(frame, curr, CV_RGB2GRAY); // градаци€ серого
        if(prev.empty())
        {
            prev = curr.clone();
            createHanningWindow(hann, curr.size(), CV_64F);
        }

        prev.convertTo(prev64f, CV_64F); 
        curr.convertTo(curr64f, CV_64F);

        Point2d shift = phaseCorrelate(prev64f, curr64f, hann); // коррел€ци€
        double radius = cv::sqrt(shift.x*shift.x + shift.y*shift.y); // сила смещени€

        if(radius > 5)
        {
            Point center(curr.cols >> 1, curr.rows >> 1);
            cv::circle(frame, center, (int)radius, cv::Scalar(255, 0, 0), 3, CV_AA);
            cv::line(frame, center, Point(center.x + (int)shift.x, center.y + (int)shift.y), cv::Scalar(255, 0, 0), 3, CV_AA);
        }

        imshow("Shift shower", frame);
        key = waitKey(100);

        prev = curr.clone();
		cvReleaseCapture(&capture);
    } while((char)key != 27); // Esc to exit...

    return 0;
}