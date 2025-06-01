#include <Arduino.h>
const int adcPin = 35;
  
// calculate your own m using ph_calibrate.ino
// When using the buffer solution of pH4 for calibration, m can be derived as:
 // m = (pH7 - pH4) / (Vph7 - Vph4)
const float m = -5.436; 

void setup()
{
   Serial.begin(9600);
}

void loop() 
{
   float Po = analogRead(adcPin) * 3.3 / 4095;
   float phValue = 7 - (3.30 - Po) * m;
   Serial.println(analogRead(adcPin));
   Serial.print("p h value = "); 
   Serial.println(phValue);
   delay(1000);
}