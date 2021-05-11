#include <unistd.h>
#include <X11/Xlib.h>

//Display-related variables
Display *display;
Window   win;
GC       gc;


// Initialize and open the simulator window with size INTERSCETION_WIDTH x INTERSECTION_HEIGHT.
void initializeWindow() {
  	// Open connection to X server
  	display = XOpenDisplay(NULL);

  	// Create a simple window, set the title and get the graphics context then
  	// make is visible and get ready to draw
  	win = XCreateSimpleWindow(display,  RootWindow(display, 0), 0, 0,
			    			  INTERSECTION_WIDTH, INTERSECTION_HEIGHT, 0, 0x000000, 0x50B050);
  	XStoreName(display, win, "Traffic Simulator");
  	gc = XCreateGC(display, win, 0, NULL);
  	XMapWindow(display, win);
  	XFlush(display);
  	usleep(20000);  // sleep for 20 milliseconds.
}


// Close the display window
void closeWindow() {
  	XFreeGC(display, gc);
  	XUnmapWindow(display, win);
  	XDestroyWindow(display, win);
  	XCloseDisplay(display);
}


// Draw a traffic light
void drawTrafficLight(TrafficLight *t) {
	int verticalOffset = 0; int horizontalOffset = 0;

	// offsets will differ depending on the orientation
    if (t->verticalDirection == 0)
    	horizontalOffset = TRAFFIC_LIGHT_SIZE;
    else
    	verticalOffset = TRAFFIC_LIGHT_SIZE;
	
	// Draw the frame
    XSetForeground(display, gc, 0xFFB700);
    XFillRectangle(display, win, gc, t->xOffset, t->yOffset, 
    			   TRAFFIC_LIGHT_SIZE + horizontalOffset*2+4, TRAFFIC_LIGHT_SIZE + verticalOffset*2+4);
    XSetForeground(display, gc, 0x000000);
    XDrawRectangle(display, win, gc, t->xOffset, t->yOffset, 
    			   TRAFFIC_LIGHT_SIZE + horizontalOffset*2+4, TRAFFIC_LIGHT_SIZE + verticalOffset*2+4);
    							 
   	// Draw the off lights first
    XSetForeground(display, gc, 0x444444);
   	XFillArc(display, win, gc, t->xOffset + 2, t->yOffset + 2, TRAFFIC_LIGHT_SIZE, 
   			 TRAFFIC_LIGHT_SIZE, 0, 360*64);
	XFillArc(display, win, gc, t->xOffset + 2 + horizontalOffset, t->yOffset + 2 + verticalOffset, 
			 TRAFFIC_LIGHT_SIZE, TRAFFIC_LIGHT_SIZE, 0, 360*64);
	XFillArc(display, win, gc, t->xOffset + 2 + horizontalOffset*2, t->yOffset + 2 + verticalOffset*2, 
			 TRAFFIC_LIGHT_SIZE, TRAFFIC_LIGHT_SIZE, 0, 360*64);
			 
   	// Choose the right light color
   	char lightColor = t->currentState;
   	switch(lightColor) {
   		case RED:
   		case DELAY_RED_1:
   		case DELAY_RED_2:
   			XSetForeground(display, gc, 0xFF0000);break;
   		case YELLOW:
   			XSetForeground(display, gc, 0xFFFF00);break;
   		case GREEN:
   			XSetForeground(display, gc, 0x00FF00);
	}
	
   	// Swap red and greed colors if traffic light is upside down ... for location reasons only
   	if ((t->verticalDirection == 1) || (t->horizontalDirection == 1))
   		if (lightColor == GREEN) 
   			lightColor = RED;
   		else if (lightColor != YELLOW) 
   			lightColor = GREEN;
   		
   	// Draw the traffic light bulb that is currently on
   	switch(lightColor) {
   		case RED:
  		case DELAY_RED_1:
   		case DELAY_RED_2:
    		XFillArc(display, win, gc, t->xOffset + 2, t->yOffset + 2, 
    				 TRAFFIC_LIGHT_SIZE, TRAFFIC_LIGHT_SIZE, 0, 360*64);
    		XSetForeground(display, gc, 0x000000);
    		XDrawArc(display, win, gc, t->xOffset + 2, t->yOffset + 2, 
    				 TRAFFIC_LIGHT_SIZE, TRAFFIC_LIGHT_SIZE, 0, 360*64);break;
   		case YELLOW:
    		XFillArc(display, win, gc, t->xOffset + 2 + horizontalOffset, t->yOffset + 2 + verticalOffset, 
    				 TRAFFIC_LIGHT_SIZE, TRAFFIC_LIGHT_SIZE, 0, 360*64);
    		XSetForeground(display, gc, 0x000000);
    		XDrawArc(display, win, gc, t->xOffset + 2 + horizontalOffset, t->yOffset + 2 + verticalOffset, 
    				 TRAFFIC_LIGHT_SIZE, TRAFFIC_LIGHT_SIZE, 0, 360*64);break;
   		case GREEN:
    		XFillArc(display, win, gc, t->xOffset + 2 + horizontalOffset*2, 
    				 t->yOffset + 2 + verticalOffset*2, TRAFFIC_LIGHT_SIZE, TRAFFIC_LIGHT_SIZE, 0, 360*64);
    		XSetForeground(display, gc, 0x000000);
    		XDrawArc(display, win, gc, t->xOffset + 2 + horizontalOffset*2, 
    				 t->yOffset + 2 + verticalOffset*2, TRAFFIC_LIGHT_SIZE, TRAFFIC_LIGHT_SIZE, 0, 360*64);
	}
}


// Repeatedly redraw the intersection, showing the roads, traffic lights and all the vehicles.
// Vehicles are drawn as squares with width VEHICLE_WIDTH.
void *showSimulation(void *c) {
  	Intersection  *intersection = c;
  
  	// Open the window
  	initializeWindow();

  	// Now keep redrawing until someone kills the thread
  	while(1) {
  		// Draw the intersection's road surfaces and lines  
    	XSetForeground(display, gc, 0x666666);
    	XFillRectangle(display, win, gc, INTERSECTION_WIDTH/2 - ROAD_WIDTH, 0, 
    				   ROAD_WIDTH*2, INTERSECTION_HEIGHT);
    	XFillRectangle(display, win, gc, 0, INTERSECTION_HEIGHT/2 - ROAD_WIDTH, 
    				   INTERSECTION_WIDTH, ROAD_WIDTH*2);
    	XSetForeground(display, gc, 0xFFFFFF);
    	XDrawLine(display, win, gc, INTERSECTION_WIDTH/2-1, 0, 
    			  INTERSECTION_WIDTH/2-1, INTERSECTION_HEIGHT/2-ROAD_WIDTH);
    	XDrawLine(display, win, gc, INTERSECTION_WIDTH/2+1, 0, 
    			  INTERSECTION_WIDTH/2+1, INTERSECTION_HEIGHT/2-ROAD_WIDTH);
    	XDrawLine(display, win, gc, INTERSECTION_WIDTH/2-1, INTERSECTION_HEIGHT/2+ROAD_WIDTH, 
    			  INTERSECTION_WIDTH/2-1, INTERSECTION_HEIGHT);
    	XDrawLine(display, win, gc, INTERSECTION_WIDTH/2+1, INTERSECTION_HEIGHT/2+ROAD_WIDTH, 
    			  INTERSECTION_WIDTH/2+1, INTERSECTION_HEIGHT);
    	XDrawLine(display, win, gc, 0, INTERSECTION_HEIGHT/2-1, 
    			  INTERSECTION_WIDTH/2-ROAD_WIDTH, INTERSECTION_HEIGHT/2-1);
    	XDrawLine(display, win, gc, 0, INTERSECTION_HEIGHT/2+1, 
    			  INTERSECTION_WIDTH/2-ROAD_WIDTH, INTERSECTION_HEIGHT/2+1);
    	XDrawLine(display, win, gc, INTERSECTION_WIDTH/2+ROAD_WIDTH, INTERSECTION_HEIGHT/2-1, 
    			  INTERSECTION_WIDTH, INTERSECTION_HEIGHT/2-1);
    	XDrawLine(display, win, gc, INTERSECTION_WIDTH/2+ROAD_WIDTH, INTERSECTION_HEIGHT/2+1, 
    			  INTERSECTION_WIDTH, INTERSECTION_HEIGHT/2+1);
    	
    	XDrawLine(display, win, gc, INTERSECTION_WIDTH/2-ROAD_WIDTH, INTERSECTION_HEIGHT/2-ROAD_WIDTH, 
    			  INTERSECTION_WIDTH/2, INTERSECTION_HEIGHT/2-ROAD_WIDTH);
    	XDrawLine(display, win, gc, INTERSECTION_WIDTH/2, INTERSECTION_HEIGHT/2+ROAD_WIDTH, 
    			  INTERSECTION_WIDTH/2+ROAD_WIDTH, INTERSECTION_HEIGHT/2+ROAD_WIDTH);
    	XDrawLine(display, win, gc, INTERSECTION_WIDTH/2-ROAD_WIDTH, INTERSECTION_HEIGHT/2+ROAD_WIDTH, 
    			  INTERSECTION_WIDTH/2-ROAD_WIDTH, INTERSECTION_HEIGHT/2);
    	XDrawLine(display, win, gc, INTERSECTION_WIDTH/2+ROAD_WIDTH, INTERSECTION_HEIGHT/2-ROAD_WIDTH, 
    			  INTERSECTION_WIDTH/2+ROAD_WIDTH, INTERSECTION_HEIGHT/2);
   
		// Draw the Traffic Lights
		for (int i=0; i<4; i++) 
			drawTrafficLight(&(intersection->lights[i]));
	
    	// Draw all the vehicles in each direction
    	for (int d=0; d<4; d++) {
    		for (int i=0; i<intersection->monitor.traffic[d].lineCount; i++) {
    			if (intersection->monitor.traffic[d].lineup[i].id != -1) {
					XSetForeground(display, gc, 0x8888FF); // draw a light blue car
	  				XFillRectangle(display, win, gc, 
	  					 	intersection->monitor.traffic[d].lineup[i].x-VEHICLE_WIDTH, 
	  					 	intersection->monitor.traffic[d].lineup[i].y-VEHICLE_WIDTH, 
	  					 	2*VEHICLE_WIDTH, 2*VEHICLE_WIDTH);
					XSetForeground(display, gc, 0x000000); // draw a black border
	  				XDrawRectangle(display, win, gc, 
	  					 	intersection->monitor.traffic[d].lineup[i].x-VEHICLE_WIDTH, 
	  					 	intersection->monitor.traffic[d].lineup[i].y-VEHICLE_WIDTH, 
	  					 	2*VEHICLE_WIDTH, 2*VEHICLE_WIDTH);
				}
			}
		}
		
		// Draw the lineup counts
		XSetForeground(display, gc, 0xFFFFFF);
		char s[3]; 
		sprintf(s, "%d", intersection->monitor.traffic[NORTHBOUND].lineCount);
		XDrawString(display, win, gc, INTERSECTION_WIDTH/2+ROAD_WIDTH/2, INTERSECTION_HEIGHT-20, s, 1);
		sprintf(s, "%d", intersection->monitor.traffic[SOUTHBOUND].lineCount);
		XDrawString(display, win, gc, INTERSECTION_WIDTH/2-ROAD_WIDTH/2, 20, s, 1);
		sprintf(s, "%d", intersection->monitor.traffic[EASTBOUND].lineCount);
		XDrawString(display, win, gc, 20, INTERSECTION_HEIGHT/2+ROAD_WIDTH/2, s, 1);
		sprintf(s, "%d", intersection->monitor.traffic[WESTBOUND].lineCount);
		XDrawString(display, win, gc, INTERSECTION_WIDTH-20, INTERSECTION_HEIGHT/2-ROAD_WIDTH/2, s, 1);
    	
    	XFlush(display);
    	usleep(20000);
  	}
  
  	closeWindow();
  	pthread_exit(NULL);
}

