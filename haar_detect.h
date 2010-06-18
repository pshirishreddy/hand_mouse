#include <cv.h>
#include <highgui.h>
#include <iostream>


using namespace std;


static CvMemStorage* storage = 0;
static CvHaarClassifierCascade* cascade = 0;
static CvHaarClassifierCascade* cascade_a = 0;
static CvHaarClassifierCascade* nested_cascade = 0;
double scale = 1;


void detect_and_draw( IplImage* img )
{
    cvNamedWindow("Sub", 1);
    static int left_double_click = 0;
    static int left_click = 0;
    static int delta_x = 0, delta_y = 0; //difference is position of the object
    static int prev_x = 0, prev_y = 0;    //record the previous position of the object
    static int sum_delta_x = 0, sum_delta_y = 0;
    static int prev_delta_x = 0, prev_delta_y = 0;
    static int final_center_x = 0, final_center_y =0;
    
    
   // cvNamedWindow("rot", 1);
    static CvScalar colors[] = 
    {
        {{0,0,255}},
        {{0,128,255}},
        {{0,255,255}},
        {{0,255,0}},
        {{255,128,0}},
        {{255,255,0}},
        {{255,0,0}},
        {{255,0,255}}
    };

    IplImage *gray, *small_img, *imgoi;
    int i, j;

    gray = cvCreateImage( cvSize(img->width,img->height), 8, 1 );
    small_img = cvCreateImage( cvSize( cvRound (img->width/scale),
                         cvRound (img->height/scale)), 8, 1 );
    imgoi = cvCreateImage( cvSize(img->width,img->height), img->depth, img->nChannels );
   // cout<<img->width<<img->height<<endl;
    cvCopy(img, imgoi, NULL);
    
    //cvZero(imgoi);

    cvCvtColor( img, gray, CV_BGR2GRAY );
    cvResize( gray, small_img, CV_INTER_LINEAR );
    cvEqualizeHist( small_img, small_img );
    cvClearMemStorage( storage );

    if( cascade )
    {
    	
        double t = (double)cvGetTickCount();
        /**/
        CvSeq* fist = cvHaarDetectObjects( small_img, cascade_a, storage,
                                            1.1, 6, 0
                                            //|CV_HAAR_FIND_BIGGEST_OBJECT
                                            //|CV_HAAR_DO_ROUGH_SEARCH
                                            |CV_HAAR_DO_CANNY_PRUNING
                                            //|CV_HAAR_SCALE_IMAGE
                                            ,
                                            cvSize(30, 30) );
        t = (double)cvGetTickCount() - t;
        printf( "detection time = %gms\n", t/((double)cvGetTickFrequency()*1000.) );
        
       
        //start for fist
        if (fist->total!=0) {
	        for( i = 0; i < fist->total; i++ )
	        {
	            cout<<"fist"<<"\n";
	            IplImage *ipimg = cvCreateImage( cvSize(img->width,img->height), img->depth, img->nChannels );
	            //ipimg = (IplImage*)cvGetSeqElem(fist, i);
	            
            	//cvShowImage("rot", ipimg);
	           // imgoi = (IplImage*)cvGetSeqElem(fist, i); 	
	            CvRect* r = (CvRect*)cvGetSeqElem( fist, i );
	            ipimg = (IplImage*)r;
	           // cout<<r->width<<"\n";
	            
	            CvMat small_img_roi;
	            CvSeq* nested_objects;
	            CvPoint center;
	            CvScalar color = colors[i%8];
	            int radius;
	            center.x = cvRound((r->x + r->width*0.5)*scale);
	            center.y = cvRound((r->y + r->height*0.5)*scale);
        	    radius = cvRound((r->width + r->height)*0.25*scale);
	            //cvCircle( img, center, radius, color, 3, 8, 0 );
	            
	            cvCircle( imgoi, center, radius, color, 3, 8, 0);
	            cvSetImageROI(imgoi,*r);
	            delta_x = center.x - prev_x;   //difference in X coordrinate of the object
	            delta_y = center.y - prev_y;   //difference in Y coordinate of the object; 
	            
	            // retain the pointer position to obtained position in case of transition from gestures
	            // record the total displacement from initial position.
	            sum_delta_x = delta_x + prev_delta_x;	
	            sum_delta_y = delta_y + prev_delta_y;
	           // cout<<"\n\n"<<delta_x<<"+"<<prev_delta_x<<"="<<sum_delta_x<<" "<<sum_delta_y<<endl;
	  	    
	            //cout<<"location of center of fist :"<<center.x<<"\t"<<center.y<<endl;
	            //cout<<"difference in position of the object :"<<delta_x<<"\t"<<delta_y<<endl;
		    //cvReleaseImage( &imgoi );
		    prev_x = center.x;
	            prev_y = center.y;
	            if(delta_x > 0 && 2*delta_x < 10) {
	            	delta_x = 0;
	            }
	            else if(delta_x  < 0 && 2*delta_x > -10) {
	            	delta_x = 0;
	            }
	            move_mouse_pointer(2*delta_x, 3*delta_y);
	            prev_delta_x = delta_x; prev_delta_y = delta_y;
		    final_center_x = center.x;
		    final_center_x = center.y;
	
	        }
        }
        else {
        	//transition = true; //all the other conditions other than the fist => transition from fist
        	
	        CvSeq* palm = cvHaarDetectObjects( small_img, cascade, storage,
	                                            1.1, 6, 0
	                                            //|CV_HAAR_FIND_BIGGEST_OBJECT
	                                            //|CV_HAAR_DO_ROUGH_SEARCH
	                                            |CV_HAAR_DO_CANNY_PRUNING
	                                            //|CV_HAAR_SCALE_IMAGE
                                            	,
	                                            cvSize(30, 30) );
        	//end for fist
        	
        	if(palm->total) {
	         for( i = 0; i < palm->total; i++ )
	        {
	            //cout<<"Open Palm"<<"\n";
	           // imgoi = (IplImage*)cvGetSeqElem(palm, i); 	
	            CvRect* r = (CvRect*)cvGetSeqElem( palm, i );
	            CvMat small_img_roi;
	            CvSeq* nested_objects;
	            CvPoint center;
	            CvScalar color = colors[i%8];
	            int radius;
	            center.x = cvRound((r->x + r->width*0.5)*scale);
	            center.y = cvRound((r->y + r->height*0.5)*scale);
	            radius = cvRound((r->width + r->height)*0.25*scale);
	            cvCircle( img, center, radius, color, 3, 8, 0 );
	            
	           // cvCircle( imgoi, center, radius, color, 3, 8, 0);
	            cvSetImageROI(imgoi,*r);
	            
	            //cout<<"location of center of palm :"<<center.x<<"\t"<<center.y<<endl;
	         //   if( !nested_cascade )
	          //      continue;
	            //cout<<"Open palm"<<"\n";
	            cvGetSubRect( small_img, &small_img_roi, *r );
	
	        }
	        left_double_click++;
	        left_click++;
	        if( left_double_click > 10) {
	        	left_double_click = 0;
	        	left_click = 0;
		        mouseClick(Button1);
		        mouseClick(Button1);
		}
		else if(left_click > 5) {
			left_click = 0;
			mouseClick(Button1);
		}
		
		
	    }
	        
        }
    }

    cvShowImage("Sub" , imgoi);
    cvShowImage( "result", img );
    cvReleaseImage( &gray );
    cvReleaseImage( &small_img );
    cvReleaseImage( &imgoi);
}

