#include <Arduino.h>
#include "Display.h"
#include "DHT11.h"

const int TEMP_PIN = A1;
const int BUTTON_PIN_ONE = 8;
const int BUTTON_PIN_TWO = 9;
const int PIN_BUZZER = 3;

const int c4 = 261;
const int a4 = 440;
const int h4 = 494;

byte prevOneState = HIGH;
byte prevTwoState = HIGH;

float tempLogs[100] = {};
int tempIndex = 0;
int temperatures[2] = {20, 25};

void setup() {
  Serial.begin(9600);

  pinMode(BUTTON_PIN_ONE, INPUT_PULLUP);
  pinMode(BUTTON_PIN_TWO, INPUT_PULLUP);
}

float getValue() {
  return DHT11.getTemperature();
}

void logValue() {
  float value = getValue();
  tempLogs[tempIndex] = value;
  tempIndex++;
  if (tempIndex > 100) {
    tempIndex = 0;
  }
}

float getAverageValue() {
  float total = 0;
  for (int i = 0; i < tempIndex; i++) {
    total += tempLogs[i];
  }
  return total / tempIndex;
}

void updateTemperatures() {
    Serial.println("Enter first temperature:");
    while (Serial.available() == 0) {}
    temperatures[0] = Serial.readStringUntil('\n').toInt();

    Serial.println("Enter second temperature:");
    while (Serial.available() == 0) {} 
    temperatures[1] = Serial.readStringUntil('\n').toInt();
    
    Serial.print("Updated temperatures: ");
    Serial.print(temperatures[0]);
    Serial.print(", ");
    Serial.println(temperatures[1]);
}

void checkSerial() {
    if (Serial.available() > 0) {
        String input = Serial.readStringUntil('\n');
        input.trim();
        if (input.equals("temp")) {
            updateTemperatures();
        }
    }
}

void temperatureAlert(float celcius) {
    if ((celcius > temperatures[1]))
    {
        digitalWrite(PIN_LED_RED, HIGH);
        tone(PIN_BUZZER, c4);
        delay(500);
        digitalWrite(PIN_LED_RED, LOW);
        delay(500);
    }
    else if ((celcius > temperatures[0]) && (celcius <= temperatures[1]))
    {
        digitalWrite(PIN_LED_BLUE, HIGH);
        tone(PIN_BUZZER, a4);
        delay(500);
        digitalWrite(PIN_LED_BLUE, LOW);
        delay(500);
    }
    else if ((celcius <= temperatures[0]))
    {
        digitalWrite(PIN_LED_GREEN, HIGH);
        tone(PIN_BUZZER, h4);
        delay(500);
        digitalWrite(PIN_LED_GREEN, LOW);
        delay(500);
    }
}

void loop() {
  checkSerial();
  logValue();
  byte buttonOneState = digitalRead(BUTTON_PIN_ONE);
  byte buttonTwoState = digitalRead(BUTTON_PIN_TWO);
  if (buttonOneState == LOW && buttonOneState != prevOneState) {
    toggleMode();
    delay(100);
  }

  if (buttonTwoState == LOW && buttonTwoState != prevTwoState) {
    float average = getAverageValue();
    Serial.println("Avarage:");
    Serial.println(average);
    delay(100);
  }
  prevOneState = buttonOneState;
  prevTwoState = buttonTwoState;

  Display.show(getValue());
}
