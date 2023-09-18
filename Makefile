all: main.cpp nextion_driver.cpp
	g++ -o test -g -std=c++11  main.cpp nextion_driver.cpp -Wall -lpthread -lpaho-mqttpp3 -lpaho-mqtt3as