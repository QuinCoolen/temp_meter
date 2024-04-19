#include <Arduino.h>
#include "Display.h"
#include "DHT11.h"

const int TEMP_PIN = A1;
const int BUTTON_PIN_ONE = 8;
const int BUTTON_PIN_TWO = 9;
byte prevOneState = HIGH;
byte prevTwoState = HIGH;

int currentMode = 0;
float tempLogs[100] = {}; // Assuming maximum 100 entries for simplicity
int tempIndex = 0;

float humiLogs[100] = {}; // Assuming maximum 100 entries for simplicity
int humiIndex = 0;

void setup() {
  Serial.begin(9600);

  pinMode(BUTTON_PIN_ONE, INPUT_PULLUP);
  pinMode(BUTTON_PIN_TWO, INPUT_PULLUP);
}

void toggleMode() {
  if (currentMode == 1) { 
    currentMode = 0;
    Serial.println("Nu meet die de temperatuur");
  } else {
    Serial.println("Nu meet die de lucht vochtigheid");
    currentMode = 1;
  }
}

float getValue() {
  return currentMode == 0 ? DHT11.getTemperature() : DHT11.getHumidity();
}

void logValue() {
  float value = getValue();
  if (currentMode == 0) {
    tempLogs[tempIndex] = value;
    tempIndex++;
    if (tempIndex > 100) {
      tempIndex = 0;
    }
  } else {
    humiLogs[humiIndex] = value;
    humiIndex++;
     if (humiIndex > 100) {
      humiIndex = 0;
    }
  }
}

float getAverageValue() {
  if (currentMode == 0) {
    float total = 0;
    for (int i = 0; i < tempIndex; i++) {
      total += tempLogs[i];
    }
    return total / tempIndex;
  } else {
    float total = 0;
    for (int i = 0; i < humiIndex; i++) {
      total += humiLogs[i];
    }
    return total / humiIndex;
  }
}

void loop() {
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
