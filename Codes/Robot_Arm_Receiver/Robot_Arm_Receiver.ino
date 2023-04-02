///////////////////////////
/*
   Project: Remote Controlled Robot Arm
   Date: 03.04.2019
   Version: v1.1
*/
///////////////////////////
///////////////////////////
//Smoothing Variables
const int numReadings = 15;

int readings[numReadings];      // the readings from the analog input
int readIndex = 0;              // the index of the current reading
int total = 0;                  // the running total
int average = 0;                // the average

int readings2[numReadings];      // the readings from the analog input
int readIndex2 = 0;              // the index of the current reading
int total2 = 0;                  // the running total
int average2 = 0;                // the average
///////////////////////////
#include <Servo.h>
Servo m1, m2, m3;
int pos1 = 90;
int pos2 = 90;
///////////////////////////
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"

RF24 radio(9, 10);

// Radio pipe addresses for the 2 nodes to communicate.
const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };

int message[2];
///////////////////////////

void setup(void)
{
  Serial.begin(57600);

  m1.attach(2);
  m2.attach(3);
  m2.attach(4);

  printf_begin();
  radio.begin();
  radio.setAutoAck(false);
  // optionally, increase the delay between retries & # of retries
  radio.setRetries(15, 15);
  radio.openReadingPipe(1, pipes[0]);
  radio.startListening();
  m1.write(60);
  m2.write(60);
  m3.write(60);

  for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    readings[thisReading] = 0;
    readings2[thisReading] = 0;
  }
}

int smooth_x(int data) {
  // subtract the last reading:
  total = total - readings[readIndex];
  // read from the sensor:
  readings[readIndex] = data;
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
  average = total / numReadings;
  return average;
}

int smooth_y(int data) {
  // subtract the last reading:
  total2 = total2 - readings2[readIndex2];
  // read from the sensor:
  readings2[readIndex2] = data;
  // add the reading to the total:
  total2 = total2 + readings2[readIndex2];
  // advance to the next position in the array:
  readIndex2 = readIndex2 + 1;

  // if we're at the end of the array...
  if (readIndex2 >= numReadings) {
    // ...wrap around to the beginning:
    readIndex2 = 0;
  }

  // calculate the average:
  average2 = total2 / numReadings;
  return average2;
}

void recieve() {
  // if there is data ready
  if ( radio.available() )
  {

    bool done = false;
    while (!done)
    {
      done = radio.read( message, sizeof(message) );
      delay(20);
    }

    printf("mesaj 1 : %d mesaj2 : %d \n", message[0], message[1]);

    message[0] = smooth_x(message[0]);
    message[1] = smooth_y(message[1]);

    printf("gelen smooth deger x %d \n", message[0]);
    printf("gelen smooth deger y %d \n", message[1]);



    for (; pos1 <= message[0]; pos1++) {
      m1.write(pos1);
      delay(10);
    }
    for (; pos1 >= message[0]; pos1--) {
      m1.write(pos1);
      delay(10);
    }


    for (; pos2 <= message[1]; pos2++) {
      m2.write(pos2);
      m3.write(pos2);
      delay(10);
    }
    for (; pos2 >= message[1]; pos2--) {
      m2.write(pos2);
      m3.write(pos2);
      delay(10);
    }

    pos1 = message[0];
    pos2 = message[1];

  }
}

void loop(void)
{
  recieve();
  /*
    for (; pos1 <= message[0]; pos1++) {
    m1.write(pos1);
    delay(10);
    }
    for (; pos1 >= message[0]; pos1--) {
    m1.write(pos1);
    delay(10);
    }


    for (; pos2 <= message[1]; pos2++) {
    m2.write(pos2);
    m3.write(pos2);
    delay(10);
    }
    for (; pos2 >= message[1]; pos2--) {
    m2.write(pos2);
    m3.write(pos2);
    delay(10);
    }

    pos1 = message[0];
    pos2 = message[1];
  */

}
// vim:cin:ai:sts=2 sw=2 ft=cpp
