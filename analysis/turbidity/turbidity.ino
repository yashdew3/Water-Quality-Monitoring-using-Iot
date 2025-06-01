#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

int sensorPin = 34; // Analog pin connected to the turbidity sensor
int turbidityValue;

void setup() {
  Serial.begin(9600);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.display();
  delay(2000);
  display.clearDisplay();
}

void loop() {
  int sensorValue = analogRead(sensorPin);
  int turbidity = map(sensorValue, 0, 1783, 100, 0); // Map the sensor value to turbidity range
  delay(100);
  display.clearDisplay();
  display.setTextSize(1.5);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print("Turbidity: ");
  display.println(turbidity);
  if (turbidity < 20) {
    display.setCursor(0, 10);
    display.print("Water is clear");
  } else if (turbidity < 50) {
    display.setCursor(0, 10);
    display.print("Water is cloudy");
  } else {
    display.setCursor(0, 10);
    display.print("Water is dirty");
  }
  display.display();
  Serial.print("Turbidity: ");                  
  Serial.println(turbidity);

  delay(1000);
}