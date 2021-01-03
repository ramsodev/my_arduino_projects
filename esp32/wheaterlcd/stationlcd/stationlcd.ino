
#include <Wire.h>
#include <BME280I2C.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <PubSubClient.h>
#include <LiquidCrystal_I2C.h>


#define SERIAL_BAUD 115200

#define uS_TO_S_FACTOR 1000000  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  30        /* Time ESP32 will go to sleep (in seconds) */

BME280I2C::Settings settings(
  BME280::OSR_X1,
  BME280::OSR_X1,
  BME280::OSR_X1,
  BME280::Mode_Forced,
  BME280::StandbyTime_1000ms,
  BME280::Filter_Off,
  BME280::SpiEnable_False,
  0x76 // I2C address. I2C specific.
);

BME280I2C bme(settings);

const char* ssid = "ramsonet";
const char* password = "year33old";
const char* mqtt_server = "192.168.1.2";
String clientid = "climate";
String topic = "home/";


const int VBAT_PIN = 33;
const float ADC_divider = 1330.0f / 1000.0f;

WiFiClient espClient;
PubSubClient client(espClient);

//LiquidCrystal_I2C lcd(0x3F, 16, 2);

const char* ntpServer = "es.pool.ntp.org";
const long  gmtOffset_sec = 3600;
const int   daylightOffset_sec = 3600;

void setup() {
  Serial.begin(SERIAL_BAUD);

  while (!Serial) {} // Wait

  Wire.begin(22, 23);

 //setup_lcd();

  while (!bme.begin())
  {
    Serial.println("Could not find BME280 sensor!");
    delay(1000);
  }


  setup_wifi();
  setupClientID();
  client.setServer(mqtt_server, 1883);



}

void setup_lcd(){
// lcd.begin(22, 23);
// lcd.backlight();
}

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
}

void setupClientID() {
  IPAddress ip = WiFi.localIP();
  String value = String(ip[3]);
  clientid += value.substring(value.length() - 1, value.length());
  topic += clientid;
  Serial.println(clientid);
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(clientid.c_str())) {
      Serial.println("connected");

    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}



void loop() {
  if ( !client.connected() ) {
    reconnect();
  }

  BME280::PresUnit presUnit(BME280::PresUnit_bar);
  BME280::TempUnit tempUnit(BME280::TempUnit_Celsius);

  float pres = bme.pres(presUnit) * 1000.0;
  float temp = bme.temp(tempUnit);
  float hum = bme.hum();
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return;
  }
  char buff[20];
  strftime(buff, 20, "%H:%M:%S - %d-%m-%Y ", &timeinfo);
  pinMode(VBAT_PIN, INPUT); 
  float VBAT = (ADC_divider * 3.158f * (float)(analogRead(VBAT_PIN))) / 4096.0;

  /*  Voltage --- Charge state
    4.2 V --- 100 %
    4.1 V --- 90 %
    4.0 V --- 80 %
    3.9 V --- 60 %
    3.8 V --- 40 %
    3.7 V --- 20 %
    3,6 V --- 0 % */
  String payload = "{";
  payload +=  "\"time\":\""; payload += buff ; payload += "\",";
  payload += "\"temperature\":"; payload += temp; payload += ",";
  payload += "\"humidity\":"; payload += hum; payload += ",";
  payload += "\"presure\":"; payload += pres; payload += ",";
  payload += "\"voltaje\":"; payload += VBAT;
  payload += "}";

  if (!client.publish( topic.c_str() , payload.c_str(), true)) {
    Serial.println("Error de publicacion");
  }
  Serial.println( payload );

  String msg = "";
  msg += temp; msg += " C - ";
  msg += hum; msg += "%";

  strftime(buff, 20, "%H:%M", &timeinfo);
  String msg2 = "";
  msg2 += buff; msg2 += " ";
  msg2 += int(pres); msg2 += "b ";
  msg2 += VBAT; msg2 += "v";
  //lcd_print(msg, msg2);
  delay(60000);
  //  esp_wifi_stop();
  //  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  //  esp_deep_sleep_start();

}

void lcd_print(String l1, String l2) {
/*  int l1s = l1.length();
  int l2s = l2.length();
  lcd.clear();
  lcd.home();
  lcd.print(l1);
  lcd.setCursor(0, 1);
  lcd.print(l2); 
  int fin = 0;
  if (l1s > 16 ) {
    fin = l1s;
  } else if (l2s > 16 && l2s > fin ) {
    fin = l2s;
  }
  fin -= 16;
  if (fin > 0 ) {
    for (int positionCounter = 0; positionCounter < fin; positionCounter++) {
      // scroll one position left:
      lcd.scrollDisplayLeft();
      // wait a bit:
      delay(500);
    }
    delay(10000);
    for (int positionCounter = 0; positionCounter < fin; positionCounter++) {
      // scroll one position right:
      lcd.scrollDisplayRight();
      // wait a bit:
      delay(500);
    }
  } */
}


