/*********************************************************************************************************
*
* File                : DustSensor
* Hardware Environment: 
* Build Environment   : Arduino
* Version             : V1.0.5-r2
* By                  : WaveShare
*
*                                  (c) Copyright 2005-2011, WaveShare
*                                       http://www.waveshare.net
*                                       http://www.waveshare.com   
*                                          All Rights Reserved
*
*********************************************************************************************************/

//#define INTERNAL_REF

#define COV_RATIO 0.2 //ug/mmm / mv
#define NO_DUST_VOLTAGE 400 //mv
#ifndef INTERNAL_REF
#define SYS_VOLTAGE 5000
#else
#define SYS_VOLTAGE 1100
#endif

//const double resolution = 1024.0;
//const double resolution = 65535.0;
const double resolution = 32768.0;

#include "Arduino.h"
#include <Adafruit_ADS1015.h>

#include <Wire.h>

/*
I/O define
*/
const int iled = 7; //drive the led of sensor
const int vout = A0; //analog input

/*
variable
*/
float density, voltage;
int adcvalue;

Adafruit_ADS1115 ads1015;

/*
private function
*/
int Filter(int m)
{
    static int flag_first = 0, _buff[10], sum;
    const int _buff_max = 10;
    int i;

    if (flag_first == 0) {
        flag_first = 1;

        for (i = 0, sum = 0; i < _buff_max; i++) {
            _buff[i] = m;
            sum += _buff[i];
        }
        return m;
    } else {
        sum -= _buff[0];
        for (i = 0; i < (_buff_max - 1); i++) {
            _buff[i] = _buff[i + 1];
        }
        _buff[9] = m;
        sum += _buff[9];

        i = sum / 10.0;
        return i;
    }
}

void setup(void)
{
    pinMode(iled, OUTPUT);
    pinMode(vout, INPUT);
    digitalWrite(iled, LOW); //iled default closed

#ifdef INTERNAL_REF
    analogReference(INTERNAL);
#endif

    Serial.begin(9600); //send and receive at 9600 baud
    Serial.print("*********************************** WaveShare ***********************************\n");

    ads1015.begin();
}

void loop(void)
{
    /*
  get adcvalue
  */
    digitalWrite(iled, HIGH);
    delayMicroseconds(200);

    int16_t adcvalue = ads1015.readADC_SingleEnded(0);
    //int adcvalue = analogRead(vout);
    
    digitalWrite(iled, LOW); 

    Serial.print("*** current readout is: ");
    Serial.print(adcvalue);
    Serial.print("\n");

    adcvalue = Filter(adcvalue);

    Serial.print("Filtered readout is: ");
    Serial.print(adcvalue);
    Serial.print("\n");

    /*
  covert voltage (mv)
  */
    voltage = (SYS_VOLTAGE / resolution) * adcvalue * 11;

    Serial.print("voltage is: ");
    Serial.print(voltage);
    Serial.print("\n");

    /*
  voltage to density
  */
    if (voltage >= NO_DUST_VOLTAGE) {
        voltage -= NO_DUST_VOLTAGE;

        density = voltage * COV_RATIO;
    } else
        density = 0;

    /*
  display the result
  */
    Serial.print("The current dust concentration is: ");
    Serial.print(density);
    Serial.print(" ug/m3\n");

    delay(1000);
}
