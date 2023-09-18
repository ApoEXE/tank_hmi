// testing commands to save
//.\mosquitto_pub.exe -h 192.168.0.250 -t Tanque1/test/sensor1 -m 1

#include "nextion_driver.h"
#include <time.h> // So we can use time() function
#include <cmath>
// Start by `#include`-ing the Mosquitto MQTT Library and other standard libraries.
#include <mqtt/client.h> // Mosquitto client.
#include <ostream>       // std::cout.
#include <pthread.h>
pthread_mutex_t mut;
int main()
{
    using namespace std::chrono;

    Nextion_driver display = Nextion_driver();

    srand((unsigned)time(NULL)); // Randomize seed initialization
    display.write_com("cle 6,0");

    // MQTT
    //  In order to connect the mqtt client to a broker,
    //  Define an Ip address pointing to a broker. In this case, the localhost on port 1883.
    std::string ip = "localhost:1883";
    // Then, define an ID to be used by the client when communicating with the broker.
    std::string id = "consumer";

    // Construct a client using the Ip and Id, specifying usage of MQTT V5.
    mqtt::client client(ip, id, mqtt::create_options(MQTTVERSION_3_1));
    // Use the connect method of the client to establish a connection to the broker.
    client.connect();
    // In order to receive messages from the broker, specify a topic to subscribe to.
    client.subscribe("Tanque1/#");
    // Begin the client's message processing loop, filling a queue with messages.
    client.start_consuming();
    bool running = true;
    char buff[100];
    char payload[] = {'1'};
    clock_t start = clock();
    clock_t stop = clock();

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
            std::string messageString = messagePointer->get_payload_str();
            std::string topic = messagePointer->get_topic();
            // Print payload string to console (debugging).
            // std::cout << messageString << std::endl;

            // Perform processing on the string.
            // This is where message processing can be passed onto different
            // functions for parsing.
            // Here, we break the loop and exit the program if a `quit` is received.
            if (messageString == "quit")
            {
                running = false;
            }
            if (topic == "Tanque1/canal/temperature/sensor1")
            {
                sprintf(buff, "%s\"%s\"", "t1.txt=", messageString.c_str());
                // printf("%s\n", buff);
                display.write_com(buff);
                sprintf(buff, "%s\"%s\"", "t0.txt=", "Ch.Temperature");
                display.write_com(buff);
            }
            if (topic == "Tanque1/canal/tds/sensor1")
            {
                sprintf(buff, "%s\"%s\"", "t3.txt=", messageString.c_str());
                display.write_com(buff);
                sprintf(buff, "%s\"%s\"", "t2.txt=", "Ch.TDS");
                display.write_com(buff);
            }
            if (topic == "Tanque1/canal/turbidity/sensor1")
            {
                sprintf(buff, "%s\"%s\"", "t5.txt=", messageString.c_str());
                display.write_com(buff);
                sprintf(buff, "%s\"%s\"", "t4.txt=", "Ch.Turbidity");
                display.write_com(buff);
            }
            if (topic == "Tanque1/canal/level/sensor1")
            {
                sprintf(buff, "%s\"%s\"", "t7.txt=", messageString.c_str());
                display.write_com(buff);
                sprintf(buff, "%s\"%s\"", "t6.txt=", "Ch.Level");
                display.write_com(buff);
            }
            if (topic == "Tanque1/canal/flow/sensor1")
            {
                sprintf(buff, "%s\"%s\"", "t9.txt=", messageString.c_str());
                display.write_com(buff);
                sprintf(buff, "%s\"%s\"", "t8.txt=", "Ch.flow");
                display.write_com(buff);
            }
            if (topic == "Tanque1/canal/current/sensor1")
            {
                sprintf(buff, "%s\"%s\"", "t11.txt=", messageString.c_str());
                display.write_com(buff);
                sprintf(buff, "%s\"%s\"", "t10.txt=", "airflow (A)");
                display.write_com(buff);
            }
            if (topic == "Tanque1/canal/ph/sensor1")
            {
                sprintf(buff, "%s\"%s\"", "t13.txt=", messageString.c_str());
                display.write_com(buff);
                sprintf(buff, "%s\"%s\"", "t12.txt=", "Ch.PH");
                display.write_com(buff);
            }
        }
        stop = clock();
        double elapsed = double(stop - start) / CLOCKS_PER_SEC;
        if (elapsed >= 0.02)
        {
            start = stop;
            pthread_mutex_lock(&mut);
            switch (display.publish)
            {
            case 0:
                payload[0] = '1';
                client.publish("Tanque1/canal/airflow/motor1", payload, 1);
                display.publish = 99;
                break;
            case 1:
                payload[0] = '1';
                client.publish("Tanque1/canal/reposo/valve1", payload, 1);
                display.publish = 99;
                break;
            case 2:
                payload[0] = '1';
                client.publish("Tanque1/canal/reserva/valve1", payload, 1);
                display.publish = 99;
                break;
            case 3:
                payload[0] = '1';
                client.publish("Tanque1/canal/desague/valve1", payload, 1);
                display.publish = 99;
                break;
            case 4:
                payload[0] = '0';
                client.publish("Tanque1/canal/turbidity/sensor1", payload, 1);
                usleep(1000);
                payload[0] = '1';
                client.publish("Tanque1/canal/turbidity/sensor1", payload, 1);
                display.publish = 99;
                break;
            default:
                break;
            }
            pthread_mutex_unlock(&mut);
        }
    }
    return 0;
}