#include <SimpleDHT.h>

#include <WiFiEspClient.h>
#include <WiFiEsp.h>
#include <WiFiEspUdp.h>
#include <PubSubClient.h>
#include "SoftwareSerial.h"

#define SERIAL_BAUD 115200

#define WIFI_AP "ramsonet"
#define WIFI_PASSWORD "year33old"

#define WIFI_BAUD 9600

WiFiEspClient espClient;

PubSubClient client(espClient);

SoftwareSerial soft(2, 3); // RX, TX

int status = WL_IDLE_STATUS;
unsigned long lastSend;



int pinDHT22 = 4;
SimpleDHT22 dht22;



void setup() {
  Serial.begin(SERIAL_BAUD);

  while (!Serial) {} // Wait

  InitWiFi();
  client.setServer( "nas", 1883 );
  lastSend = 0;

  

}

void InitWiFi()
{
  // initialize serial for ESP module
  soft.begin(WIFI_BAUD);
  // initialize ESP module
  WiFi.init(&soft);
  // check for the presence of the shield
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue
    while (true);
  }

  Serial.println("Connecting to AP ...");
  // attempt to connect to WiFi network
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(WIFI_AP);
    // Connect to WPA/WPA2 network
    status = WiFi.begin(WIFI_AP, WIFI_PASSWORD);
    delay(500);
  }
  Serial.println("Connected to AP");
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Connecting to mqtt server ...");
    // Attempt to connect (clientId, username, password)
    if ( client.connect("Arduino Uno Device", NULL, NULL) ) {
      Serial.println( "[DONE]" );
    } else {
      Serial.print( "[FAILED] [ rc = " );
      Serial.print( client.state() );
      Serial.println( " : retrying in 5 seconds]" );
      // Wait 5 seconds before retrying
      delay( 5000 );
    }
  }
}

void loop() {
  float temperature = 0;
  float humidity = 0;
 
  int err = SimpleDHTErrSuccess;
  if ((err = dht22.read2(pinDHT22, &temperature, &humidity, NULL)) != SimpleDHTErrSuccess) {
    Serial.print("Read DHT22 failed, err="); Serial.println(err); delay(2000);
    return;
  }
  

  send(temperature, humidity, 0);

  delay(25000);
}

void send(float temp, float hum, float pres) {
  status = WiFi.status();
  if ( status != WL_CONNECTED) {
    while ( status != WL_CONNECTED) {
      Serial.print("Attempting to connect to WPA SSID: ");
      Serial.println(WIFI_AP);
      // Connect to WPA/WPA2 network
      status = WiFi.begin(WIFI_AP, WIFI_PASSWORD);
      delay(500);
    }
    Serial.println("Connected to AP");
  }

  if ( !client.connected() ) {
    reconnect();
  }

  if ( millis() - lastSend > 1000 ) { // Update and send only after 1 seconds
    // Prepare a JSON payload string
    String payload = "{";
    payload += "\"temperature\":"; payload += temp; payload += ",";
    payload += "\"humidity\":"; payload += hum; payload += ",";
    payload += "\"presure\":"; payload += pres;
    payload += "}";

    // Send payload
    char attributes[100];
    payload.toCharArray( attributes, 100 );
    client.publish( "home/climate1", attributes );
    Serial.println( attributes );
    lastSend = millis();
  }

  client.loop();
}

