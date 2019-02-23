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
int gyro_data_x, gyro_data_y;
///////////////////////////
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"

RF24 radio(9, 10);

const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };
int message[2];

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
  radio.openWritingPipe(pipes[0]);
  radio.startListening();
}

int get_gyro_data_x() {
  ACC_GYR.getMotion6(&accx, &accy, &accz, &gyrx, &gyry , &gyrz);
  gyro_data_x = map(accx, -17000, 17000, 0, 180);
  return gyro_data_x;
}

int get_gyro_data_y() {
  ACC_GYR.getMotion6(&accx, &accy, &accz, &gyrx, &gyry , &gyrz);
  gyro_data_y = map(accy, -17000, 17000, 0, 180);
  return gyro_data_y;
}


void loop(void)
{
  radio.stopListening();

  message[0] = get_gyro_data_x();
  message[1] = get_gyro_data_y();
  printf("Now sending %d %d...\n", message[0], message[1]);
  bool ok = radio.write( message, sizeof(message));

  if (ok)
    printf("ok...");
  else
    printf("failed.\n\r");

  // Now, continue listening
  radio.startListening();


  delay(50);

}
