#define INTERSECTION_WIDTH        800   // The width and height of the window
#define INTERSECTION_HEIGHT       600
#define ROAD_WIDTH           	   40	// Width of the vertical or horizontal road

#define TRAFFIC_LIGHT_SIZE	   10	// Size of the raffic light (in pixels)
#define MAX_DISTANCE		65535   // A large value indicating that no RED_LIGHT or no vehicle is ahead

#define NORTHBOUND          	    0	// Directions for traffic flow.  These are the indices
#define SOUTHBOUND          	    1	// into the Vehicle.lineups in the TrafficLineup struct
#define EASTBOUND          	    2
#define WESTBOUND          	    3

#define MAX_CONNECTIONS        	    9   // Maximum # of vehicles allowed to line up in one direction at the intersection

#define SERVER_IP     		    "127.0.0.1"   // IP address of simulator server
#define SERVER_PORT          	 	 6000   // PORT of the simulator server

// Command codes sent from client to server

#define SHUTDOWN           				1
#define CONNECT            				2
#define UPDATE             				3

// Reply codes sent from server to client
#define YES                				5
#define NO                 				6


typedef struct {
  	short    	entryX;
  	short           entryY;
  	short    	stopX;
  	short           stopY;
  	short    	exitX;
  	short           exitY;
  	Vehicle   	lineup[MAX_CONNECTIONS]; //an array of vehicles
  	unsigned char	lineCount; //how many vehicles are in line in any given moment
} TrafficLineup;


typedef struct {
  	char            online; // 0 = no, 1 = yes
  	TrafficLineup   traffic[4]; //4 lines, one for each direction
  	unsigned char   idCounter[4]; //a unique id counter of each direction
} TrafficMonitor;

typedef struct {
	TrafficLight	lights[4];
  	TrafficMonitor 	monitor;
} Intersection;





