#ifndef CONFIG_H
#define CONFIG_H

/*
 * This example demonstrate how to use asynchronous client & server APIs
 * in order to establish tcp socket connections in client server manner.
 * server is running (on port 7050) on one ESP, acts as AP, and other clients running on
 * remaining ESPs acts as STAs. after connection establishment between server and clients
 * there is a simple message transfer in every 2s. clients connect to server via it's host name
 * (in this case 'esp_server') with help of DNS service running on server side.
 *
 * Note: default MSS for ESPAsyncTCP is 536 byte and defualt ACK timeout is 5s.
*/

#define SSID "SSID"
#define PASSWORD "Password"

#define SERVER_HOST_NAME "esp_server"

#define TCP_SERVER_PORT 4533

#endif // CONFIG_H