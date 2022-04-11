#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "HTTPSRedirect.h"
#include <DHT.h>

#define DHTPIN 5                                                           // what digital pin we're connected to
#define RIGHT_S 4                                                           // what digital pin we're connected right sensor to
#define LEFT_S 14                                                           // what digital pin we're connected left sensor to

#define DHTTYPE DHT22                                                       // select dht type as DHT 11 or DHT22
DHT dht(DHTPIN, DHTTYPE);

float h;
float t;
String sheetHumid = "";
String sheetTemp = "";
int people_count = 0;
int loop_counter = 0;

// Enter network credentials:
const char* ssid     = "sigma-guest";
const char* password = "starforlife2005";

// Enter Google Script Deployment ID:
const char *GScriptId = "AKfycbyd8zByoh1pEv8lf9VNWb9QASjXZEEwOeeNFkKv0bTm3NbEpvk";

// Enter command (insert_row or append_row) and your Google Sheets sheet name (default is Sheet1):
String payload_base =  "{\"command\": \"insert_row\", \"sheet_name\": \"TempSheet\", \"values\": ";
String payload = "";

// Google Sheets setup (do not edit)
const char* host = "script.google.com";
const int httpsPort = 443;
const char* fingerprint = "";
String url = String("/macros/s/") + GScriptId + "/exec";
HTTPSRedirect* client = nullptr;

// Declare variables that will be published to Google Sheets
int value0 = 0;
int value1 = 0;
int value2 = 0;

void setup() {
  pinMode(RIGHT_S, INPUT);
  pinMode(LEFT_S, INPUT);
  Serial.begin(9600);        
  delay(10);
  Serial.println('\n');
  dht.begin();     //initialise DHT22
  
  // Connect to WiFi
  WiFi.begin(ssid, password);             
  Serial.print("Connecting to ");
  Serial.print(ssid); Serial.println(" ...");

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println('\n');
  Serial.println("Connection established!");  
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());

  // Use HTTPSRedirect class to create a new TLS connection
  client = new HTTPSRedirect(httpsPort);
  client->setInsecure();
  client->setPrintResponseBody(true);
  client->setContentTypeHeader("application/json");
  
  Serial.print("Connecting to ");
  Serial.println(host);

  // Try to connect for a maximum of 5 times
  bool flag = false;
  for (int i=0; i<5; i++){ 
    int retval = client->connect(host, httpsPort);
    if (retval == 1){
       flag = true;
       Serial.println("Connected");
       break;
    }
    else
      Serial.println("Connection failed. Retrying...");
  }
  if (!flag){
    Serial.print("Could not connect to server: ");
    Serial.println(host);
    return;
  }
  delete client;    // delete HTTPSRedirect object
  client = nullptr; // delete HTTPSRedirect object
}


void loop() {
  int right_bubble = digitalRead(RIGHT_S);
  int left_bubble = digitalRead(LEFT_S);
  h = dht.readHumidity();                                              // Reading temperature or humidity takes about 250 milliseconds!
  t = dht.readTemperature();                                           // Read temperature as Celsius (the default)

  if (isnan(h) || isnan(t)) {                                                // Check if any reads failed and exit early (to try again).
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

if (right_bubble == HIGH){
    Serial.println(" Motion Detected, right!\n");
    delay(1000);
    if(left_bubble == HIGH){
                                         Serial.println("<<<<\n");
                                         people_count++;
                                         delay(3000);
                                         }
                            }
                                       
if (left_bubble == HIGH){
    Serial.println(" Motion Detected, left!\n");
    delay(1000);
    if(right_bubble == HIGH){
                                         Serial.println(">>>>\n");
                                         people_count--;
                                         delay(3000);
                                         }
                            }
              else{
                   Serial.print("NO MOTION, # of people inside:%");
                   Serial.print(people_count);
                   Serial.println();
                   
              }
              delay(500);
  while(loop_counter == 10){
  Serial.print("Humidity: ");  Serial.print(h);
  sheetHumid = String(h) + String("%");                                         //convert integer humidity to string humidity
  Serial.print("%  Temperature: ");  Serial.print(t);  Serial.println("°C ");
  sheetTemp = String(t) + String("°C");

  static bool flag = false;
  if (!flag){
    client = new HTTPSRedirect(httpsPort);
    client->setInsecure();
    flag = true;
    client->setPrintResponseBody(true);
    client->setContentTypeHeader("application/json");
  }
  if (client != nullptr){
    if (!client->connected()){
      client->connect(host, httpsPort);
    }
  }
  else{
    Serial.println("Error creating client object!");
  }
  
  // Create json object string to send to Google Sheets
  payload = payload_base + "\"" + sheetHumid + "," + sheetTemp + "\"}";
  
  // Publish data to Google Sheets
  Serial.println("Publishing data...");
  Serial.println(payload);
  if(client->POST(url, host, payload)){ 
    // do stuff here if publish was successful
  }
  else{
    // do stuff here if publish was not successful
    Serial.println("Error while connecting");
  }
   loop_counter = 0;
  }
  loop_counter++;
  // a delay of several seconds is required before publishing again    
  delay(500);
}