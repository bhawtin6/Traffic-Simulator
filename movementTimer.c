#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

void *movement(void *inter){
	Intersection *intersec = (Intersection *)inter;
	
	while (1){
		//0 northbound vehicle movement
		for (int i = 0 ; i < intersec->monitor.traffic[0].lineCount; i++){
			//printf("dir = north, i=%d, speed = %d\n", i, intersec->monitor.traffic[0].lineup[i].speed);
			intersec->monitor.traffic[0].lineup[i].y-=intersec->monitor.traffic[0].lineup[i].speed;
		} 
		
		//1 southbound vehicle movement
		for (int i = 0 ; i < intersec->monitor.traffic[1].lineCount; i++){
			intersec->monitor.traffic[1].lineup[i].y+=intersec->monitor.traffic[1].lineup[i].speed;
		} 
		
		//2 eastbound vehicle movement
		for (int i = 0 ; i < intersec->monitor.traffic[2].lineCount; i++){
			intersec->monitor.traffic[2].lineup[i].x+=intersec->monitor.traffic[2].lineup[i].speed;
			//printf(".x = %d", intersec->monitor.traffic[2].lineup[i].x);
		} 
		
		//3 westbound vehicle movement
		for (int i = 0 ; i < intersec->monitor.traffic[3].lineCount; i++){
			intersec->monitor.traffic[3].lineup[i].x-=intersec->monitor.traffic[3].lineup[i].speed;
		} 
		usleep(20000);
	}
		
}
