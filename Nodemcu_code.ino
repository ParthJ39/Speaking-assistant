#include <ArduinoJson.h>
#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

// Declare the "link" serial port
// Please see SoftwareSerial library for detail
SoftwareSerial linkSerial(D2, D3); // RX, TX

const char* wifi_ssid = "SAKEC_St";             // SSID
const char* wifi_password = "S@k3c@st";         // WIFI
const unsigned int writeInterval = 25000;   // write interval (in ms)

String https_host = "api.asksensors.com"; // ASKSENSORS API host name
const int https_port = 443;                        // https port
const char* https_fingerprint =  "B5 C3 1B 2C 0D 5D 9B E5 D6 7C B6 EF 50 3A AD 3F 9F 1E 44 75";     // ASKSENSORS HTTPS SHA1 certificate
const char* apiKeyIn = "Jemz4ONP18p6HIQNORHvmUqDs9mLvEgi"; 

WiFiClientSecure client;

void setup() {
  // Initialize "debug" serial port
  // The data rate must be much higher than the "link" serial port
  Serial.begin(115200);
  while (!Serial) continue;

  // Initialize the "link" serial port
  // Use the lowest possible data rate to reduce error ratio
  linkSerial.begin(4800);

  Serial.println();
  Serial.print("********** connecting to WIFI : ");
  Serial.println(wifi_ssid);
  WiFi.begin(wifi_ssid, wifi_password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("-> WiFi connected");
  Serial.println("-> IP address: ");
  Serial.println(WiFi.localIP());
  
  // Set Client to insecure
  client.setInsecure();
}
 
void loop() {
  // Check if the other Arduino is transmitting
  Serial.print("********** connecting to HOST : ");
  Serial.println(https_host);
  if (!client.connect(https_host, https_port)) {
    Serial.println("-> connection failed");
  }
  if (linkSerial.available()) 
  {
    // Allocate the JSON document
    // This one must be bigger than for the sender because it must store the strings
    StaticJsonDocument<300> doc;

    // Read the JSON document from the "link" serial port
    DeserializationError err = deserializeJson(doc, linkSerial);

    if (err == DeserializationError::Ok) 
    {
      Serial.println(doc["value"].as<int>());

      String url = "/write/";
      url += apiKeyIn;
      url += "?module1=";
      url += doc["value"].as<int>();
  
      Serial.print("********** requesting URL: ");
      Serial.println(url);

      client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                   "Host: " + https_host + "\r\n" +
                   "Connection: close\r\n\r\n");
                   
      Serial.println("> Request sent to ASKSENSORS");
    } 
    else 
    {
      // Print error to the "debug" serial port
      Serial.print("deserializeJson() returned ");
      Serial.println(err.c_str());
  
      // Flush all bytes in the "link" serial port buffer
      while (linkSerial.available() > 0)
        linkSerial.read();
    }
  }
}
