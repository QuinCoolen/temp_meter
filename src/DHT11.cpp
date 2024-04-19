
/*
 * Adapted from Rich Shield Example Code
 * Name:
 * Date: 07/06/2019
 * Author: Jaap Geurts <jaap.geurts@fontys.nl>
 * Version: 1.0
 */

/*
 * Based on code from
 * 
 *   FILE: dht.cpp
 * AUTHOR: Rob Tillaart
 * VERSION: 0.1.25
 * PURPOSE: DHT Temperature & Humidity Sensor library for Arduino
 *    URL: http://arduino.cc/playground/Main/DHTLib
 */

#include "DHT11.h"

DHT11Hardware DHT11;

// how many timing transitions we need to keep track of. 2 * number bits + extra
const int MAXTIMINGS = 85;

DHT11Hardware::DHT11Hardware(uint8_t pin, uint8_t count)
{
    dataPin = pin;
    _count = count;
    firstreading = true;

    pinMode(dataPin, INPUT);
    digitalWrite(dataPin, HIGH);
    _lastreadtime = 0;
}

float DHT11Hardware::getTemperature()
{
    float f;

    if (read()) {
        f = data[2];
        return f;
    }
    return NAN;
}

float DHT11Hardware::getHumidity()
{
    float f;
    if (read()) {
        f = data[0];
        return f;
    }
    return NAN;
}

bool DHT11Hardware::read()
{
    uint8_t laststate = HIGH;
    uint8_t counter = 0;
    uint8_t j = 0, i;
    unsigned long currenttime;

    // pull the pin high and wait 250 milliseconds
    digitalWrite(dataPin, HIGH);
    delay(250);

    currenttime = millis();
    if (currenttime < _lastreadtime) {
        // ie there was a rollover
        _lastreadtime = 0;
    }
    if (!firstreading && ((currenttime - _lastreadtime) < 2000)) {
        return true; // return last correct measurement
        //delay(2000 - (currenttime - _lastreadtime));
    }
    firstreading = false;
    _lastreadtime = millis();

    data[0] = data[1] = data[2] = data[3] = data[4] = 0;

    // now pull it low for ~20 milliseconds
    pinMode(dataPin, OUTPUT);
    digitalWrite(dataPin, LOW);
    delay(20);
    cli(); // make sure no interrupts happen
    digitalWrite(dataPin, HIGH);
    delayMicroseconds(40);
    pinMode(dataPin, INPUT);

    // read in timings
    for ( i=0; i< MAXTIMINGS; i++) {
        counter = 0;
        while (digitalRead(dataPin) == laststate) {
            counter++;
            delayMicroseconds(1);
            if (counter == 255) {
                break;
            }
        }
        laststate = digitalRead(dataPin);

        if (counter == 255) break;

        // ignore first 3 transitions
        if ((i >= 4) && (i%2 == 0)) {
            // shove each bit into the storage bytes
            data[j/8] <<= 1;
            if (counter > _count)
                data[j/8] |= 1;
            j++;
        }

    }

    sei(); // re-enable interrupts


    // check we read 40 bits and that the checksum matches
    if ((j >= 40) &&
            (data[4] == ((data[0] + data[1] + data[2] + data[3]) & 0xFF)) ) {
        return true;
    }

    return false;
}
