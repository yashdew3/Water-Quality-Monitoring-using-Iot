#define BLYNK_PRINT Serial            //Defines object used for printing
#include <WiFi.h>                     //to use esp32 wifi functionalities
#include <WiFiClient.h>               //Creates a client that can connect to to a specified internet IP address and port as defined in client.
#include <BlynkSimpleEsp32.h>         //to connect blynk with esp32
#include <OneWire.h>                  //It is a protocol and communication channel
#include <DallasTemperature.h>        //use the functionalities of ds18b20

#define DS18B20 4    //Connect to GPIO4 pin
OneWire ourWire(DS18B20);            //communicate with the sensor
DallasTemperature sensor(&ourWire);

char auth[] = BLYNK_AUTH_TOKEN; // for connection with the blynk


char ssid[] = "";  // Enter your wifi name
char pass[] = "";  // Enter your wifi password

BlynkTimer timer;           // timer which keeps track of how many milliseconds have elapsed since boot-up

void sendSensor()             // to read the temperature from the DS18B20 sensor and send the data to the Blynk app
{
  // Request temperature to all devices on the data line
  sensor.requestTemperatures(); 
  
  Serial.print("Celsius temperature: ");
  //We can connect more than one IC on the same data wire. 0 refers to the first IC on the wire
  Serial.print(sensor.getTempCByIndex(0));  //returns the temperature in Celsius
  Serial.print(" - Fahrenheit temperature: ");
  Serial.println(sensor.getTempFByIndex(0));
  int tempC=sensor.getTempCByIndex(0);
  int tempF=sensor.getTempFByIndex(0);
  delay(1000);
  
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
    Blynk.virtualWrite(V0, tempC);
    Blynk.virtualWrite(V1, tempF);
    delay(500);
}
void setup()
{   
  
   Serial.begin(9600);          //how fast the data is to be sent
  sensor.begin();

  Blynk.begin(auth, ssid, pass);
  timer.setInterval(100L, sendSensor);          //to set a delay for functions that are executed repeatedly.
 
  }

void loop()
{
  Blynk.run();
  timer.run();
 }