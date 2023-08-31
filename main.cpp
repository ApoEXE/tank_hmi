// testing commands to save
//.\mosquitto_pub.exe -h 192.168.0.250 -t Tanque1/test/sensor1 -m 1

#include "nextion_driver.h"
#include <time.h> // So we can use time() function
#include <cmath>
// Start by `#include`-ing the Mosquitto MQTT Library and other standard libraries.
#include <mqtt/client.h>  // Mosquitto client.
#include <ostream>  // std::cout.

int main(){

    Nextion_driver display = Nextion_driver();

    srand((unsigned) time(NULL)); //Randomize seed initialization
    display.write_com("cle 6,0");


    //MQTT
    // In order to connect the mqtt client to a broker, 
    // Define an Ip address pointing to a broker. In this case, the localhost on port 1883.
    std::string ip = "localhost:1883";
    // Then, define an ID to be used by the client when communicating with the broker.
    std::string id = "consumer";
 
    // Construct a client using the Ip and Id, specifying usage of MQTT V5.
    mqtt::client client(ip, id, mqtt::create_options(MQTTVERSION_3_1));
    // Use the connect method of the client to establish a connection to the broker.
    client.connect();
    // In order to receive messages from the broker, specify a topic to subscribe to.
    client.subscribe("Tanque1/test/#");
    // Begin the client's message processing loop, filling a queue with messages.
    client.start_consuming();
    bool running = true;


    while (running)
    {
        // Construct a message pointer to hold an incoming message.
        mqtt::const_message_ptr messagePointer;
        // Try to consume a message, passing messagePointer by reference.
        // If a message is consumed, the function will return `true`, 
        // allowing control to enter the if-statement body.
        if (client.try_consume_message(&messagePointer))
        {
            // Construct a string from the message payload.
            std::string messageString = messagePointer -> get_payload_str();
            // Print payload string to console (debugging).
            std::cout << messageString << std::endl;
 
            // Perform processing on the string.
            // This is where message processing can be passed onto different
            // functions for parsing. 
            // Here, we break the loop and exit the program if a `quit` is received.
            if (messageString == "quit")
            {
                running = false;
            } 
            char buff[100];
            
            sprintf(buff,"%s\"%s\"","t1.txt=",messageString.c_str());
            printf("%s\n",buff);
            display.write_com(buff);
            
        }

    }
    
    
    return 0;
}