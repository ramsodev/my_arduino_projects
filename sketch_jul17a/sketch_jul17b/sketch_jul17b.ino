#include <SoftwareSerial.h>
SoftwareSerial BT1(3, 2); // RX | TX

void setup()
  {  
    Serial.begin(115200);
    Serial.print("inicio");
    BT1.begin(115200);
  }

void loop()
  {  String B= "." ;
     if (BT1.available())
         { char c = BT1.read() ;
           Serial.print(c);
         }
     if (Serial.available())
         {  char c = Serial.read();
            BT1.print(c);
         }
   }
