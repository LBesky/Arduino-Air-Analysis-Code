// SPDX-FileCopyrightText: 2011 Limor Fried/ladyada for Adafruit Industries
//
// SPDX-License-Identifier: MIT

// thermistor-1.ino Simple test program for a thermistor for Adafruit Learning System
// https://learn.adafruit.com/thermistor/using-a-thermistor by Limor Fried, Adafruit Industries
// MIT License - please keep attribution and consider buying parts from Adafruit

// the value of the 'other' resistor
#define SERIESRESISTOR 5000    
 
// What pin to connect the sensor to
// which analog pin to connect
#define THERMISTORPIN A2         
// resistance at 25 degrees C
#define THERMISTORNOMINAL 5000      
// temp. for nominal resistance (almost always 25 C)
#define TEMPERATURENOMINAL 25   
// The beta coefficient of the thermistor (usually 3000-4000)
#define BCOEFFICIENT 3935
// the value of the 'other' resistor
#define SERIESRESISTOR 5000 

float reading;

#include <SPI.h>
#include "Adafruit_Si7021.h"

int timer = 0;

int analogPin = A0;
float vout;
float temp;

Adafruit_Si7021 sensor = Adafruit_Si7021();

const int numReadings = 10;

int readings[numReadings];      // the readings from the analog input
int readIndex = 0;              // the index of the current reading
float total = 0;                  // the running total
float average = 0;                // the average

void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600); // was 115200, will try 9600 now.
  analogReference(DEFAULT);

  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
    delay(10);
  }

  //Serial.println("Si7021 test!");

  if (!sensor.begin()) {
    Serial.println("Did not find Si7021 sensor!");
    while (true)
      ;
  }

  /* Serial.print("Found model ");
  switch(sensor.getModel()) {
    case SI_Engineering_Samples:
      Serial.print("SI engineering samples"); break;
    case SI_7013:
      Serial.print("Si7013"); break;
    case SI_7020:
      Serial.print("Si7020"); break;
    case SI_7021:
      Serial.print("Si7021"); break;
    case SI_UNKNOWN:
    default:
      Serial.print("Unknown");
  
  }
  /*Serial.print(" Rev(");
  //Serial.print(sensor.getRevision());
  //Serial.print(")");
  Serial.print(" Serial #"); Serial.print(sensor.sernum_a, HEX); Serial.println(sensor.sernum_b, HEX);
  */
  for (int thisReading = 0; thisReading < numReadings; thisReading++) 
  {
    readings[thisReading] = 0;
  }
}


void loop() {
  // put your main code here, to run repeatedly:

  int a = analogRead(analogPin);
  
  vout = (a/1024.0)*5000;
  temp = (vout - 500)/10; // converts raw voltage to temperature
  
  //Serial.println(temp);

  // subtract the last reading:
  total = total - readings[readIndex];
  // read from the sensor:
  readings[readIndex] = temp;
  // add the reading to the total:
  total = total + readings[readIndex];
  // advance to the next position in the array:
  readIndex = readIndex + 1;

  // if we're at the end of the array...
  if (readIndex >= numReadings) {
    // ...wrap around to the beginning:
    readIndex = 0;
  }

  // calculate the average:
  float old_average = average;
  average = total * 1.0 / numReadings;
  // send it to the computer as ASCII digits
  /*
  Serial.print("TMP-36 Temp:  ");
  Serial.println(average);
  delay(1000);

  Serial.print("Humidity:    ");
  Serial.print(sensor.readHumidity(), 2);
  Serial.print("\tTemperature: ");
  Serial.println(sensor.readTemperature(), 2);
  delay(1000);
  */
  reading = analogRead(THERMISTORPIN);
 
  //DEBUG CODE// Serial.print("Analog reading "); 
  //DEBUG CODE// Serial.println(reading);
 
  // convert the value to resistance
  reading = (1023 / reading)  - 1;     // (1023/ADC - 1) 
  reading = SERIESRESISTOR / reading;  // 10K / (1023/ADC - 1)
  //DEBUG CODE// Serial.print("Thermistor resistance "); 
  //DEBUG CODE// Serial.println(reading);

  float steinhart;
  steinhart = reading / THERMISTORNOMINAL;     // (R/Ro)
  steinhart = log(steinhart);                  // ln(R/Ro)
  steinhart /= BCOEFFICIENT;                   // 1/B * ln(R/Ro)
  steinhart += 1.0 / (TEMPERATURENOMINAL + 273.15); // + (1/To)
  steinhart = 1.0 / steinhart;                 // Invert
  steinhart -= 273.15;                         // convert absolute temp to C

  /*
  Serial.print("Thermistor Temperature "); 
  Serial.println(steinhart);
  delay(4000);
  */

  timer = timer + 1;

  Serial.print(timer); // Current Time reading in Seconds
  Serial.print("\t");
  Serial.print(average); //TMP-36 Temperature Reading
  Serial.print("\t");
  Serial.print(sensor.readHumidity(), 2); //SI7021 Humidity Reading
  Serial.print("\t");
  Serial.print(sensor.readTemperature(), 2); // SI7021 Temperature Reading
  Serial.print("\t");
  Serial.println(steinhart); // Thermistor Temperature Reading.
  delay(1000);
}
