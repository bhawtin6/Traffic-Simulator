Simulates a 4-way intersection where cars approach and obey traffic lights. Cars also respect one another and avoid colliding with cars in front of them. Cars move gracefully, and gently accelerate/decelerate according to their situation.

FILES INCLUDED:
	simulator.h
	trafficLight.h
	vehicle.h
	display.c
	generator.c
	movementTimer.c
	simulator.c
	stop.c
	trafficLight.c
	trafficServer.c
	vehicle.c
	makefile

REQUIREMENTS
	C compiler
	libX11

RUNNING INSTRUCTIONS
download all files and place them in a known directory together
in terminal, navigate to the directory and use command "make" to compile source code.
launch the simulator in the background using command "./simulator &"
launch the generator in the foreground using command "./generator"
to stop the generator, press ctrl+c in terminal
to stop the simulator, type "./stop" in terminal


