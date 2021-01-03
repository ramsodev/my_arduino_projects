
int ADCpins[] = {34, 35, 32, 33, 25, 26, 27, 14, 12, 22, 19, 23, 18, 5, 17, 16, 4, 0, 2, 15, 13}; // these are the ADC pins
int pinCount = 20;
float VBAT;  // battery voltage from ESP32 ADC read float ADC_divider = 250/30;  // voltage divider proportions - hypothetical so far :-)

void setup() {
  Serial.begin(115200);

  while (!Serial) {} // Wait
  float ADC_divider = 1.33;
  for (int thisPin = 0; thisPin < pinCount; thisPin++) {

    Serial.print(thisPin, DEC);
    Serial.print(" = ");
    Serial.print(ADCpins[thisPin], DEC);
    Serial.print(" => ");

    pinMode(ADCpins[thisPin], INPUT);

 
    float VBAT = (ADC_divider * 3.15f * (float)(analogRead(ADCpins[thisPin]))) / 4096.0;
    // LiPo battery voltage in volts
    Serial.print("Vbat = ");
    Serial.print(VBAT);
    Serial.println(" Volts");
  }

}

void loop() {
  // put your main code here, to run repeatedly:

}
