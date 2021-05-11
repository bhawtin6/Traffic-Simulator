#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>


#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 6000



// Handle client requests coming in through the server socket.  This code should run
// indefinitiely.  It should wait for a client to send a request, process it, and then
// close the client connection and wait for another client.  The requests that may be
// handles are SHUTDOWN, CONNECT and UPDATE.  
void *handleIncomingRequests(void *inter) {

	int cmd;
	Intersection *intersec = (Intersection *)inter;	
	int                 serverSocket, clientSocket;
	struct sockaddr_in  serverAddress, clientAddr;
	int                 status, addrSize, bytesRcv;
	char                buffer[30];
	char*               response = "OK";
	char		    id[30];	

	// Create the server socket
	serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (serverSocket < 0) {
		printf("*** SERVER ERROR: Could not open socket.\n");
		exit(-1);
	}

	// Setup the server address
	memset(&serverAddress, 0, sizeof(serverAddress)); // zeros the struct
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddress.sin_port = htons((unsigned short) SERVER_PORT);

	int one = 1; setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
	// Bind the server socket
	status = bind(serverSocket,  (struct sockaddr *)&serverAddress, sizeof(serverAddress));
	if (status < 0) {
		printf("*** SERVER ERROR: Could not bind socket.\n");
		exit(-1);
	}

	// Set up the line-up to handle up to 5 clients in line 
	status = listen(serverSocket, 5);
	if (status < 0) {
		printf("*** SERVER ERROR: Could not listen on socket.\n");
		exit(-1);
	}

	// Wait for clients now
	while (1) {
	
		response = "OK"; //generic response message
		
		//accept client socket
		addrSize = sizeof(clientAddr);
		clientSocket = accept(serverSocket, (struct sockaddr *) &clientAddr, &addrSize);
		if (clientSocket < 0) {
			printf("*** SERVER ERROR: Could not accept incoming client connection.\n");
			exit(-1);
		}


		// Get the command from the client
		bytesRcv = recv(clientSocket, buffer, sizeof(buffer), 0);
		buffer[bytesRcv] = 0; // put a 0 at the end so we can display the string
		if (strcmp(buffer, "") == 0) break;
		cmd = atoi(buffer);

		// Respond with an "OK" message
		send(clientSocket, response, strlen(response), 0);
		
		//shutdown if commanded, breaks inf loop
		if (cmd == SHUTDOWN) break;
		
		//connect command path
		else if (cmd == CONNECT){
			
			int direction; //direction of car
			unsigned char speed; //speed of car
			
			//get direction
			bytesRcv = recv(clientSocket, buffer, sizeof(buffer), 0);
			buffer[bytesRcv] = 0; 
			direction = atoi(buffer);	

			// Respond with an "OK" message
			send(clientSocket, response, strlen(response), 0);

			//get speed
			bytesRcv = recv(clientSocket, buffer, sizeof(buffer), 0);
			buffer[bytesRcv] = 0; 
			speed = atoi(buffer);

			// Respond with id of vehicle, useful for debugging
			sprintf(id, "%d", intersec->monitor.idCounter[direction]);
			send(clientSocket, id, strlen(id), 0);
			
			//add a new vehicle if there is room in the given line
			if (intersec->monitor.traffic[direction].lineCount < MAX_CONNECTIONS){	
				intersec->monitor.traffic[direction].lineup[intersec->monitor.traffic[direction].lineCount].speed = speed;
				intersec->monitor.traffic[direction].lineup[intersec->monitor.traffic[direction].lineCount].x = intersec->monitor.traffic[direction].entryX;
				intersec->monitor.traffic[direction].lineup[intersec->monitor.traffic[direction].lineCount].y = intersec->monitor.traffic[direction].entryY;
				intersec->monitor.traffic[direction].lineup[intersec->monitor.traffic[direction].lineCount].id = intersec->monitor.idCounter[direction];
				
				//increase counters
				if (intersec->monitor.traffic[direction].lineCount == 255) intersec->monitor.traffic[direction].lineCount = 1;
				else intersec->monitor.traffic[direction].lineCount++;
				intersec->monitor.idCounter[direction]++;

			}
		}
		
		//update command path
		else if (cmd == UPDATE){
			//attributes of the vehicle being updated
			int direction;
			unsigned char speed;
			unsigned char id;


			//get speed
			bytesRcv = recv(clientSocket, buffer, sizeof(buffer), 0);
			buffer[bytesRcv] = 0; 
			speed = atoi(buffer);

			// Respond with an "OK" message
			send(clientSocket, response, strlen(response), 0);

			//get direction
			bytesRcv = recv(clientSocket, buffer, sizeof(buffer), 0);
			buffer[bytesRcv] = 0; 
			direction = atoi(buffer);

			// Respond with an "OK" message
			send(clientSocket, response, strlen(response), 0);

			//get id
			bytesRcv = recv(clientSocket, buffer, sizeof(buffer), 0);
			buffer[bytesRcv] = 0; 
			id = atoi(buffer);

			// process and respond
			int exists = 0;
			int i; //used for finding and indexing a requested car
			
			//try to find the requested vehicle by checking through each existing one's id
			for (i = 0; i < intersec->monitor.traffic[direction].lineCount ; i++){
				if (intersec->monitor.traffic[direction].lineup[i].id == id){
					exists = 1;
					intersec->monitor.traffic[direction].lineup[i].speed=speed;
					break;
				}
			}
			
			//if the vehicle is found, handle client communication here
			if (exists == 1){
				
				//handle for northbound vehicles
				if (direction == NORTHBOUND && intersec->monitor.traffic[direction].lineup[i].y>(0-VEHICLE_WIDTH) ) {
					
					//set response to yes
					response = "YES";
					
					//send state of the traffic light
					char res[30]; 
					sprintf(res, "%d", intersec->lights[direction].currentState);
					send(clientSocket, res, strlen(res), 0);
					
					//recieve response from client
					bytesRcv = recv(clientSocket, buffer, sizeof(buffer), 0);
					buffer[bytesRcv] = 0; 
					
					//calculate and send distance to the traffic light
					int distance;
					if (intersec->monitor.traffic[direction].lineup[i].y < intersec->monitor.traffic[direction].stopY) distance = MAX_DISTANCE;
					else distance = intersec->monitor.traffic[direction].lineup[i].y - intersec->monitor.traffic[direction].stopY;
					sprintf(res, "%d", distance);
					send(clientSocket, res, strlen(res), 0);
					
					//recieve response from client
					bytesRcv = recv(clientSocket, buffer, sizeof(buffer), 0);
					buffer[bytesRcv] = 0; 
					
					//calculate speedAhead and distAhead (describe the vehicle ahead of the one being updated)
					int speedAhead;
					int distAhead;
					
					//if the vehicle is the front vehicle
					if (i == 0) {
						speedAhead = VEHICLE_TOP_SPEED + 1;
						distAhead = MAX_DISTANCE;
					}
					//if the vehicle is not the front vehicle
					else {
						speedAhead = intersec->monitor.traffic[direction].lineup[i-1].speed;
						distAhead = intersec->monitor.traffic[direction].lineup[i].y - intersec->monitor.traffic[direction].lineup[i-1].y;
					}
					
					//send speedAhead
					sprintf(res, "%d", speedAhead);
					send(clientSocket, res, strlen(res), 0);
					//recieve response from client
					bytesRcv = recv(clientSocket, buffer, sizeof(buffer), 0);
					buffer[bytesRcv] = 0; 
						
					//send dist to next vehicle
					sprintf(res, "%d", distAhead);
					send(clientSocket, res, strlen(res), 0);
				}	
				
				//handle southbound vehicles	
				else if (direction == SOUTHBOUND && intersec->monitor.traffic[direction].lineup[i].y<(INTERSECTION_HEIGHT+VEHICLE_WIDTH) ) {
					
					//set response to yes
					response = "YES";
					
					//send state of the traffic light
					char res[2];
					sprintf(res, "%d", intersec->lights[direction].currentState);
					send(clientSocket, res, strlen(res), 0);
					
					//recieve response from client
					bytesRcv = recv(clientSocket, buffer, sizeof(buffer), 0);
					buffer[bytesRcv] = 0;
					
					//calculate and send distance to the traffic light
					int distance;
					if (intersec->monitor.traffic[direction].lineup[i].y > intersec->monitor.traffic[direction].stopY) distance = MAX_DISTANCE;
					else distance =  intersec->monitor.traffic[direction].stopY - intersec->monitor.traffic[direction].lineup[i].y;
					sprintf(res, "%d", distance);
					send(clientSocket, res, strlen(res), 0);
					
					//recieve response from client
					bytesRcv = recv(clientSocket, buffer, sizeof(buffer), 0);
					buffer[bytesRcv] = 0; 
					
					//calculate speedAhead and distAhead (describe the vehicle ahead of the one being updated)
					int speedAhead;
					int distAhead;
					
					//if the vehicle is the front vehicle
					if (i == 0) {
						speedAhead = VEHICLE_TOP_SPEED + 1;
						distAhead = MAX_DISTANCE;
					}
					//if the vehicle is not the front vehicle
					else {
						speedAhead = intersec->monitor.traffic[direction].lineup[i-1].speed;
						distAhead = intersec->monitor.traffic[direction].lineup[i-1].y - intersec->monitor.traffic[direction].lineup[i].y ;
					}
					
					//send speedAhead
					sprintf(res, "%d", speedAhead);
					send(clientSocket, res, strlen(res), 0);
					//recieve response from client
					bytesRcv = recv(clientSocket, buffer, sizeof(buffer), 0);
					buffer[bytesRcv] = 0; 
						
					//send dist to next vehicle
					sprintf(res, "%d", distAhead);
					send(clientSocket, res, strlen(res), 0);
				}
				
				//handle eastbound vehicles
				else if (direction == EASTBOUND && intersec->monitor.traffic[direction].lineup[i].x<(INTERSECTION_WIDTH+VEHICLE_WIDTH) ) {
					
					//set response to yes
					response = "YES";
					
					//send state of the traffic light
					char res[2];
					sprintf(res, "%d", intersec->lights[direction].currentState);
					send(clientSocket, res, strlen(res), 0);
					
					//recieve response from client
					bytesRcv = recv(clientSocket, buffer, sizeof(buffer), 0);
					buffer[bytesRcv] = 0;
					
					//calculate and send distance to the traffic light
					int distance;
					if (intersec->monitor.traffic[direction].lineup[i].x > intersec->monitor.traffic[direction].stopX) distance = MAX_DISTANCE;
					else distance =  intersec->monitor.traffic[direction].stopX - intersec->monitor.traffic[direction].lineup[i].x ;
					sprintf(res, "%d", distance);
					send(clientSocket, res, strlen(res), 0);
					
					//recieve response from client
					bytesRcv = recv(clientSocket, buffer, sizeof(buffer), 0);
					buffer[bytesRcv] = 0; 
					
					//calculate speedAhead and distAhead (describe the vehicle ahead of the one being updated)
					int speedAhead;
					int distAhead;
					
					//if the vehicle is the front vehicle
					if (i == 0) {
						speedAhead = VEHICLE_TOP_SPEED + 1;
						distAhead = MAX_DISTANCE;
					}
					//if the vehicle is not the front vehicle
					else {
						speedAhead = intersec->monitor.traffic[direction].lineup[i-1].speed;
						distAhead = intersec->monitor.traffic[direction].lineup[i-1].x - intersec->monitor.traffic[direction].lineup[i].x ;
					}
					
					//send speedAhead
					sprintf(res, "%d", speedAhead);
					send(clientSocket, res, strlen(res), 0);
					
					//recieve response from client
					bytesRcv = recv(clientSocket, buffer, sizeof(buffer), 0);
					buffer[bytesRcv] = 0; 
						
					//send dist to next vehicle
					sprintf(res, "%d", distAhead);
					send(clientSocket, res, strlen(res), 0);
					
				}
				
				//handle westbound vehicles
				else if (direction == WESTBOUND && intersec->monitor.traffic[direction].lineup[i].x>(0-VEHICLE_WIDTH) ) {
					
					//set response to yes
					response = "YES";
					
					//send state of the traffic light
					char res[2];
					sprintf(res, "%d", intersec->lights[direction].currentState);
					send(clientSocket, res, strlen(res), 0);
					
					//recieve response from client
					bytesRcv = recv(clientSocket, buffer, sizeof(buffer), 0);
					buffer[bytesRcv] = 0;
					
					//calculate and send distance to the traffic light
					int distance;
					if (intersec->monitor.traffic[direction].lineup[i].x < intersec->monitor.traffic[direction].stopX) distance = MAX_DISTANCE;
					else distance = intersec->monitor.traffic[direction].lineup[i].x - intersec->monitor.traffic[direction].stopX;
					sprintf(res, "%d", distance);
					send(clientSocket, res, strlen(res), 0);
					
					//recieve response from client
					bytesRcv = recv(clientSocket, buffer, sizeof(buffer), 0);
					buffer[bytesRcv] = 0;
					
					//calculate speedAhead and distAhead (describe the vehicle ahead of the one being updated)
					int speedAhead;
					int distAhead;
					
					//if the vehicle is the front vehicle
					if (i == 0) {
						speedAhead = VEHICLE_TOP_SPEED + 1;
						distAhead = MAX_DISTANCE;
					}
					//if the vehicle is not the front vehicle
					else {
						speedAhead = intersec->monitor.traffic[direction].lineup[i-1].speed;
						distAhead = intersec->monitor.traffic[direction].lineup[i].x - intersec->monitor.traffic[direction].lineup[i-1].x;
					}
					
					//send speedAhead
					sprintf(res, "%d", speedAhead);
					send(clientSocket, res, strlen(res), 0);
					//recieve response from client
					bytesRcv = recv(clientSocket, buffer, sizeof(buffer), 0);
					buffer[bytesRcv] = 0; 
						
					//send dist to next vehicle
					sprintf(res, "%d", distAhead);
					send(clientSocket, res, strlen(res), 0);
					
				}
				
				//this will be reached if the vehicle is now off the stage
				else {
					//set response to no
					response = "NO";
					
					//'bump' all the following cars down in the array, overwriting the one to be 'deleted' in the process
					i++;
					for ( ; i < intersec->monitor.traffic[direction].lineCount ; i++){
						intersec->monitor.traffic[direction].lineup[i-1].x=intersec->monitor.traffic[direction].lineup[i].x;
						intersec->monitor.traffic[direction].lineup[i-1].y=intersec->monitor.traffic[direction].lineup[i].y;
						intersec->monitor.traffic[direction].lineup[i-1].speed=intersec->monitor.traffic[direction].lineup[i].speed;
						intersec->monitor.traffic[direction].lineup[i-1].id=intersec->monitor.traffic[direction].lineup[i].id;
					}
					//decrement the line count
					intersec->monitor.traffic[direction].lineCount--;
				}
			}
			//if the car does not exist, set response to no
			else response = "NO";
			
			//if the response = no, send response.
			// - note: this is not done for response = yes, as that has been handled already.
			if (strcmp("NO", response)==0){
				send(clientSocket, response, strlen(response), 0);
			}
		}

	close(clientSocket); // Close this client's socket
	}

	// Don't forget to close the sockets!
	close(serverSocket);


}
