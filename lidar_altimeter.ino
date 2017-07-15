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
//#include "100.h"
#include <Wire.h>
#include <LIDARLite.h>

#include "TimerOne.h"

LIDARLite myLidarLite;

uint16_t audio_count = 0;
uint16_t audio_len = 0;
uint8_t *audio_data = NULL;

void audio_callback() {
  audio_count = (audio_count + 1);
  if (audio_count >= audio_len) {
    audio_count = 0;
    audio_len = 0;
    if (OCR2B > 128)
      OCR2B -= 1;
    if (OCR2B > 128)
      OCR2B += 1;
  } else
    OCR2B = pgm_read_byte(&audio_data[audio_count]);
}

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
    
/*    case 100:
      audio_data = __100_u8;
      audio_len = __100_u8_len;
      break;
      */
  }
}

byte n = 0;;
unsigned int altitude;
byte announced_feet = 0;

void loop() {
  delay(250);
  altitude = (altitude + myLidarLite.distance()) >> 1;
  /* If out of range, it returns 0. Disregard, then */
  if (altitude == 0) 
    return;
  unsigned int feet = altitude;
  feet *= 100;
  feet /= 3048;
  Serial.print(altitude);
  Serial.print(" ");
  Serial.println(feet);
  if ((feet != announced_feet) && (audio_len==0))  {
    say(feet);
    announced_feet = feet;
  }
}

/*
   speaker_pcm

   Plays 8-bit PCM audio on pin 11 using pulse-width modulation (PWM).
   For Arduino with Atmega168 at 16 MHz.

   Uses two timers. The first changes the sample value 8000 times a second.
   The second holds pin 11 high for 0-255 ticks out of a 256-tick cycle,
   depending on sample value. The second timer repeats 62500 times per second
   (16000000 / 256), much faster than the playback rate (8000 Hz), so
   it almost sounds halfway decent, just really quiet on a PC speaker.

   Takes over Timer 1 (16-bit) for the 8000 Hz timer. This breaks PWM
   (analogWrite()) for Arduino pins 9 and 10. Takes Timer 2 (8-bit)
   for the pulse width modulation, breaking PWM for pins 11 & 3.

   References:
       http://www.uchobby.com/index.php/2007/11/11/arduino-sound-part-1/
       http://www.atmel.com/dyn/resources/prod_documents/doc2542.pdf
       http://www.evilmadscientist.com/article.php/avrdac
       http://gonium.net/md/2006/12/27/i-will-think-before-i-code/
       http://fly.cc.fer.hr/GDM/articles/sndmus/speaker2.html
       http://www.gamedev.net/reference/articles/article442.asp

   Michael Smith <michael@hurts.ca>
*/

#include <stdint.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/pgmspace.h>

#define SAMPLE_RATE 8000


volatile uint16_t sample;
byte lastSample;


void stopPlayback()
{
  // Disable playback per-sample interrupt.
  TIMSK1 &= ~_BV(OCIE1A);

  // Disable the per-sample timer completely.
  TCCR1B &= ~_BV(CS10);

  // Disable the PWM timer.
  TCCR2B &= ~_BV(CS10);

  digitalWrite(3, LOW);
}

// This is called at 8000 Hz to load the next sample.
/*ISR(TIMER1_COMPA_vect) {
    if (sample >= sounddata_length) {
        if (sample == sounddata_length + lastSample) {
            stopPlayback();
        }
        else {
                OCR2B = sounddata_length + lastSample - sample;
        }
    }
    else {
            OCR2B = pgm_read_byte(&sounddata_data[sample]);
    }

    ++sample;
  }*/

void startPlayback()
{
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

  // Set initial pulse width to the first sample.
  //        OCR2B = pgm_read_byte(&sounddata_data[0]);



  // Set up Timer 1 to send a sample every interrupt.
  /*
      cli();

      // Set CTC mode (Clear Timer on Compare Match) (p.133)
      // Have to set OCR1A *after*, otherwise it gets reset to 0!
      TCCR1B = (TCCR1B & ~_BV(WGM13)) | _BV(WGM12);
      TCCR1A = TCCR1A & ~(_BV(WGM11) | _BV(WGM10));

      // No prescaler (p.134)
      TCCR1B = (TCCR1B & ~(_BV(CS12) | _BV(CS11))) | _BV(CS10);

      // Set the compare register (OCR1A).
      // OCR1A is a 16-bit register, so we have to do this with
      // interrupts disabled to be safe.
      OCR1A = F_CPU / SAMPLE_RATE;    // 16e6 / 8000 = 2000

      // Enable interrupt when TCNT1 == OCR1A (p.136)
      TIMSK1 |= _BV(OCIE1A);

      lastSample = pgm_read_byte(&sounddata_data[sounddata_length-1]);
      sample = 0;
      sei();
  */
}


void setup()
{
  startPlayback();

  // put your setup code here, to run once:
  Timer1.initialize(125 * 2);
  Timer1.attachInterrupt(audio_callback);
  audio_count = 0;
  pinMode(3, OUTPUT);
  pinMode(11, OUTPUT);
  Serial.begin(9600);
  myLidarLite.begin(0, true); // Set configuration to default and I2C to 400 kHz
    /*
    configure(int configuration, char lidarliteAddress)

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
  myLidarLite.configure(0); // Change this number to try out alternate configurations
}


