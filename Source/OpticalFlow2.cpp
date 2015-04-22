#include < stdio.h>
#include < iostream>

#include < opencv2\opencv.hpp>
#include < opencv2/core/core.hpp>
#include < opencv2/highgui/highgui.hpp>
#include < opencv2/video/background_segm.hpp>


#ifdef _DEBUG        
#pragma comment(lib, "opencv_core248d.lib")
#pragma comment(lib, "opencv_imgproc248d.lib")   //MAT processing
#pragma comment(lib, "opencv_objdetect248d.lib") //HOGDescriptor
//#pragma comment(lib, "opencv_gpu248d.lib")
//#pragma comment(lib, "opencv_features2d248d.lib")
#pragma comment(lib, "opencv_highgui248d.lib")
#pragma comment(lib, "opencv_ml248d.lib")
//#pragma comment(lib, "opencv_stitching248d.lib");
//#pragma comment(lib, "opencv_nonfree248d.lib");
#pragma comment(lib, "opencv_video248d.lib")
#else
#pragma comment(lib, "opencv_core248.lib")
#pragma comment(lib, "opencv_imgproc248.lib")
#pragma comment(lib, "opencv_objdetect248.lib")
//#pragma comment(lib, "opencv_gpu248.lib")
//#pragma comment(lib, "opencv_features2d248.lib")
#pragma comment(lib, "opencv_highgui248.lib")
#pragma comment(lib, "opencv_ml248.lib")
//#pragma comment(lib, "opencv_stitching248.lib");
//#pragma comment(lib, "opencv_nonfree248.lib");
#pragma comment(lib, "opencv_video248d.lib")
#endif 

using namespace cv;
using namespace std;


void drawOptFlowMap (const Mat& flow, Mat& cflowmap, int step, const Scalar& color) {
 for(int y = 0; y < cflowmap.rows; y += step)
        for(int x = 0; x < cflowmap.cols; x += step)
        {
            const Point2f& fxy = flow.at< Point2f>(y, x);
            line(cflowmap, Point(x,y), Point(cvRound(x+fxy.x), cvRound(y+fxy.y)),
                 color);
            circle(cflowmap, Point(cvRound(x+fxy.x), cvRound(y+fxy.y)), 1, color, -1);
        }
    }


int main()
{
 int s=1;
 //global variables
 Mat GetImg;
 Mat prvs, next; //current frame
 
 char fileName[100] = "D:\\VTS.avi"; //video\\mm2.avi"; //mm2.avi"; //cctv 2.mov"; //mm2.avi"; //";//_p1.avi";
 VideoCapture stream1(fileName);   //0 is the id of video device.0 if you have only one camera   

 if(!(stream1.read(GetImg))) //get one frame form video
  return 0;
 resize(GetImg, prvs, Size(GetImg.size().width/s, GetImg.size().height/s) );
 cvtColor(prvs, prvs, CV_BGR2GRAY);

 //unconditional loop   
 while (true) {   
  
  if(!(stream1.read(GetImg))) //get one frame form video   
   break;
  //Resize
  resize(GetImg, next, Size(GetImg.size().width/s, GetImg.size().height/s) );
  cvtColor(next, next, CV_BGR2GRAY);
  ///////////////////////////////////////////////////////////////////
  Mat flow;
  calcOpticalFlowFarneback(prvs, next, flow, 0.5, 3, 15, 3, 5, 1.2, 0);

  Mat cflow;
  cvtColor(prvs, cflow, CV_GRAY2BGR);
  drawOptFlowMap(flow, cflow, 10, CV_RGB(0, 255, 0));
  imshow("OpticalFlowFarneback", cflow);

  ///////////////////////////////////////////////////////////////////
  //Display
  imshow("prvs", prvs);
  imshow("next", next);

  if (waitKey(5) >= 0)   
   break;

  prvs = next.clone();
 }

}