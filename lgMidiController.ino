// This project uses an Arduino Due for MIDI compatible USB communication if wanted

//#define PRINT_SERIAL
#define USE_USB_MIDI
#define USE_RGB
#define USE_RGB_STRIP

#include <MIDI.h>     // MIDI Library
#ifdef USE_USB_MIDI
  #include <USB-MIDI.h> // USB-MIDI Library
#endif
#include <Bounce2.h>  // Bounce2 Library

#include "MidiChannel.h"
#include "Display.h"
#ifdef USE_RGB_STRIP
  #include "LedStrip.h"
#endif

#define LED_PIN               LED_BUILTIN

#ifdef USE_RGB
  #define RGB_STRIP_G         3
  #define RGB_STRIP_R         4
  #define RGB_STRIP_B         6
#endif

// TX1: 18
// RX1: 19

// SDA: 20
// SCL: 21

#define PIN_IN_CH_1           22    // MIDI channel selection: Bit 1
#define PIN_IN_CH_2           23    // MIDI channel selection: Bit 2
#define PIN_IN_CH_3           24    // MIDI channel selection: Bit 3
#define PIN_IN_CH_4           25    // MIDI channel selection: Bit 4

#define PIN_IN_KEY_C_1        26    // Key  1 C   (low)
#define PIN_IN_KEY_Cis_2      27    // Key  2 C#
#define PIN_IN_KEY_D_3        28    // Key  3 D
#define PIN_IN_KEY_Dis_4      29    // Key  4 D#
#define PIN_IN_KEY_E_5        30    // Key  5 E
#define PIN_IN_KEY_F_6        31    // Key  6 F
#define PIN_IN_KEY_Fis_7      32    // Key  7 F#
#define PIN_IN_KEY_G_8        33    // Key  8 G
#define PIN_IN_KEY_Gis_9      34    // Key  9 G#
#define PIN_IN_KEY_A_10       35    // Key 10 A
#define PIN_IN_KEY_Ais_11     36    // Key 11 A#
#define PIN_IN_KEY_B_12       37    // Key 12 B
#define PIN_IN_KEY_C_13       38    // Key 13 C   (high)

#define PIN_IN_OCT_UP         39    // Octave Up
#define PIN_IN_OCT_DOWN       40    // Octave Down

#define PIN_IN_HOLD           41    // Aux control: Hold note
#define PIN_IN_CTRL_1         42    // Aux control: CC24
#define PIN_IN_CTRL_2         43    // Aux control: CC25
#define PIN_IN_CTRL_3         44    // Aux control: CC26

#define PIN_IN_TRSPRT_LEFT    45    // Transport button: left:  CC20
#define PIN_IN_TRSPRT_DOWN    46    // Transport button: down:  CC21
#define PIN_IN_TRSPRT_RIGHT   47    // Transport button: right: CC22
#define PIN_IN_TRSPRT_UP      48    // Transport button: up:    CC23

#define PIN_IN_SW_MODE        49    // Mode switch: MIDI or USB-MIDI

#define DEBOUNCE_BUTTON_MS    5     // Debounce duration in ms
#define DEBOUNCE_KEY_MS       5     // Debounce duration in ms
#define CC_DELAY_MS           2     // Number of ms between enabling and disabling CC

//MIDI_CREATE_DEFAULT_INSTANCE();
MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);  // Initialize serial MIDI on TX1
#ifdef USE_USB_MIDI  
  //USBMIDI_CREATE_DEFAULT_INSTANCE();
  USBMIDI_CREATE_INSTANCE(0, MIDIUSB)                 // Initialize native USB MIDI
#endif

int Channel = 1;          // Midi channel, read from DIP
int Octave = 2;           // Current octave. -1 to 9.
bool IsHold = false;      // Is hold mode active?
bool UseUsbMidi = false;  // Use native USB MIDI?

Bounce2::Button btnKey1_C = Bounce2::Button();
Bounce2::Button btnKey2_Cis = Bounce2::Button();
Bounce2::Button btnKey3_D = Bounce2::Button();
Bounce2::Button btnKey4_Dis = Bounce2::Button();
Bounce2::Button btnKey5_E = Bounce2::Button();
Bounce2::Button btnKey6_F = Bounce2::Button();
Bounce2::Button btnKey7_Fis = Bounce2::Button();
Bounce2::Button btnKey8_G = Bounce2::Button();
Bounce2::Button btnKey9_Gis = Bounce2::Button();
Bounce2::Button btnKey10_A = Bounce2::Button();
Bounce2::Button btnKey11_Ais = Bounce2::Button();
Bounce2::Button btnKey12_B = Bounce2::Button();
Bounce2::Button btnKey13_C = Bounce2::Button();
Bounce2::Button btnOctUp = Bounce2::Button();
Bounce2::Button btnOctDown = Bounce2::Button();
Bounce2::Button btnAuxHold = Bounce2::Button();
Bounce2::Button btnAuxCtrl1 = Bounce2::Button();
Bounce2::Button btnAuxCtrl2 = Bounce2::Button();
Bounce2::Button btnAuxCtrl3 = Bounce2::Button();
Bounce2::Button btnAuxTrsprtLeft = Bounce2::Button();
Bounce2::Button btnAuxTrsprtDown = Bounce2::Button();
Bounce2::Button btnAuxTrsprtRight = Bounce2::Button();
Bounce2::Button btnAuxTrsprtUp = Bounce2::Button();

int KeyStates[] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  };

void setup()
{
#ifdef PRINT_SERIAL
  Serial.begin(115200);
  Serial.println("Launching MIDI controller");
#endif
  
  // Set LED pin
  pinMode(LED_PIN, OUTPUT);
  
#ifdef USE_USB_MIDI
  // Option for MIDI mode selection
  pinMode(PIN_IN_SW_MODE, INPUT);
  int nMode = digitalRead(PIN_IN_SW_MODE);
  if (nMode == 0)
  {
    UseUsbMidi = false;
#ifdef PRINT_SERIAL
    Serial.println("Use serial MIDI");
#endif
  }
  else
  {
    UseUsbMidi = true;
#ifdef PRINT_SERIAL
    Serial.println("Use USB-MIDI");
#endif
  }
#endif

  // Read MIDI channel from DIP switches
  Channel = ReadMidiChannel(PIN_IN_CH_1, PIN_IN_CH_2, PIN_IN_CH_3, PIN_IN_CH_4);

  // Launch MIDI and set channel
#ifdef USE_USB_MIDI
  if (UseUsbMidi)
  {  
    MIDIUSB.begin(Channel);
  }
  else
#endif
  {  
    MIDI.begin(Channel);
  }
#ifdef PRINT_SERIAL
  Serial.print("MIDI Channel ");
  Serial.println(Channel);
#endif
        
  // Setup debounce button
  RegisterKey(&btnKey1_C, PIN_IN_KEY_C_1, DEBOUNCE_KEY_MS);
  RegisterKey(&btnKey2_Cis, PIN_IN_KEY_Cis_2, DEBOUNCE_KEY_MS);
  RegisterKey(&btnKey3_D, PIN_IN_KEY_D_3, DEBOUNCE_KEY_MS);
  RegisterKey(&btnKey4_Dis, PIN_IN_KEY_Dis_4, DEBOUNCE_KEY_MS);
  RegisterKey(&btnKey5_E, PIN_IN_KEY_E_5, DEBOUNCE_KEY_MS);
  RegisterKey(&btnKey6_F, PIN_IN_KEY_F_6, DEBOUNCE_KEY_MS);
  RegisterKey(&btnKey7_Fis, PIN_IN_KEY_Fis_7, DEBOUNCE_KEY_MS);
  RegisterKey(&btnKey8_G, PIN_IN_KEY_G_8, DEBOUNCE_KEY_MS);
  RegisterKey(&btnKey9_Gis, PIN_IN_KEY_Gis_9, DEBOUNCE_KEY_MS);
  RegisterKey(&btnKey10_A, PIN_IN_KEY_A_10, DEBOUNCE_KEY_MS);
  RegisterKey(&btnKey11_Ais, PIN_IN_KEY_Ais_11, DEBOUNCE_KEY_MS);
  RegisterKey(&btnKey12_B, PIN_IN_KEY_B_12, DEBOUNCE_KEY_MS);
  RegisterKey(&btnKey13_C, PIN_IN_KEY_C_13, DEBOUNCE_KEY_MS);

  RegisterButton(&btnOctUp, PIN_IN_OCT_UP, DEBOUNCE_BUTTON_MS);
  RegisterButton(&btnOctDown, PIN_IN_OCT_DOWN, DEBOUNCE_BUTTON_MS);
  RegisterButton(&btnAuxHold, PIN_IN_HOLD, DEBOUNCE_BUTTON_MS);
  RegisterButton(&btnAuxCtrl1, PIN_IN_CTRL_1, DEBOUNCE_BUTTON_MS);
  RegisterButton(&btnAuxCtrl2, PIN_IN_CTRL_2, DEBOUNCE_BUTTON_MS);
  RegisterButton(&btnAuxCtrl3, PIN_IN_CTRL_3, DEBOUNCE_BUTTON_MS);
  RegisterButton(&btnAuxTrsprtLeft, PIN_IN_TRSPRT_LEFT, DEBOUNCE_BUTTON_MS);
  RegisterButton(&btnAuxTrsprtDown, PIN_IN_TRSPRT_DOWN, DEBOUNCE_BUTTON_MS);
  RegisterButton(&btnAuxTrsprtRight, PIN_IN_TRSPRT_RIGHT, DEBOUNCE_BUTTON_MS);
  RegisterButton(&btnAuxTrsprtUp, PIN_IN_TRSPRT_UP, DEBOUNCE_BUTTON_MS);

  setupDisplay();

  RgbOff();

#ifdef USE_RGB_STRIP
  setupStrip();
#endif

#ifdef PRINT_SERIAL
  Serial.println("Setup done. Starting loop.");
#endif
}

void loop()
{  
  btnOctUp.update();
  if (btnOctUp.pressed() && Octave < 8)
  {
    Octave++;
#ifdef PRINT_SERIAL
    Serial.println("Octave up");
#endif
  }

  btnOctDown.update();
  if (btnOctDown.pressed() && Octave > -2)
  {
    Octave--;
#ifdef PRINT_SERIAL
    Serial.println("Octave down");
#endif
  }

  btnAuxHold.update();
  if (btnAuxHold.pressed())
  {
    IsHold = !IsHold;
#ifdef PRINT_SERIAL
    Serial.print("Hold: ");
    Serial.println(IsHold);
#endif
  }

  PerformKey(btnKey1_C, 0);
  PerformKey(btnKey2_Cis, 1);
  PerformKey(btnKey3_D, 2);
  PerformKey(btnKey4_Dis, 3);
  PerformKey(btnKey5_E, 4);
  PerformKey(btnKey6_F, 5);
  PerformKey(btnKey7_Fis, 6);
  PerformKey(btnKey8_G, 7);
  PerformKey(btnKey9_Gis, 8);
  PerformKey(btnKey10_A, 9);
  PerformKey(btnKey11_Ais, 10);
  PerformKey(btnKey12_B, 11);
  PerformKey(btnKey13_C, 12);

  PerformCC(btnAuxTrsprtLeft, 0);
  PerformCC(btnAuxTrsprtDown, 1);
  PerformCC(btnAuxTrsprtRight, 2);
  PerformCC(btnAuxTrsprtUp, 3);
  PerformCC(btnAuxCtrl1, 4);
  PerformCC(btnAuxCtrl2, 5);
  PerformCC(btnAuxCtrl3, 6);
  
  loopDisplay(Octave, IsHold, Channel, UseUsbMidi);

#ifdef USE_RGB_STRIP
  loopStrip(KeyStates);
#endif
}

void RegisterKey(Bounce2::Button* btn, int channel, int debounce_ms)
{
  btn->attach(channel, INPUT_PULLUP); // INPUT: external pull-up, INPUT_PULLUP: internal pull-up
  btn->interval(debounce_ms);
  btn->setPressedState(HIGH);
}

void RegisterButton(Bounce2::Button* btn, int channel, int debounce_ms)
{
  btn->attach(channel, INPUT_PULLUP); // INPUT: external pull-up, INPUT_PULLUP: internal pull-up
  btn->interval(debounce_ms);
  btn->setPressedState(HIGH);
}

void PerformKey(Bounce2::Button &button, int idxKey)
{
  button.update();


  // Hold: Freeze note until key is pressed again
  if (IsHold)
  {
    bool isPressed = button.pressed();
    if (isPressed)
    {
      // Has not been frozen
      if (KeyStates[idxKey] < 0)
      {
        digitalWrite(LED_PIN, HIGH);
        int key = KeyIndexToNote(idxKey);
        
#ifdef USE_USB_MIDI
        if (UseUsbMidi)
        {  
          MIDIUSB.sendNoteOn(key, 127, Channel);
        }
        else
#endif
        {  
          MIDI.sendNoteOn(key, 127, Channel);
        }
        
        KeyStates[idxKey] = key;
        digitalWrite(LED_PIN, LOW);
#ifdef PRINT_SERIAL
        Serial.print(key);
        Serial.println(" hold");
#endif
        RgbOn();
      }
      else
      {
        digitalWrite(LED_PIN, HIGH);
        
#ifdef USE_USB_MIDI
        if (UseUsbMidi)
        {  
          MIDIUSB.sendNoteOff(KeyStates[idxKey], 0, Channel);
        }
        else
#endif
        {  
          MIDI.sendNoteOff(KeyStates[idxKey], 0, Channel);
        }
        
        digitalWrite(LED_PIN, LOW);
        KeyStates[idxKey] = -1;
#ifdef PRINT_SERIAL
        Serial.print(KeyIndexToNote(idxKey));
        Serial.println(" hold released");
#endif
        RgbOff();
      }
    }
  }
  // !Hold: Hold note only while key is pressed
  else
  {
    bool isPressed = button.read(); // Inverted to check press down correctly
    if (isPressed && KeyStates[idxKey] < 0)
    {
      digitalWrite(LED_PIN, HIGH);
      int key = KeyIndexToNote(idxKey);
    
#ifdef USE_USB_MIDI
      if (UseUsbMidi)
      {  
        MIDIUSB.sendNoteOn(key, 127, Channel);
      }
      else
#endif
      {  
        MIDI.sendNoteOn(key, 127, Channel);
      }
      
      KeyStates[idxKey] = key;
      digitalWrite(LED_PIN, LOW);
#ifdef PRINT_SERIAL
      Serial.print(key);
      Serial.println(" pressed");
#endif
      RgbOn();
    }
    else if (!isPressed && KeyStates[idxKey] >= 0)
    {
      digitalWrite(LED_PIN, HIGH);

#ifdef USE_USB_MIDI
      if (UseUsbMidi)
      {  
        MIDIUSB.sendNoteOff(KeyStates[idxKey], 0, Channel);
      }
      else
#endif
      {  
        MIDI.sendNoteOff(KeyStates[idxKey], 0, Channel);
      }
      
      digitalWrite(LED_PIN, LOW);
      KeyStates[idxKey] = -1;
#ifdef PRINT_SERIAL
      Serial.print(KeyIndexToNote(idxKey));
      Serial.println(" released");
#endif
      RgbOff();
    }
  }
}

void PerformCC(Bounce2::Button &button, int idxCC)
{
  button.update();

  bool isPressed = button.pressed();
  if (!isPressed)
    return;
  
  // 20-31 = Undefined
  int CC = 20 + idxCC;
  
  digitalWrite(LED_PIN, HIGH);

#ifdef USE_USB_MIDI
  if (UseUsbMidi)
  {  
    MIDIUSB.sendControlChange(CC, 127, Channel);
  }
  else
#endif
  {  
    MIDI.sendControlChange(CC, 127, Channel);
  }
  
  delay(CC_DELAY_MS);

#ifdef USE_USB_MIDI
  if (UseUsbMidi)
  {  
    MIDIUSB.sendControlChange(CC, 0, Channel);
  }
  else
#endif
  {  
    MIDI.sendControlChange(CC, 0, Channel);
  }

  digitalWrite(LED_PIN, LOW);
#ifdef PRINT_SERIAL
  Serial.print("CC");
  Serial.print(CC);
  Serial.println(" send");
#endif
}

int KeyIndexToNote(int idxKey)
{
  // C-1 = 0
  // C0 = 12
  // C1 = 24
  // C2 = 36
  // C3 = 48
  // C4 = 60
  // C5 = 72
  // C6 = 84
  // C7 = 96
  // C8 = 108
  // C9 = 120
  return (Octave + 2) * 12 + idxKey;
}

void RgbOff()
{
  #ifdef USE_RGB
    analogWrite(RGB_STRIP_G, 0);
    analogWrite(RGB_STRIP_R, 128);
    analogWrite(RGB_STRIP_B, 128);
  #endif
}

void RgbOn()
{
  #ifdef USE_RGB
    analogWrite(RGB_STRIP_G, 128);
    analogWrite(RGB_STRIP_R, 0);
    analogWrite(RGB_STRIP_B, 128);
  #endif
}
 
