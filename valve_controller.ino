#include <ESP8266WiFi.h>
#include <urlencode.h>
#include <string.h>
#include <ArduinoJson.h>
#include "credentials.h"
#include <ESP8266WebServer.h>   // Include the WebServer library

IPAddress host;

const int httpPort = 3001;
String authentication_token = "";
const char* device = "";
const char* json_buffer = "";

String name = "ro";
boolean loggedIn = false;
String key = "13e13460f1728c111a68582fd5370a0b";

//settings
boolean on = false;

unsigned long currentMillis = 0;
unsigned long previousReportMillis = 0;

int valvePin = 12;//D6


ESP8266WebServer server(80);
String header;


void setValve(){
  if (on == true){
    digitalWrite(valvePin, HIGH);
  }
  else{
    digitalWrite(valvePin, LOW);
  }
}

//     This rutine is exicuted when you open its IP in browser
//===============================================================

void handleValvePath() {
 String valveState = "";
 Serial.println("Valve request");
 Serial.println(server.args());
 if(server.hasArg("key") && server.arg("key") == key){
  
  if(server.hasArg("valve")){
    valveState = server.arg("valve");
    if(valveState == "open"){
      on = true;
      Serial.println("Valve open");
    }
    else if (valveState == "close"){
      on = false;
      Serial.println("Valve closed");
    }
     previousReportMillis = millis();
  }
  server.send(200, "text/plain", valveState);
 }
 server.send(401, "text/plain", "Unauthorized");
 
}

void setup() {
  pinMode(valvePin, OUTPUT);

  digitalWrite(valvePin, LOW);
  
  Serial.begin(9600);
  delay(10);
  
  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.println("dupa");
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA); // <<< Station
  Serial.println();
  Serial.println();
  wifiConnect();
   
  Serial.println("");
  Serial.println("WiFi connected");
   
  // Print the IP address
  Serial.print("Use this URL to connect: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");

  server.on("/valve", handleValvePath);
  server.begin();
}

void wifiConnect() {
  Serial.print("Connecting to ");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
}

void verifyConnectionStatus() {
  if (WiFi.status() != WL_CONNECTED) {
    if (WiFi.status() != WL_CONNECTION_LOST) {
       Serial.println("connection lost!");
    }
    Serial.println("Connecting...");
    wifiConnect();
  }
}

void loop() {
  verifyConnectionStatus();

  setValve();

  server.handleClient();          //Handle client requests
  if (millis() >= previousReportMillis + 240000) {
    Serial.println("Master has timed out!!!!!!");
    delay(1000);
    on = false;
    
  }
}
