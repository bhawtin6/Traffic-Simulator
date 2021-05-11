#include <unistd.h>

// Set the state of the given traffic light to the newState
void setState(TrafficLight *t, char newState) {
	//change light state
	t->currentState = newState;
	
	//set timer according to state
	if (t->currentState == RED) t->countDownTimer = RED_COUNTDOWN;
	else if (t->currentState == YELLOW) t->countDownTimer = YELLOW_COUNTDOWN;
	else if (t->currentState == GREEN) t->countDownTimer = GREEN_COUNTDOWN;
	else t->countDownTimer = DELAY_RED_COUNTDOWN;
}


// This code should run in an infinite loop continuously simulating a timed traffic light.
void *runTrafficLight(void *t) {

  	TrafficLight  *light = t;
  			
	while(1) {
		//start timer
		int timer = light->countDownTimer;
		while (timer > 0){
			usleep(500000);
			timer --;
		}
		//timer is up, change state of lights
		if (light->currentState == RED) setState(light, DELAY_RED_2);
		else setState(light, light->currentState - 1);
 	}
}

