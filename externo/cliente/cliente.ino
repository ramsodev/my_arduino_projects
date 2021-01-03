#include <SimpleDHT.h>

#include <BME280I2C.h>
#include <Wire.h>

#include <nRF24L01.h>
#include <RF24.h>
#include <RF24_config.h>
#include <SPI.h>

#define SERIAL_BAUD 115200

BME280I2C bme; 

int pinDHT22 = 2;
SimpleDHT22 dht22;

const int pinCE = 9;
const int pinCSN = 10;
RF24 radio(pinCE, pinCSN);
 
// Single radio pipe address for the 2 nodes to communicate.
const uint64_t pipe = 0xE8E8F0F0E1LL;


void setup() {
  Serial.begin(SERIAL_BAUD);

  while(!Serial) {} // Wait

  Wire.begin();

  while(!bme.begin())
  {
    Serial.println("Could not find BME280 sensor!");
    delay(1000);
  }

  // bme.chipID(); // Deprecated. See chipModel().
  switch(bme.chipModel())
  {
     case BME280::ChipModel_BME280:
       Serial.println("Found BME280 sensor! Success.");
       break;
     case BME280::ChipModel_BMP280:
       Serial.println("Found BMP280 sensor! No Humidity available.");
       break;
     default:
       Serial.println("Found UNKNOWN sensor! Error!");
  }
  radio.begin();
  radio.openWritingPipe(pipe);
}

void loop() {
  float temperature = 0;
  float humidity = 0;
  float  presure(NAN);
  BME280::PresUnit presUnit(BME280::PresUnit_bar);
  
  int err = SimpleDHTErrSuccess;
  if ((err = dht22.read2(pinDHT22, &temperature, &humidity, NULL)) != SimpleDHTErrSuccess) {
    Serial.print("Read DHT22 failed, err="); Serial.println(err);delay(2000);
    return;
  }
  presure = bme.pres(presUnit);
  
  send(temperature, humidity, presure*1000);
  
  delay(25000);
}

void send(float temp, float hum, float pres){
  float data[3];
  data[0] = temp;
  data[1] = hum;
  data[2] = pres;
  Serial.println("Enviando ");
  Serial.print((float)temp); Serial.print(" *C, ");
  Serial.print((float)hum); Serial.println(" RH%");
  Serial.print((float)pres); Serial.println(" mbar");
  radio.write(data, sizeof data);
}

