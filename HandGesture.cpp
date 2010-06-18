#include <cv.h>
#include <highgui.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <float.h>
#include <limits.h>
#include <time.h>
#include <ctype.h>
#include "mouse.h"
#include "haar_detect.h"

using namespace std;

int use_nested_cascade = 0;
bool transition = false;
const char* cascade_name =
    "fixed semi opened palm.xml";
const char* cascade_name_a = 
    "Agest.xml";

int main( int argc, char** argv )
{
    GetGlobalMousePosition();  //get the global position of the pointer
    
    CvCapture* capture = 0;
    IplImage *frame, *frame_copy = 0;
    IplImage *image = 0;
    const char* scale_opt = "--scale=";
    int scale_opt_len = (int)strlen(scale_opt);
    const char* cascade_opt = "--cascade=";
    int cascade_opt_len = (int)strlen(cascade_opt);
    const char* nested_cascade_opt = "--nested-cascade";
    int nested_cascade_opt_len = (int)strlen(nested_cascade_opt);
    int i;
    const char* input_name = 0;
    cascade = (CvHaarClassifierCascade*)cvLoad( cascade_name, 0, 0, 0 );
    cascade_a = (CvHaarClassifierCascade*)cvLoad( cascade_name_a, 0, 0, 0 );

      storage = cvCreateMemStorage(0);
    
    if( !input_name || (isdigit(input_name[0]) && input_name[1] == '\0') )		//if no arguments are specified capture from camera '0'
        capture = cvCaptureFromCAM( !input_name ? 0 : input_name[0] - '0' );
	cvNamedWindow( "result", 1 );

    if( capture )		//NOTE if frames are captured from CAM
    {
        for(;;)
        {
            if( !cvGrabFrame( capture ))
                break;
            frame = cvRetrieveFrame( capture );
            cvFlip(frame, 0 , 1);
            if( !frame )
                break;
            if( !frame_copy )
                frame_copy = cvCreateImage( cvSize(frame->width,frame->height),
                                            IPL_DEPTH_8U, frame->nChannels );
            if( frame->origin == IPL_ORIGIN_TL )
                cvCopy( frame, frame_copy, 0 );
            else
                cvFlip( frame, frame_copy, 0 );
            
            detect_and_draw( frame_copy );		//call the function implementing viola jones

            if( cvWaitKey( 10 ) >= 0 )
                goto _cleanup_;				//Decoy
        }

        cvWaitKey(10);
_cleanup_:
        cvReleaseImage( &frame_copy );
        cvReleaseCapture( &capture );
    }
     cvDestroyWindow("result");
    cvDestroyWindow("Sub");

    return 0;
}


