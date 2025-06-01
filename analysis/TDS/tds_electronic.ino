//TDS Sensor and ESP32
// Download Libraries
#define BLYNK_TEMPLATE_ID "TMPL3WI6ndq3m"
#include <Arduino.h>
#include <OneWire.h>
#include <DFRobot_ESP_PH_WITH_ADC.h> 
#include <DFRobot_ESP_EC.h>
#include <DallasTemperature.h>
#include <EEPROM.h>
#include <BlynkSimpleEsp32.h>

#define ONE_WIRE_BUS 4                // this is the gpio pin 13 on esp32.
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

DFRobot_ESP_EC ec;
DFRobot_ESP_PH_WITH_ADC ph;

float phvoltage, phValue, phtemperature = 25;
float voltage, ecValue, temperature = 25;

// You should get Auth Token in the Blynk App.
char auth[] = BLYNK_AUTH_TOKEN; // for connection with the blynk

// Your WiFi credentials.
BlynkTimer timer;

float calibvalue = 1.28;  // calibration value


float readTemperature()
{
  //add your code here to get the temperature from your temperature sensor
  sensors.requestTemperatures();
  return sensors.getTempCByIndex(0);
  
}

void setup()
{
  Serial.begin(115200);
  Blynk.begin(auth, ssid, pass);
  EEPROM.begin(32);//needed EEPROM.begin to store calibration k in eeprom
  ph.begin();
  ec.begin();
  sensors.begin();


  timer.setInterval(1000L,MainFunction);
}

void loop()
{

    Blynk.run();
    timer.run(); // Initiates BlynkTimer
    

}

void MainFunction()
{


    voltage = analogRead(36); // A0 is the gpio 36 
    temperature = readTemperature();  // read your temperature sensor to execute temperature compensation
    Serial.print("temperature:");
    Serial.print(temperature, 1); 
    Serial.println("^C");

    ecValue = ec.readEC(voltage, temperature); // convert voltage to EC with temperature compensation
    ecValue = ecValue - calibvalue;
    if( ecValue < 0)
      ecValue = 0;
    Serial.print("EC:");
    Serial.println(ecValue); 

    

// Sensor Values to Blynk application

  Blynk.virtualWrite(V0, temperature);
  Blynk.virtualWrite(V2, ecValue);


  ec.calibration(voltage, temperature); // calibration process by Serail CMD

}