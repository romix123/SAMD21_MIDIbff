/*
 *******************************************************************************
 * USB-MIDI to Legacy Serial MIDI converter
 * Copyright (C) 2012-2017 Yuuichi Akagawa
 *
 * Idea from LPK25 USB-MIDI to Serial MIDI converter
 *   by Collin Cunningham - makezine.com, narbotic.com
 *
 * This is sample program. Do not expect perfect behavior.
 *******************************************************************************
 */

#include <usbh_midi.h>
#include <usbhub.h>
#include <Adafruit_NeoPixel.h>
#define NEOPIXEL      11
#define NEO_PWR       12
Adafruit_NeoPixel strip = Adafruit_NeoPixel(1, NEOPIXEL, NEO_GRB + NEO_KHZ800);

#ifdef USBCON
#define _MIDI_SERIAL_PORT Serial1
#else
#define _MIDI_SERIAL_PORT Serial
#endif
//////////////////////////
// MIDI Pin assign
// 2 : GND
// 4 : +5V(Vcc) with 220ohm
// 5 : TX
//////////////////////////

USBHost UsbH;
USBH_MIDI  Midi(&UsbH);

void MIDI_poll();
void doDelay(uint32_t t1, uint32_t t2, uint32_t delayTime);

void setup()
{
  _MIDI_SERIAL_PORT.begin(31250);

  if (UsbH.Init()) {
    while (1); //halt
  }
  delay( 200 );
    strip.begin();
  strip.setBrightness(50);
  strip.show(); // Initialize all pixels to 'off'
}

void loop()
{
  UsbH.Task();
  uint32_t t1 = (uint32_t)micros();
  if ( Midi ) {
    MIDI_poll();
  }
  //delay(1ms)
  doDelay(t1, (uint32_t)micros(), 1000);
}

// Poll USB MIDI Controler and send to serial MIDI
void MIDI_poll()
{
  uint8_t outBuf[ 3 ];
  uint8_t size;

  do {
    if ( (size = Midi.RecvData(outBuf)) > 0 ) {
      //MIDI Output
      _MIDI_SERIAL_PORT.write(outBuf, size);
      strip.setPixelColor(0, Wheel((outBuf[1]-36)*4));        
      strip.show();
    }
  } while (size > 0);
}

// Delay time (max 16383 us)
void doDelay(uint32_t t1, uint32_t t2, uint32_t delayTime)
{
  uint32_t t3;

    t3 = t2 - t1;
  if ( t3 < delayTime ) {
    delayMicroseconds(delayTime - t3);
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
