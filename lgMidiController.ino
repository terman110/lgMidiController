#include <MIDI.h>     // MIDI Library
#include <Bounce2.h>  // Bounce2 Library

#include "MidiChannel.h"
#include "Display.h"

#define LED_PIN               LED_BUILTIN

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

#define DEBOUNCE_BUTTON_MS    5     // Debounce duration in ms
#define DEBOUNCE_KEY_MS       5     // Debounce duration in ms
#define CC_DELAY_MS           2     // Number of ms between enabling and disabling CC


//MIDI_CREATE_DEFAULT_INSTANCE();
MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);  // Initialize serial MIDI on TX1

int Channel = 1;    // Midi channel, read from DIP
int Octave = 2;     // Current octave. -1 to 9.
bool IsHold = false;  // Is hold mode active?

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
  Serial.begin(115200);
  
  // Set LED pin
  pinMode(LED_PIN, OUTPUT);

  // Read MIDI channel from DIP switches
  Channel = ReadMidiChannel(PIN_IN_CH_1, PIN_IN_CH_2, PIN_IN_CH_3, PIN_IN_CH_4);

  // Launch MIDI and set channel
  MIDI.begin(Channel);

  // Setup debounce button
  RegisterButton(&btnKey1_C, PIN_IN_KEY_C_1, DEBOUNCE_KEY_MS);
  RegisterButton(&btnKey2_Cis, PIN_IN_KEY_Cis_2, DEBOUNCE_KEY_MS);
  RegisterButton(&btnKey3_D, PIN_IN_KEY_D_3, DEBOUNCE_KEY_MS);
  RegisterButton(&btnKey4_Dis, PIN_IN_KEY_Dis_4, DEBOUNCE_KEY_MS);
  RegisterButton(&btnKey5_E, PIN_IN_KEY_E_5, DEBOUNCE_KEY_MS);
  RegisterButton(&btnKey6_F, PIN_IN_KEY_F_6, DEBOUNCE_KEY_MS);
  RegisterButton(&btnKey7_Fis, PIN_IN_KEY_Fis_7, DEBOUNCE_KEY_MS);
  RegisterButton(&btnKey8_G, PIN_IN_KEY_G_8, DEBOUNCE_KEY_MS);
  RegisterButton(&btnKey9_Gis, PIN_IN_KEY_Gis_9, DEBOUNCE_KEY_MS);
  RegisterButton(&btnKey10_A, PIN_IN_KEY_A_10, DEBOUNCE_KEY_MS);
  RegisterButton(&btnKey11_Ais, PIN_IN_KEY_Ais_11, DEBOUNCE_KEY_MS);
  RegisterButton(&btnKey12_B, PIN_IN_KEY_B_12, DEBOUNCE_KEY_MS);
  RegisterButton(&btnKey13_C, PIN_IN_KEY_C_13, DEBOUNCE_KEY_MS);

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
}

void loop()
{

  btnOctUp.update();
  if (btnOctUp.pressed() && Octave < 8)
    Octave++;

  btnOctDown.update();
  if (btnOctDown.pressed() && Octave > -2)
    Octave--;

  btnAuxHold.update();
  if (btnAuxHold.pressed())
    IsHold = !IsHold;

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
  
  loopDisplay(Octave, IsHold, Channel);
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
        MIDI.sendNoteOn(key, 127, Channel);
        KeyStates[idxKey] = key;
        digitalWrite(LED_PIN, LOW);
      }
      else
      {
        digitalWrite(LED_PIN, HIGH);
        MIDI.sendNoteOff(KeyStates[idxKey], 0, Channel);
        digitalWrite(LED_PIN, LOW);
        KeyStates[idxKey] = -1;
      }
    }
  }
  // !Hold: Hold note only while key is pressed
  else
  {
    bool isPressed = !button.read(); // Inverted to check press down correctly
    if (isPressed && KeyStates[idxKey] < 0)
    {
      digitalWrite(LED_PIN, HIGH);
      int key = KeyIndexToNote(idxKey);
      MIDI.sendNoteOn(key, 127, Channel);
      KeyStates[idxKey] = key;
      digitalWrite(LED_PIN, LOW);
    }
    else if (!isPressed && KeyStates[idxKey] >= 0)
    {
      digitalWrite(LED_PIN, HIGH);
      MIDI.sendNoteOff(KeyStates[idxKey], 0, Channel);
      digitalWrite(LED_PIN, LOW);
      KeyStates[idxKey] = -1;
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
  MIDI.sendControlChange(CC, 127, Channel);
  delay(CC_DELAY_MS);
  MIDI.sendControlChange(CC, 0, Channel);
  digitalWrite(LED_PIN, LOW);
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
