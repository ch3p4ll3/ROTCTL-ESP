#include <Arduino.h>
#include <AsyncTCP.h>
#include <DNSServer.h>
#include <WiFi.h>

#include "config.h"

double currentAzimuth = 0.0;
double currentElevation = 0.0;

double targetAzimuth = 0.0;
double targetElevation = 0.0;

#ifndef CONFIG_H
#define CONFIG_H

#define SSID "esp-tcp-server"
#define PASSWORD "123456789"

#define SERVER_HOST_NAME "esp-tcp-server"

#define TCP_SERVER_PORT 8000
#define DNS_PORT 53

#endif // CONFIG_H

static DNSServer DNS;

static void handleData(void *arg, AsyncClient *client, void *data, size_t len)
{
	Serial.printf("\n data received from client %s \n", client->remoteIP().toString().c_str());

  String decodedData = String((uint8_t *)data, len);
  String toSendString;

  if (decodedData.startsWith("p")){
    Serial.println("Get current position");
    toSendString = String(currentAzimuth);
    toSendString += "\n";
    toSendString += String(currentElevation);
    toSendString += "\n";
  }

  else if (decodedData.startsWith("P")){
    Serial.println("Set position" + decodedData);
    String numbers = decodedData.substring(2);
    int indexOfSpace = numbers.indexOf(' ');

    String azim = numbers.substring(0, indexOfSpace);
    String elev = numbers.substring(indexOfSpace + 1);

    targetAzimuth = azim.toDouble();
    targetElevation = elev.toDouble();
  
    toSendString = "RPRT 0\n";
  }

  else if (decodedData.startsWith("S")){
    toSendString = "RPRT 0\n";
  }

	if (client->space() > strlen(toSendString.c_str()) && client->canSend())
	{
		client->add(toSendString.c_str(), strlen(toSendString.c_str()));
		client->send();
	}
}

static void handleError(void *arg, AsyncClient *client, int8_t error)
{
	Serial.printf("\n connection error %s from client %s \n", client->errorToString(error), client->remoteIP().toString().c_str());
}

static void handleDisconnect(void *arg, AsyncClient *client)
{
	Serial.printf("\n client %s disconnected \n", client->remoteIP().toString().c_str());
}

static void handleTimeOut(void *arg, AsyncClient *client, uint32_t time)
{
	Serial.printf("\n client ACK timeout ip: %s \n", client->remoteIP().toString().c_str());
}

static void handleNewClient(void *arg, AsyncClient *client)
{
	Serial.printf("\n new client has been connected to server, ip: %s", client->remoteIP().toString().c_str());
	// register events
	client->onData(&handleData, NULL);
	client->onError(&handleError, NULL);
	client->onDisconnect(&handleDisconnect, NULL);
	client->onTimeout(&handleTimeOut, NULL);
}

void setup()
{
  Serial.begin(115200);

  WiFi.mode(WIFI_STA); //Optional
  WiFi.begin(SSID, PASSWORD);
  Serial.println("\nConnecting");

  while(WiFi.status() != WL_CONNECTED){
  Serial.print(".");
  delay(100);
  }

  Serial.println("\nConnected to the WiFi network");
  Serial.print("Local ESP32 IP: ");
  Serial.println(WiFi.localIP());

  // // create access point
  // while (!WiFi.softAP(SSID, PASSWORD, 6, false, 15))
  // {
  // 	delay(500);
  // 	Serial.print(".");
  // }

  // start dns server

  AsyncServer *server = new AsyncServer(TCP_SERVER_PORT); // start listening on tcp port 7050
  server->onClient(&handleNewClient, server);
  server->begin();
}

void loop()
{
  if (targetAzimuth > currentAzimuth){
    currentAzimuth += 0.00005;
  }
  else if (targetAzimuth < currentAzimuth){
    currentAzimuth -= 0.00005;
  }

  if (targetElevation > currentElevation){
    currentElevation += 0.00005;
  }
  else if (targetElevation < currentElevation){
    currentElevation -= 0.00005;
  }
}