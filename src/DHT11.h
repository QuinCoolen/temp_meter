/*
 * Rich shield example
 * Adapted from Rich Shield Example Code
 * Name: DHT11
 * Date: 07/06/2019
 * Author: Jaap Geurts <jaap.geurts@fontys.nl>
 * Version: 1.0
 */

#ifndef _DHT11_H
#define _DHT11_H

#include <Arduino.h>

#include <inttypes.h>

/* DHT library

MIT license
written by Adafruit Industries
*/


class DHT11Hardware
{
public:

    DHT11Hardware(uint8_t pin=12, uint8_t count=6);

    float getTemperature();
    float getHumidity();

private:
    uint8_t data[6];
    uint8_t dataPin, _count;
    bool read();
    uint32_t _lastreadtime;
    bool firstreading;

};

extern DHT11Hardware DHT11;

#endif // DHT11_H
