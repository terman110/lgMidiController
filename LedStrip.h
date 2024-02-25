#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif
#ifndef PSTR
 #define PSTR // Make Arduino Due happy
#endif

// WS2812 with DIN at pin 8
#define PIN_STRIP_DIN 8

// Strip with 10 segments
const int NUM_LEDS = 10;

#define BRIGHTNESS 60
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, PIN_STRIP_DIN, NEO_BRG + NEO_KHZ800);

long HSBtoRGB(float _hue, float _sat, float _brightness) {
   float red = 0.0;
   float green = 0.0;
   float blue = 0.0;
   
   if (_sat == 0.0) {
       red = _brightness;
       green = _brightness;
       blue = _brightness;
   } else {
       if (_hue == 360.0) {
           _hue = 0;
       }

       int slice = _hue / 60.0;
       float hue_frac = (_hue / 60.0) - slice;

       float aa = _brightness * (1.0 - _sat);
       float bb = _brightness * (1.0 - _sat * hue_frac);
       float cc = _brightness * (1.0 - _sat * (1.0 - hue_frac));
       
       switch(slice) {
           case 0:
               red = _brightness;
               green = cc;
               blue = aa;
               break;
           case 1:
               red = bb;
               green = _brightness;
               blue = aa;
               break;
           case 2:
               red = aa;
               green = _brightness;
               blue = cc;
               break;
           case 3:
               red = aa;
               green = bb;
               blue = _brightness;
               break;
           case 4:
               red = cc;
               green = aa;
               blue = _brightness;
               break;
           case 5:
               red = _brightness;
               green = aa;
               blue = bb;
               break;
           default:
               red = 0.0;
               green = 0.0;
               blue = 0.0;
               break;
       }
   }

   long ired = red * 255.0;
   long igreen = green * 255.0;
   long iblue = blue * 255.0;
   
   return long((ired << 16) | (igreen << 8) | (iblue));
}


long SetPixelColor(long color, int x)
{  
  // Get the red, blue and green parts from generated color
  int r = color >> 16 & 255;
  int g = color >> 8 & 255;
  int b = color & 255;
  strip.setPixelColor(x, strip.Color(r, g, b));
  return color;
}

void SetPixelColor(int r, int g, int b, int x)
{  
  strip.setPixelColor(x, strip.Color(r, g, b));
}

void SetPixelColorOn(int x)
{  
  strip.setPixelColor(x, strip.Color(0, 128, 128));
}

void SetPixelColorOff(int x)
{  
  strip.setPixelColor(x, strip.Color(128, 0, 128));
}

void SetPixelColor(int x, bool on)
{
  if (on)
    SetPixelColorOn(x);
  else
    SetPixelColorOff(x);
}

long ToPixelColor(int r, int g, int b)
{
  return ((long)r << 16) + ((long)g << 8) + (long)b;
}

void FromPixelColor(long c, int& r, int& g, int& b)
{
    r = c >> 16 & 0xFF;
    g = c >> 8 & 0xFF;
    b = c >> 0 & 0xFF;
}

int getCheckSum(String s) 
{
  int i, XOR, c;
  for (XOR = 0, i = 0; i < s.length(); i++) {
    c = (unsigned char)s.charAt(i);
    if (c == '*') break;
    if ((c!='$') && (c!='!')) XOR ^= c;
  }
  return XOR; 
}

byte nibble(char c)
{
  if (c >= '0' && c <= '9')
    return c - '0';

  if (c >= 'a' && c <= 'f')
    return c - 'a' + 10;

  if (c >= 'A' && c <= 'F')
    return c - 'A' + 10;

  return 0;  // Not a valid hexadecimal character
}

byte toByte(char a, char b)
{
  return nibble(a) << 4 | nibble(b);
}

char nibble_to_hex(uint8_t nibble) {  // convert a 4-bit nibble to a hexadecimal character
  nibble &= 0xF;
  return nibble > 9 ? nibble - 10 + 'A' : nibble + '0';
}

String byte_to_str(int v) {  // convert an 8-bit byte to a string of 2 hexadecimal characters
  return String(nibble_to_hex(v >> 4)) + String(nibble_to_hex(v));
}

void setupStrip()
{
  // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
  #if defined (__AVR_ATtiny85__)
    if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
  #endif
  // End of trinket special code
  
  strip.setBrightness(BRIGHTNESS);
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'

  for(int i = 0; i <= 128; ++i)
  {
    for(int x = 0; x < NUM_LEDS; ++x)
      SetPixelColor(i, 0, i, x);
    strip.show();
    delay(15);
  }
}

int lastKeyStates[] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  };

void loopStrip(int* keyStates) 
{
  bool hasChanges = false;
  for(int i = 0; i < 13; ++i)
  {
    if (lastKeyStates[i] != keyStates[i])
    {
      hasChanges = true;
      break;
    }
  }

  if (!hasChanges)
    return;

  for(int i = 0; i < 13; ++i)
    lastKeyStates[i] = keyStates[i];

  // PX 0: Key 12
  SetPixelColor(0, lastKeyStates[12] >= 0);
  
  // PX 1: Key 11
  SetPixelColor(1, lastKeyStates[11] >= 0);
  
  // PX 2: Key 9 or 10 or 11
  SetPixelColor(2, lastKeyStates[9] >= 0 || lastKeyStates[10] >= 0 || lastKeyStates[11] >= 0);
  
  // PX 3: Key 8 or 9
  SetPixelColor(3, lastKeyStates[8] >= 0 || lastKeyStates[9] >= 0);
  
  // PX 4: Key 6 or 7
  SetPixelColor(4, lastKeyStates[6] >= 0 || lastKeyStates[7] >= 0);
  
  // PX 5: Key 5 or 6
  SetPixelColor(5, lastKeyStates[5] >= 0 || lastKeyStates[6] >= 0);
  
  // PX 6: Key 4
  SetPixelColor(6, lastKeyStates[4] >= 0);
  
  // PX 7: Key 2 or 3 or 4
  SetPixelColor(7, lastKeyStates[2] >= 0 || lastKeyStates[3] >= 0 || lastKeyStates[4] >= 0);
  
  // PX 8: Key 1 or 2
  SetPixelColor(8, lastKeyStates[1] >= 0 || lastKeyStates[2] >= 0);
  
  // PX 9: Key 0
  SetPixelColor(9, lastKeyStates[0] >= 0);
      
  strip.show();
}
