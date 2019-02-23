#include <Servo.h>
Servo m1, m2, m3;

#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"

RF24 radio(9, 10);

// Radio pipe addresses for the 2 nodes to communicate.
const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };

int message[2];
int pos1 = 90;
int pos2 = 90;
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
  m1.write(90);
  m2.write(90);
  m3.write(90);
}

void loop(void)
{

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
// vim:cin:ai:sts=2 sw=2 ft=cpp
