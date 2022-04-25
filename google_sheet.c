#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "HTTPSRedirect.h"
#include <DHT.h>

#define DHTPIN 12
#define S_Delay 1500
#define S_Delay_A 3000
#define LOOP_Delay 400

int counter = 0;
int right = 16; 
int left = 14;                 

#define DHTTYPE DHT22                         
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
  pinMode(right, INPUT);
  pinMode(left, INPUT);
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
  delay(60000);
}


void loop() {
  int right_bububle = digitalRead(right);
  int left_bubble = digitalRead(left);
  h = dht.readHumidity();                                              // Reading temperature or humidity takes about 250 milliseconds!
  t = dht.readTemperature();                                           // Read temperature as Celsius (the default)

  if (isnan(h) || isnan(t)) {                                                // Check if any reads failed and exit early (to try again).
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
  if (right_bububle == HIGH) {
    Serial.println("Motion detected: RIGHT SIDE");
      delay(S_Delay);
      left_bubble = digitalRead(left);
      if(left_bubble == HIGH){
              Serial.println("\t\t\t(>>>>>>>>>>IN<<<<<<<<<<)");
              counter++;
              delay(S_Delay_A);
        }
    }

    right_bububle = digitalRead(right);
    left_bubble = digitalRead(left);
    
    if (left_bubble == HIGH) {
    Serial.println("Motion detected: LEFT SIDE");
    delay(S_Delay);
    right_bububle = digitalRead(right);
      if(right_bububle == HIGH){ 
        Serial.println("\t\t\t(<<<<<<<<<<OUT>>>>>>>>>>)");
        counter--;
        delay(S_Delay_A); 
        }
    }
    else {
          Serial.print("\t\t\t###NOTHING###COUNTER: ");
          Serial.print(counter);
          Serial.print("###\n");
          }

    if(counter < 0){ counter = 0;}

  while(loop_counter == 20){
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
  payload = payload_base + "\"" + sheetHumid + "," + sheetTemp + "," + counter + "\"}";
  
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