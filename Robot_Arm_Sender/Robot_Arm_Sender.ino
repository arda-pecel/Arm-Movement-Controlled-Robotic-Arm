///////////////////////////
/*
   Author: Emre Şahin
   Project: Remote Controlled Robot Arm
   Date: 22.01.2019
   Version: v1.0
*/
///////////////////////////
#include   <MPU6050.h>
#include   <Wire.h>
///////////////////////////
MPU6050 ACC_GYR;
int16_t accx, accy, accz;
int16_t gyrx, gyry, gyrz;
int gyro_data;
///////////////////////////
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"

RF24 radio(9, 10);

const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };

// The various roles supported by this sketch
typedef enum { role_ping_out = 1, role_pong_back } role_e;

// The debug-friendly names of those roles
const char* role_friendly_name[] = { "invalid", "Ping out", "Pong back"};

// The role of the current running sketch
role_e role = role_ping_out;
int message[1];

void setup(void)
{
  Serial.begin(57600);
  
  ACC_GYR.initialize();
  Serial.println(ACC_GYR.testConnection() ? "Gyroscope On" : "Gyroscope Off");
  
  printf_begin();
  radio.begin();
  radio.setAutoAck(false);
  // optionally, increase the delay between retries & # of retries
  radio.setRetries(15, 15);

  // This simple sketch opens two pipes for these two nodes to communicate
  // back and forth.
  // Open 'our' pipe for writing
  // Open the 'other' pipe for reading, in position #1 (we can have up to 5 pipes open for reading)

  //if ( role == role_ping_out )
  {
    radio.openWritingPipe(pipes[0]);
    //radio.openReadingPipe(1, pipes[1]);
  }
  //else
  {
    //radio.openWritingPipe(pipes[1]);
    //radio.openReadingPipe(1,pipes[0]);
  }
  radio.startListening();
}

int get_gyro_data() {
  ACC_GYR.getMotion6(&accx, &accy, &accz, &gyrx, &gyry , &gyrz);
  gyro_data = map(accx, -17000, 17000, 0, 180);
  Serial.println(gyro_data);
  return gyro_data;
}


void loop(void)
{
  radio.stopListening();
  
  message[0] = get_gyro_data();
  printf("Now sending %d...", message[0]);
  bool ok = radio.write( message, 1);

  if (ok)
    printf("ok...");
  else
    printf("failed.\n\r");

  // Now, continue listening
  radio.startListening();


  delay(50);

}
// vim:cin:ai:sts=2 sw=2 ft=cpp
