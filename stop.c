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
 
int main() {
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

	// Go into loop to commuincate with server now
	while (1) {
		//send shutdown command to server
		sprintf(buffer, "%d", SHUTDOWN);
		send(clientSocket, buffer, strlen(buffer), 0);

		// Get response from server, should be "OK"
		bytesRcv = recv(clientSocket, buffer, 80, 0);
		buffer[bytesRcv] = 0; // put a 0 at the end so we can display the string
		
		break; //exit inf loop
	} 

	close(clientSocket);  // Don't forget to close the socket !
}

 


