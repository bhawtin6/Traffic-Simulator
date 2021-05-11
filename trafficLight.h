#define RED			 1
#define DELAY_RED_1		 2 // Transition state from yellow to red
#define YELLOW			 3
#define GREEN			 4
#define DELAY_RED_2	 	 5 // Transition state from red to green

#define RED_COUNTDOWN		13 // Time units for each state
#define DELAY_RED_COUNTDOWN 	 2
#define YELLOW_COUNTDOWN	 3
#define GREEN_COUNTDOWN		10

typedef struct {
  	char currentState;		// RED, YELLOW or GREEN or DELAY_RED_1 or DELAY_RED_2
  	char verticalDirection;		// 1 for down-moving traffic, -1 for up-moving, 0 for horizontal traffic
  	char horizontalDirection;	// 1 for right-moving traffic, -1 for left-moving, 0 for vertical traffic
  	int  xOffset;			// Top left corner X coordinate (in pixels)
  	int  yOffset;			// Top left corner Y coordinate (in pixels)
  	int  countDownTimer;		// counts down and when reaches zero it changes state
} TrafficLight;


extern void setState(TrafficLight *t, char newState);	// Call this to set the state of the traffic light

