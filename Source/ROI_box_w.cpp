#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include <cv.h>
#include <highgui.h>

using namespace cv;
using namespace std;

void my_mouse_callback( int event, int x, int y, int flags, void* param );

bool destroy=false;
CvRect box;
bool drawing_box = false;

void draw_box(IplImage* img, CvRect rect)
{
  cvRectangle(img, cvPoint(box.x, box.y), cvPoint(box.x+box.width,box.y+box.height),
              cvScalar(0,0,255) ,2);

  CvRect rect2=cvRect(box.x,box.y,box.width,box.height);
  //cvSetImageROI(image, rect2);   //here I wanted to set the drawn rect as ROI
}

// Implement mouse callback
void my_mouse_callback( int event, int x, int y, int flags, void* param )
{
  IplImage* frame = (IplImage*) param;

  switch( event )
  {
      case CV_EVENT_MOUSEMOVE: 
      {
          if( drawing_box )
          {
              box.width = x-box.x;
              box.height = y-box.y;
          }
      }
      break;

      case CV_EVENT_LBUTTONDOWN:
      {
          drawing_box = true;
          box = cvRect( x, y, 0, 0 );
      }
      break;

      case CV_EVENT_LBUTTONUP:
      {
          drawing_box = false;
          if( box.width < 0 )
          {
              box.x += box.width;
              box.width *= -1;
          }

          if( box.height < 0 )
          {
              box.y += box.height;
              box.height *= -1;
          }

          draw_box(frame, box);
      }
      break;

      case CV_EVENT_RBUTTONUP:
      {
          destroy=true;
      }
      break;

      default:
      break;
   }
}

int main(int argc, char *argv[])
{
  const char* name = "Box Example";
  cvNamedWindow( name );
  box = cvRect(0,0,1,1);

  CvCapture* capture = cvCreateFileCapture(argv[1]); 
  if (!capture)
  {
    printf("!!! Failed cvCaptureFromFile\n");
    return 1;
  }

  IplImage* image = cvQueryFrame(capture);
  if (!image)
  {
    printf("!!! Failed cvQueryFrame #1\n");
    return 2;
  }

  IplImage* temp = cvCloneImage(image);

  // Set up the callback
  cvSetMouseCallback(name, my_mouse_callback, (void*) image);

  // Main loop
  while( 1 )
  {
    if (destroy) 
    {
      cvDestroyWindow(name); break;
    }
    cvCopyImage(image, temp);

    if (drawing_box) 
        draw_box(temp, box);

    cvMoveWindow(name, 200, 100);
    cvShowImage(name, temp);

    if (cvWaitKey(15) == 27) 
        break;
  }

  cvReleaseImage(&temp);
  cvDestroyWindow(name);

  cvNamedWindow("Example2", CV_WINDOW_AUTOSIZE);
  cvMoveWindow("Example2", 150, 150);

  // Retrieve a single frame from the device  and set the ROI
  IplImage* vid_frame = cvQueryFrame(capture);
  if (!vid_frame)
  {
    printf("!!! Failed cvQueryFrame #2\n");
    return 2;
  }

  cvSetImageROI(vid_frame, box);

  // Allocate space for a single-channel ROI (to store grayscale frames)
  IplImage* gray_roi = cvCreateImage(cvSize(box.width, box.height), IPL_DEPTH_8U, 1);
  IplImage* rgb_roi = cvCreateImage(cvSize(box.width, box.height), IPL_DEPTH_8U, 3);

  while(1) 
  {
    if (!vid_frame)
    {
        vid_frame = cvQueryFrame(capture);
        if (!vid_frame)
        {
            printf("!!! Failed cvQueryFrame #3\n");
            break;
        }
    }

    draw_box(vid_frame,  box);

    // Set ROI and perform some processing (in this case, converting the ROI to grayscale)
    cvSetImageROI(vid_frame, box);
    cvCvtColor(vid_frame, gray_roi, CV_BGR2GRAY);
        //cvShowImage("Example2", gray_roi);

    /* At this point gray_roi has the size of thei ROI and contains the processed image.
     * For fun, we copy the processed image back to the original image and display it on the screen!
     */
    cvCvtColor(gray_roi, rgb_roi, CV_GRAY2BGR);

    // As the ROI is still set, cvCopy is affected by it
    cvCopy(rgb_roi, vid_frame, NULL);

    // Now reset the ROI so cvShowImage displays the full image
    cvResetImageROI(vid_frame);
    cvShowImage("Example2", vid_frame);

    char c = cvWaitKey(33);
    if( c == 27 ) break;

    vid_frame = NULL;
  }
  cvSaveImage("processed.jpg", vid_frame);

  cvReleaseImage(&gray_roi);
  cvReleaseImage(&rgb_roi);
  cvReleaseCapture( &capture );
  cvDestroyWindow( "Example2" );

  return 0;
}