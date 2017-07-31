/*
  lidar_altimeter

  Monitors altitude using a LIDAR sensor attached via I2C bus and provides
  callouts when certain altitudes are reached.

   Parts based upon "speaker_pcm" (https://playground.arduino.cc/Code/PCMAudio):
   Plays 8-bit PCM audio on pin 11 using pulse-width modulation (PWM).
   For Arduino with Atmega168 at 16 MHz.
   By Michael Smith <michael@hurts.ca>

*/
#include <Wire.h>
#include <LIDARLite.h>

#include <stdint.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/pgmspace.h>

#include <TimerOne.h>

#include "1.h"
#include "2.h"
#include "3.h"
#include "4.h"
#include "5.h"
#include "10.h"
#include "20.h"
#include "30.h"
#include "40.h"
#include "50.h"
#include "100.h"

LIDARLite myLidarLite;
/*
  Selects one of several preset configurations.

  Parameters
  ----------------------------------------------------------------------------
  configuration:  Default 0.
  0: Default mode, balanced performance.
  1: Short range, high speed. Uses 0x1d maximum acquisition count.
  2: Default range, higher speed short range. Turns on quick termination
      detection for faster measurements at short range (with decreased
      accuracy)
  3: Maximum range. Uses 0xff maximum acquisition count.
  4: High sensitivity detection. Overrides default valid measurement detection
      algorithm, and uses a threshold value for high sensitivity and noise.
  5: Low sensitivity detection. Overrides default valid measurement detection
      algorithm, and uses a threshold value for low sensitivity and noise.
  lidarliteAddress: Default 0x62. Fill in new address here if changed. See
  operating manual for instructions.
*/
#define LIDAR_CONFIG 3

/* Sets, how many readings will be taken each burst */
#define MAX_MEASURES 15

volatile uint16_t audio_count = 0; // Current offset into sample
uint16_t audio_len = 0; // Maximum offset until which we want to play back
uint8_t *audio_data = NULL; // Pointer which to play from, assumed to be in FLASH
unsigned int altitude; // Current altitude in centimeter
byte announced_feet = 0; // Last announced altitude, used when determining if a callout is needed

/* Feeds audio data from the pointers into the PWM module */
void audio_callback() {
  audio_count = (audio_count + 1);
  if (audio_count >= audio_len) {
    /* When nothing left to play, return to PWM level of 128,
       which is the idle level of unsigned eight bit audio.
       Reset the counters, too.
    */
    audio_count = 0;
    audio_len = 0;
    if (OCR2B > 128)
      OCR2B -= 1;
    if (OCR2B < 128)
      OCR2B += 1;
  } else
    OCR2B = pgm_read_byte(&audio_data[audio_count]);
}

/** Tries to say a number.
   If number is not available, just skip it.
*/
void say(byte number) {

  switch (number) {
    case 1:
      audio_data = __1_u8;
      audio_len = __1_u8_len;
      break;

    case 2:
      audio_data = __2_u8;
      audio_len = __2_u8_len;
      break;

    case 3:
      audio_data = __3_u8;
      audio_len = __3_u8_len;
      break;

    case 4:
      audio_data = __4_u8;
      audio_len = __4_u8_len;
      break;

    case 5:
      audio_data = __5_u8;
      audio_len = __5_u8_len;
      break;

    case 10:
      audio_data = __10_u8;
      audio_len = __10_u8_len;
      break;

    case 20:
      audio_data = __20_u8;
      audio_len = __20_u8_len;
      break;

    case 30:
      audio_data = __30_u8;
      audio_len = __30_u8_len;
      break;

    case 40:
      audio_data = __40_u8;
      audio_len = __40_u8_len;
      break;

    case 50:
      audio_data = __50_u8;
      audio_len = __50_u8_len;
      break;
      /*
          case 100:
            audio_data = __100_u8;
            audio_len = __100_u8_len;
            break;
      */
  }
}

int cmp_func(const unsigned int *a, const unsigned int *b)
{
  signed long x = *a;
  x -= *b;
  if (x > 0)
    return (1);

  if (x < 0)
    return (-1);

  if (x == 0)
    return (0);
}

void loop() {

  static unsigned int distances[MAX_MEASURES];
  byte value_count = 0;
  byte count_max = 0;
  unsigned long until = millis() + 333;
  unsigned median;

  while (millis() < until) {
    // Sleep after getting MAX_MEASURES
    if (value_count == MAX_MEASURES)
      continue;

    // Otherwise (not enough points) try to get more:
    // first get distance in cm
    unsigned int distance = myLidarLite.distance();
    // Only work with plausible values over 10cm and below 50m
    if ( (distance > 10) && (distance < 5000) ) {
      value_count += 1;
      if (value_count > MAX_MEASURES)
        value_count = 0;
      if (count_max < value_count)
        count_max = value_count;
      distances[value_count] = distance;
    }
  }
  // Median
  qsort(distances, count_max, sizeof(distances[count_max]), cmp_func);
  median = distances[1];
  // Average
  unsigned long average = 0;
  for (byte i = 0; i < count_max; i++) {
    average += distances[i];
    Serial.print(distances[i]);
    Serial.print(" ");
  }
  average /= count_max;

  // If too little measurements where taken, do not consider this round
  if (count_max < ((MAX_MEASURES) / 3) )
    return;

  unsigned int distance = median;
  /* If out of range, it returns very low distances. Disregard, then */
  if (distance > 5)
  {
    altitude = (altitude + distance) >> 1;
    unsigned long int feet = altitude;
    feet *= 100L;
    feet /= 3048L;
    Serial.print(altitude);
    Serial.print(" ");
    Serial.print(distance);
    Serial.print(" ");
    Serial.println(feet);
    if ((feet != announced_feet) && (audio_len == 0))  {
      say(feet);
      announced_feet = feet;
    }
  }
}

void setup()
{
  // Init PWM for sound output
  pinMode(3, OUTPUT);

  // Set up Timer 2 to do pulse width modulation on the speaker
  // pin.

  // Use internal clock (datasheet p.160)
  ASSR &= ~(_BV(EXCLK) | _BV(AS2));

  // Set fast PWM mode  (p.157)
  TCCR2A |= _BV(WGM21) | _BV(WGM20);
  TCCR2B &= ~_BV(WGM22);

  // Do non-inverting PWM on pin OC2B (p.155)
  // On the Arduino this is pin 3.
  TCCR2A = (TCCR2A | _BV(COM2B1)) & ~_BV(COM2B0);
  TCCR2A &= ~(_BV(COM2A1) | _BV(COM2A0));
  // No prescaler (p.158)
  TCCR2B = (TCCR2B & ~(_BV(CS12) | _BV(CS11))) | _BV(CS10);

  // Set up sound sample feeding timer interrupt.

  audio_count = 0;

  Timer1.initialize(125 * 2);
  Timer1.attachInterrupt(audio_callback);

  pinMode(3, OUTPUT);
  Serial.begin(9600);
  myLidarLite.begin(LIDAR_CONFIG, true); // Set configuration and I2C to 400 kHz

  myLidarLite.configure(LIDAR_CONFIG);
}


