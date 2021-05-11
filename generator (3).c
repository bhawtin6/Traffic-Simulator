#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include "trafficLight.h"
#include "vehicle.h"
#include "simulator.h"


void main() {
	//start an infinite loop
	while (1){
		//generate random direction from 0-3
		int r = rand() % 4;
		char s[20];
		//generate command line command
		sprintf(s, "./vehicle %d 50 &", r);
		//submit command, sleep for half a second
		system(s);
		usleep(500000);
	}
}
