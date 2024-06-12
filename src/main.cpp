#include <Arduino.h>
#include "Display.h"
#include "DHT11.h"

// Defineer de pinnen die gebruikt worden voor de temperatuursensor, de tweede knop en de buzzer
const int TEMP_PIN = A1; // Pin voor de temperatuursensor
const int BUTTON_PIN_TWO = 9; // Pin voor de tweede knop
const int PIN_BUZZER = 3; // Pin voor de buzzer

// Definieer de frequenties van tonen die de buzzer kan spelen
const int c4 = 261; // Toon C4
const int a4 = 440; // Toon A4
const int h4 = 494; // Toon H4

// Variabele om de vorige staat van de tweede knop bij te houden
byte prevTwoState = HIGH;

// Array voor het loggen van temperatuurwaarden
float tempLogs[100] = {};
int tempIndex = 0; // Index voor de huidige positie in de tempLogs array
int temperatures[2] = {20, 25}; // Temperatuur drempelwaarden

// Initialiseer de seriële communicatie en de pinnen voor de knoppen
void setup() {
  Serial.begin(9600); // Start de seriële communicatie met een baudrate van 9600
  pinMode(BUTTON_PIN_TWO, INPUT_PULLUP); // Stel de tweede knop in als input met pull-up weerstand
}

// Functie om de huidige temperatuurwaarde te krijgen van de sensor
float getTempValue() {
  return DHT11.getTemperature(); // Haal de temperatuur op van de DHT11 sensor
}

// Functie om de huidige temperatuurwaarde te loggen
void logValue() {
  float value = getTempValue(); // Verkrijg de huidige temperatuur
  tempLogs[tempIndex] = value; // Sla de temperatuur op in de array
  tempIndex++; // Verhoog de index
  if (tempIndex > 100) { // Als de index groter is dan 100, reset de index naar 0
    tempIndex = 0;
  }
}

// Functie om de gemiddelde temperatuurwaarde te berekenen
float getAverageValue() {
  float total = 0;
  for (int i = 0; i < tempIndex; i++) {
    total += tempLogs[i]; // Tel alle temperatuurwaarden bij elkaar op
  }
  return total / tempIndex; // Bereken het gemiddelde
}

// Functie om de temperatuur drempelwaarden bij te werken via de seriële monitor
void updateTemperatures() {
    Serial.println("Enter first temperature:"); // Vraag om de eerste temperatuur
    while (Serial.available() == 0) {} // Wacht tot er input is via de seriële monitor
    temperatures[0] = Serial.readStringUntil('\n').toInt(); // Lees en stel de eerste temperatuur in

    Serial.println("Enter second temperature:"); // Vraag om de tweede temperatuur
    while (Serial.available() == 0) {} 
    temperatures[1] = Serial.readStringUntil('\n').toInt(); // Lees en stel de tweede temperatuur in
    
    Serial.print("Updated temperatures: "); // Print de bijgewerkte temperaturen
    Serial.print(temperatures[0]);
    Serial.print(", ");
    Serial.println(temperatures[1]);
}

// Functie om seriële input te controleren en te verwerken
void checkSerial() {
    if (Serial.available() > 0) {
        String input = Serial.readStringUntil('\n'); // Lees de input van de seriële monitor
        input.trim(); // Verwijder eventuele spaties of nieuwe regels aan het begin en einde
        if (input.equals("temp")) { // Als de input "temp" is, werk de temperaturen bij
            updateTemperatures();
        }
    }
}

// Functie om een temperatuurwaarschuwing te geven via de LED's en de buzzer
void temperatureAlert(float celcius) {
    if ((celcius > temperatures[1])) { // Als de temperatuur boven de tweede drempel ligt
        digitalWrite(PIN_LED_RED, HIGH); // Zet de rode LED aan
        tone(PIN_BUZZER, c4); // Speel een toon met de buzzer
        delay(500); // Wacht 500 milliseconden
        digitalWrite(PIN_LED_RED, LOW); // Zet de rode LED uit
        delay(500); // Wacht 500 milliseconden
    }
    else if ((celcius > temperatures[0]) && (celcius <= temperatures[1])) { // Als de temperatuur tussen de drempels ligt
        digitalWrite(PIN_LED_BLUE, HIGH); // Zet de blauwe LED aan
        tone(PIN_BUZZER, a4); // Speel een toon met de buzzer
        delay(500); // Wacht 500 milliseconden
        digitalWrite(PIN_LED_BLUE, LOW); // Zet de blauwe LED uit
        delay(500); // Wacht 500 milliseconden
    }
    else if ((celcius <= temperatures[0])) { // Als de temperatuur onder de eerste drempel ligt
        digitalWrite(PIN_LED_GREEN, HIGH); // Zet de groene LED aan
        tone(PIN_BUZZER, h4); // Speel een toon met de buzzer
        delay(500); // Wacht 500 milliseconden
        digitalWrite(PIN_LED_GREEN, LOW); // Zet de groene LED uit
        delay(500); // Wacht 500 milliseconden
    }
}

// De hoofdlus van het programma
void loop() {
  float celcius = getTempValue(); // Verkrijg de huidige temperatuurwaarde
  checkSerial(); // Controleer op seriële input
  logValue(); // Log de huidige temperatuur

  byte buttonTwoState = digitalRead(BUTTON_PIN_TWO); // Lees de staat van de tweede knop

  if (buttonTwoState == LOW && buttonTwoState != prevTwoState) { // Als de tweede knop ingedrukt is en de staat is veranderd
    float average = getAverageValue(); // Bereken de gemiddelde temperatuur
    Serial.println("Average:"); // Print "Average:" naar de seriële monitor
    Serial.println(average); // Print de gemiddelde temperatuur naar de seriële monitor
    delay(100); // Wacht 100 milliseconden om "bouncing" te voorkomen
  }

  prevTwoState = buttonTwoState; // Update de vorige staat van de tweede knop
  Display.show(getTempValue()); // Toon de huidige temperatuur op het display
  temperatureAlert(celcius); // Controleer of de temperatuur een waarschuwing moet activeren
}
