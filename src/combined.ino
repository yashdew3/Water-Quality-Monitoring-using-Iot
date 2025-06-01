#define BLYNK_PRINT Serial            //Defines object used for printing
#include <WiFi.h>                     //to use esp32 wifi functionalities
#include <WiFiClient.h>               //Creates a client that can connect to to a specified internet IP address and port as defined in client.
#include <BlynkSimpleEsp32.h>         //to connect blynk with esp32
#include <OneWire.h>                  //It is a protocol and communication channel
#include <DallasTemperature.h>        //use the functionalities of ds18b20
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//display
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// TDS sensor
#define TdsSensorPin 32
#define VREF 3.3      // analog reference voltage(Volt) of the ADC
#define SCOUNT  30           // sum of sample point
int analogBuffer[SCOUNT];    // store the analog value in the array, read from ADC
int analogBufferTemp[SCOUNT];
int analogBufferIndex = 0,copyIndex = 0;
float averageVoltage = 0,tdsValue = 0,temperature = 25;


// Turbidity
int sensorPin = 34; // Analog pin connected to the turbidity sensor
int turbidity;

// Temperature
#define DS18B20 4    //Connect to GPIO4 pin
OneWire ourWire(DS18B20);            //communicate with the sensor
DallasTemperature sensor(&ourWire);
int tempC;
int tempF;


// Blynk setup
char auth[] = BLYNK_AUTH_TOKEN; // for connection with the blynk
char ssid[] = "";  // Enter your wifi name
char pass[] = "";  // Enter your wifi password

BlynkTimer timer;           // timer which keeps track of how many milliseconds have elapsed since boot-up

void sendSensor()             // to read the temperature from the DS18B20 sensor and send the data to the Blynk app
{
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
    Blynk.virtualWrite(V0, tempC);
    Blynk.virtualWrite(V1, tempF);
    Blynk.virtualWrite(V2, tdsValue);
    Blynk.virtualWrite(V3, turbidity);
}

void readTemperature() {
  // Request temperature to all devices on the data line
  sensor.requestTemperatures(); 
  
  Serial.print("Celsius temperature: ");
  //We can connect more than one IC on the same data wire. 0 refers to the first IC on the wire
  Serial.print(sensor.getTempCByIndex(0));  //returns the temperature in Celsius
  Serial.print(" - Fahrenheit temperature: ");
  Serial.println(sensor.getTempFByIndex(0));
  tempC=sensor.getTempCByIndex(0);
  tempF=sensor.getTempFByIndex(0);

}

void setup()
{   
  
   Serial.begin(115200);          //how fast the data is to be sent
  sensor.begin();

  pinMode(TdsSensorPin,INPUT);

  Blynk.begin(auth, ssid, pass);
  timer.setInterval(100L, sendSensor);          //to set a delay for functions that are executed repeatedly.
  
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.display();
  delay(2000);
  display.clearDisplay();

}

void readTurbidity() {
  int sensorValue = analogRead(sensorPin);
  turbidity = map(sensorValue, 0, 1923, 100, 0); // Map the sensor value to turbidity range
  delay(100);
  
  Serial.print("Turbidity: ");                  
  Serial.println(turbidity);
}

void readTDS() {
  static unsigned long analogSampleTimepoint = millis();
   if(millis()-analogSampleTimepoint > 40U)     //every 40 milliseconds,read the analog value from the ADC
   {
     analogSampleTimepoint = millis();
     analogBuffer[analogBufferIndex] = analogRead(TdsSensorPin);    //read the analog value and store into the buffer
     analogBufferIndex++;
     if(analogBufferIndex == SCOUNT) 
         analogBufferIndex = 0;
   }   
   static unsigned long printTimepoint = millis();
   if(millis()-printTimepoint > 800U)
   {
      printTimepoint = millis();
      for(copyIndex=0;copyIndex<SCOUNT;copyIndex++)
        analogBufferTemp[copyIndex]= analogBuffer[copyIndex];
      averageVoltage = getMedianNum(analogBufferTemp,SCOUNT) * (float)VREF / 4096.0; // read the analog value more stable by the median filtering algorithm, and convert to voltage value
      float compensationCoefficient=1.0+0.02*(temperature-25.0);    //temperature compensation formula: fFinalResult(25^C) = fFinalResult(current)/(1.0+0.02*(fTP-25.0));
      float compensationVolatge=averageVoltage/compensationCoefficient;  //temperature compensation
      tdsValue=(133.42*compensationVolatge*compensationVolatge*compensationVolatge - 255.86*compensationVolatge*compensationVolatge + 857.39*compensationVolatge)*0.5; //convert voltage value to tds value

      Serial.print("TDS Value:");
      Serial.print(tdsValue,0);
      Serial.println("ppm");
   }
      
}

int getMedianNum(int bArray[], int iFilterLen) 
{
      int bTab[iFilterLen];
      for (byte i = 0; i<iFilterLen; i++)
      bTab[i] = bArray[i];
      int i, j, bTemp;
      for (j = 0; j < iFilterLen - 1; j++) 
      {
      for (i = 0; i < iFilterLen - j - 1; i++) 
          {
        if (bTab[i] > bTab[i + 1]) 
            {
        bTemp = bTab[i];
            bTab[i] = bTab[i + 1];
        bTab[i + 1] = bTemp;
         }
      }
      }
      if ((iFilterLen & 1) > 0)
    bTemp = bTab[(iFilterLen - 1) / 2];
      else
    bTemp = (bTab[iFilterLen / 2] + bTab[iFilterLen / 2 - 1]) / 2;
      return bTemp;
}

void displaySensors() {
  display.clearDisplay();
  display.setTextSize(1.5);
  display.setTextColor(SSD1306_WHITE);

  //display temperature
  display.setCursor(0,0);
  display.print("Temperature: ");
  display.print(tempC);
  display.println(" ^C");

  // display turbidity
  display.setCursor(0, 10);
  display.print("Turbidity: ");
  display.println(turbidity);
  if (turbidity < 20) {
    display.setCursor(0, 20);
    display.print("Water is clear");
  } else if (turbidity < 50) {
    display.setCursor(0, 20);
    display.print("Water is cloudy");
  } else {
    display.setCursor(0, 20);
    display.print("Water is dirty");
  }
  // display tds
  display.setCursor(0,30);
  display.print("TDS: ");
  display.print(tdsValue);
  display.println(" ppm");

  //

  display.display();
}


void loop()
{
  Blynk.run();
  timer.run();

  readTurbidity();
  delay(100);
  readTDS();
  delay(100);
  readTemperature();
  delay(100);

  displaySensors();
  delay(200);
}