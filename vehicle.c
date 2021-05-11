#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "trafficLight.h"
#include "vehicle.h"
#include "simulator.h"

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 6000


// A vehicle starts be specifying (as command-line arguments) its incoming direction 
// (i.e., NORTHBOUND=0, SOUTHBOUND=1, EASTBOUND =2, WESTBOUND=3) as well as its speed 
// (i.e., 1 to 100% of its top speed).
int main(int argc, char * argv[]) {

	// WRITE CODE HERE TOO
	char *direction = argv[1];
	int speed = atoi(argv[2]);
	unsigned char id;
	
	int                 clientSocket;
	struct sockaddr_in  clientAddress;
	int                 status, bytesRcv;
	char                buffer[2];   // stores sent and received data

	// Create socket
	clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (clientSocket < 0) {
		printf("*** CLIENT ERROR: Could open socket.\n");
		exit(-1);
	}

	// Setup address
	memset(&clientAddress, 0, sizeof(clientAddress));
	clientAddress.sin_family = AF_INET;
	clientAddress.sin_addr.s_addr = inet_addr(SERVER_IP);
	clientAddress.sin_port = htons((unsigned short) SERVER_PORT);

	// Connect to server
	status = connect(clientSocket, (struct sockaddr *) &clientAddress, sizeof(clientAddress));
	if (status < 0) {
		printf("*** CLIENT ERROR: Could not connect.\n");
		exit(-1);
	}

  	//send connect signal
	sprintf(buffer, "%d", CONNECT);
	send(clientSocket, buffer, strlen(buffer), 0);
	//get server response
	bytesRcv = recv(clientSocket, buffer, 80, 0);
	buffer[bytesRcv] = 0; 
	
	//send direction
	strcpy(buffer,direction);
	send(clientSocket, buffer, strlen(buffer), 0);
	//get server response
	bytesRcv = recv(clientSocket, buffer, 80, 0);
	buffer[bytesRcv] = 0;
	
	//send speed
	speed = VEHICLE_TOP_SPEED*speed/100;
	sprintf(buffer, "%d", speed);
	send(clientSocket, buffer, strlen(buffer), 0);
	
	//get server response with vehicle ID
	bytesRcv = recv(clientSocket, buffer, 80, 0);
	buffer[bytesRcv] = 0; 
	
	//set vehicle id 
	id = atoi(buffer);
  	
  	// Go into an infinite loop to keep sending/getting updates from traffic monitor
  	
  	while(1) {
 
	  	// Create socket
		clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (clientSocket < 0) {
		  printf("*** CLIENT ERROR: Could open socket.\n");
		  exit(-1);
		}

		// Setup address
		memset(&clientAddress, 0, sizeof(clientAddress));
		clientAddress.sin_family = AF_INET;
		clientAddress.sin_addr.s_addr = inet_addr(SERVER_IP);
		clientAddress.sin_port = htons((unsigned short) SERVER_PORT);

		// Connect to server
		status = connect(clientSocket, (struct sockaddr *) &clientAddress, sizeof(clientAddress));
		if (status < 0) {
		  printf("*** CLIENT ERROR: Could not connect.\n");
		  exit(-1);
		}
  	
		//send update signal
		sprintf(buffer, "%d", UPDATE);
		send(clientSocket, buffer, strlen(buffer), 0);
		//get server response
		bytesRcv = recv(clientSocket, buffer, 80, 0);
		buffer[bytesRcv] = 0; 
		
		//send speed
		sprintf(buffer, "%d", speed);
		send(clientSocket, buffer, strlen(buffer), 0);
		//get server response
		bytesRcv = recv(clientSocket, buffer, 80, 0);
		buffer[bytesRcv] = 0;
		
		//send direction
		strcpy(buffer,direction);
		send(clientSocket, buffer, strlen(buffer), 0);
		//get server response
		bytesRcv = recv(clientSocket, buffer, 80, 0);
		buffer[bytesRcv] = 0;
		
		//send id
		sprintf(buffer, "%d", id);
		send(clientSocket, buffer, strlen(buffer), 0);
		//recieve no or light colour
		bytesRcv = recv(clientSocket, buffer, 80, 0);
		buffer[bytesRcv] = 0; 
		
		//if the response is no, skip the speed updating algorithm
		if (strcmp(buffer, "NO") == 0){
			break;
		}
		
		//the response indicates that the car is still valid, update its speed
		else {
			//set lightColour as recieved from the server
			char lightColour = atoi(buffer);
			//send response to server
			send(clientSocket, buffer, strlen(buffer), 0);
			
			//recieve distance
			int distance;
			bytesRcv = recv(clientSocket, buffer, 80, 0);
			buffer[bytesRcv] = 0; 
			distance = atoi(buffer);
			//send response to server
			send(clientSocket, buffer, strlen(buffer), 0);
			
			//get info on car ahead
			int speedAhead;
			int distAhead;
			//recieve speedAhead
			bytesRcv = recv(clientSocket, buffer, 80, 0);
			buffer[bytesRcv] = 0;
			//send response to server
			send(clientSocket, buffer, strlen(buffer), 0);
			speedAhead = atoi(buffer);
			
			//recieve distAhead
			bytesRcv = recv(clientSocket, buffer, 80, 0);
			buffer[bytesRcv] = 0;
			distAhead = atoi(buffer);
			
			//first car in line
			if (distAhead == MAX_DISTANCE){
				//slow down for red or yellow light
				if (lightColour != GREEN && distance != MAX_DISTANCE){
					if (distance <= (2*VEHICLE_WIDTH + speed)) speed = 0;
					else if (distance <= (3*VEHICLE_WIDTH*speed)) {
						speed = speed/1.5;
						if (speed < 1) speed = 1;
					}
				}
				//light is green or we are past the light, accelerate
				else {
					if (speed < VEHICLE_TOP_SPEED) speed++;
				}
			}
			//there are cars ahead
			else {
				//slow down for light
				if (lightColour != GREEN && distance != MAX_DISTANCE){
					if(distance <=(2*VEHICLE_WIDTH + speed)) speed = 0;
					else if (distance <= (3*VEHICLE_WIDTH*speed)) {
						speed = speed/1.5;
						if (speed < 1) speed = 1;
					}
				}
				//slow down for vehicle ahead
				if (speedAhead < speed){
					if (distAhead <= (4*VEHICLE_WIDTH+speed)){
						speed = 0;
					}
					else if (distAhead <= (3*VEHICLE_WIDTH*speed)){
						speed = speed/1.5;
						if (speed < 1) speed = 1;
					}
				}
				//accelerate the vehicel if conditions allow
				else if ((speedAhead > speed) && ((distance == MAX_DISTANCE) || (lightColour == GREEN))){
					if (speed <speedAhead) speed++;
				}
			}
		}
    		usleep(50000);  // A delay to slow things down a little
  	}
  	exit(0); //exit, needed to prevent stack smashing.
}

