#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "GravityTDS.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define TdsSensorPin 36  // Analog pin connected to the TDS sensor
#define ONE_WIRE_BUS 4  // Pin connected to the DS18B20 temperature sensor

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
GravityTDS gravityTds;

float tdsValue = 0;

void setup() {
  Serial.begin(115200);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.display();
  delay(2000);
  display.clearDisplay();
  sensors.begin();
  gravityTds.setPin(TdsSensorPin);
  gravityTds.setAref(3.3);  // Reference voltage on ADC, default 5.0V on ESP32
  gravityTds.setAdcRange(4096);  // 4096 for 12-bit ADC on ESP32
  gravityTds.begin();  // Initialization
}

void loop() {
  sensors.requestTemperatures();
  gravityTds.setTemperature(sensors.getTempCByIndex(0));  // Set the temperature and execute temperature compensation
  gravityTds.update();  // Sample and calculate
  tdsValue = gravityTds.getTdsValue();  // Get the TDS value

  Serial.print(tdsValue, 0);
  Serial.print("ppm-----------");
  Serial.print("Temperature is: ");
  Serial.println(sensors.getTempCByIndex(0));

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print("TDS: ");
  display.print(tdsValue, 0);
  display.print(" PPM");

  display.setCursor(0, 10);
  display.print("Temp: ");
  display.print(sensors.getTempCByIndex(0));
  display.print(" C");

  display.display();
  delay(1500);
}