all:         simulator.o vehicle.o generator.o stop.o
	     gcc -o simulator simulator.o  -lX11 -lpthread
	     gcc -o vehicle vehicle.o -lm 
	     gcc -o generator generator.o 
	     gcc -o stop stop.o 

simulator.o:   simulator.c display.c vehicle.c trafficLight.c trafficServer.c movementTimer.c simulator.h trafficLight.h
	     gcc -c simulator.c

vehicle.o:     vehicle.c simulator.h vehicle.h
	     gcc -c vehicle.c

generator.o:   generator.c simulator.h trafficLight.h
	     gcc -c generator.c

stop.o:        stop.c simulator.h trafficLight.h
	     gcc -c stop.c

clean:
	     rm -f simulator.o vehicle.o generator.o stop.o simulator vehicle generator stop
