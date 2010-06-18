#include <X11/Xlib.h>
#include <iostream>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <float.h>
#include <limits.h>
#include <time.h>
#include <string.h>
#include <ctype.h>


/**
 * Start of X11 programming 
 */
 
 
 void move_mouse_pointer(int,int);
 
 int GetGlobalMousePosition();
 
 void mouseClick(int button);
 
 
 void move_mouse_pointer(int delta_x, int delta_y)
{
    // Opening and closing the display each time is not the right way to do this.
    Display *display = XOpenDisplay(0);
    XWarpPointer(display, None, None, 0, 0, 0, 0, delta_x, delta_y);
    // XWarpPointer (dpy, scr->Root, scr->Root, 0, 0, scr->MyDisplayWidth, scr->MyDisplayHeight, x, y);

    XCloseDisplay(display);
}


int GetGlobalMousePosition()
{
	Display *dsp = XOpenDisplay( NULL );
	if( !dsp ){ return 1; }
	
	int screenNumber = DefaultScreen(dsp);
	
	XEvent event;
	
	/* get info about current pointer position */
	XQueryPointer(dsp, RootWindow(dsp, DefaultScreen(dsp)),
	&event.xbutton.root, &event.xbutton.window,
	&event.xbutton.x_root, &event.xbutton.y_root,
	&event.xbutton.x, &event.xbutton.y,
	&event.xbutton.state);
	
	//printf("Mouse Coordinates: %d %d\n", event.xbutton.x, event.xbutton.y);
	move_mouse_pointer((-event.xbutton.x),(-event.xbutton.y));
	XCloseDisplay( dsp );
}


void mouseClick(int button)
{
	Display *display = XOpenDisplay(NULL);
	XEvent event;
	if(display == NULL)
	{
		fprintf(stderr, "XDisplay Not found\n");
		exit(EXIT_FAILURE);
	}
	memset(&event, 0x00, sizeof(event));
	event.type = ButtonPress;
	event.xbutton.button = button;
	event.xbutton.same_screen = True;
	
	XQueryPointer(display, RootWindow(display, DefaultScreen(display)), &event.xbutton.root, &event.xbutton.window, &event.xbutton.x_root, &event.xbutton.y_root, 		 &event.xbutton.x, &event.xbutton.y, &event.xbutton.state);
	
	event.xbutton.subwindow = event.xbutton.window;
	
	while(event.xbutton.subwindow)
	{
		event.xbutton.window = event.xbutton.subwindow;
		XQueryPointer(display, event.xbutton.window, &event.xbutton.root, &event.xbutton.subwindow, &event.xbutton.x_root, &event.xbutton.y_root, &event.xbutton.x, 		&event.xbutton.y, &event.xbutton.state);
	}
	
	if(XSendEvent(display, PointerWindow, True, 0xfff, &event) == 0) fprintf(stderr, "Error\n");
	XFlush(display);
	usleep(100000);
	
	event.type = ButtonRelease;
	event.xbutton.state = 0x100;
	
	if(XSendEvent(display, PointerWindow, True, 0xfff, &event) == 0) fprintf(stderr, "Error\n");
	
	XFlush(display);
	XCloseDisplay(display);
}
/**
 * End of X11 programming
 */

