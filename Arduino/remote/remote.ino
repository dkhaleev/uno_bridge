/* SDRuno Bridge plugin mocking firmware v0.1

*/
#include <TimerOne.h>

//first PCF port replicator routine
#include "lib/PCF8575.cpp"
PCF8575 PCF_1(0x20);
volatile bool PCF_1_FLAG = false;
const int PCF_1_IRQ_PIN = 18;
uint16_t pcf_1_status = 65535; //all pins high

//TFT screen routine
#include "lib/TFT_HX8357.cpp"
TFT_HX8357 tft = TFT_HX8357();

//pinChangeInerrupts routine. Used this because of exhausted hardware interrupts
#include "PinChangeInterrupt.h"

//Rotary encoder routine
int encoderAPinA = 66; //A12
int encoderAPinB = 67; //A13
uint8_t encoderAPinALast = LOW;

struct __attribute__((__packed__)) State {
  int8_t      Demodulator             = 0; //Demodulator type,        Enum:
  //    DemodulatorNone = 0,
  //    DemodulatorAM = 1,
  //    DemodulatorSAM = 2,
  //    DemodulatorNFM = 3,
  //    DemodulatorMFM = 4,
  //    DemodulatorWFM = 5,
  //    DemodulatorSWFM = 6,
  //    DemodulatorDSB = 7,
  //    DemodulatorLSB = 8,
  //    DemodulatorUSB = 9,
  //    DemodulatorCW = 10,
  //    DemodulatorDigital = 11,
  //    DemodulatorDAB = 12,
  //    DemodulatorIQOUT = 13
  int8_t      WfmDeemphasisMode       = 0; //
  //    DeemphasisNone = 0,
  //    Deemphasis50us = 1,
  //    Deemphasis75us = 2,
  int8_t      NoiseBlankerMode        = 0;
  //    NoiseBlankerOff = 0,
  //    NoiseBlankerBwWide = 1,
  //    NoiseBlankerBwNarrow = 2
  int8_t      AgcMode                 = 0;
  //    AGCModeOff = 0,
  //    AGCModeSlow = 1,
  //    AGCModeMedium = 2,
  //    AGCModeFast = 3
  int8_t      AgcThreshold            = 0;
  int8_t      NoiseBlankerLevel       = 0;
  int8_t      NoiseReductionLevel     = 0;
  int8_t      CwPeakFilterThreshold   = 0;
  int8_t      AudioVolume             = 0;
  int8_t      SP1MinPower             = 0;
  long int    VfoFrequency            = 0;          //VFO Freq,                Hz
  long int    CenterFrequency         = 443300000;  //Center Frequency,        Hz
  long int    SP1MinFrequency         = 0;
  long int    SP1MaxFrequency         = 0;
  long int    MPXLevel                = 0;
  int         FilterBandwidth         = 4300;       //Filter bandwith,         Hz
  int         SquelchLevel            = 0;
  bool        SquelchEnable           = false;
  bool        FmNoiseReductionEnable  = false;
  bool        AudioMute               = false;
  bool        BiasTEnable             = false;
  //non essential params
  uint16_t    Step                    = 16;         //trailing four bits are reserved for aux and does not mapped to step place highlighter
  bool        VfoMode                 = false;      //

  long int    fingerprint;                          //hash-like substitute. @ToDo: rework me later
} state;

bool state_changed = true;

//LED params
// pin 11 of 74HC595 (SHCP)
const int bit_clock_pin = 14; //SCLK
// pin 12 of 74HC595 (STCP)
const int digit_clock_pin = 15; //LOAD/latch
// pin 14 of 74HC595 (DS)
const int data_pin = 2; //SDI pin

// digit pattern for a 7-segment display
const byte digit_pattern[16] =
{
  B00111111,  // 0
  B00000110,  // 1
  B01011011,  // 2
  B01001111,  // 3
  B01100110,  // 4
  B01101101,  // 5
  B01111101,  // 6
  B00000111,  // 7
  B01111111,  // 8
  B01101111,  // 9
  B01110111,  // A
  B01111100,  // b
  B00111001,  // C
  B01011110,  // d
  B01111001,  // E
  B01110001   // F
};

//temporary substitute for checksum
long lastRandomSent;

unsigned long timepassed = millis();

unsigned long passed_10 = 0;

void setup() {
  Serial.begin(9600);
  Serial.setTimeout(10); //10 mS
  State state;

  //LED pins setup
  pinMode(data_pin,         OUTPUT);
  pinMode(bit_clock_pin,    OUTPUT);
  pinMode(digit_clock_pin,  OUTPUT);

  //Timer interrupt for display freq;
  Timer1.initialize(10000); //uS timing value
  Timer1.attachInterrupt(fillRegisters_isr);

  //PCF_1 interrupt for keyboard and buttons
  PCF_1.begin();
  pinMode(PCF_1_IRQ_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PCF_1_IRQ_PIN), pcf_isr, FALLING);

  //  pinMode(66, INPUT_PULLUP);
  //  pinMode(67, INPUT_PULLUP);
  //  pinMode(68, INPUT_PULLUP);
  //  pinMode(69, INPUT_PULLUP);
  attachPinChangeInterrupt(digitalPinToPinChangeInterrupt(66), encoder_a_isr, CHANGE);
  //  attachInterrupt(digitalPinToInterrupt(A12), encoder_isr, FALLING);

  //TFT init
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(0x0000);
}

void loop() {
  state.fingerprint = random();

  if (state_changed) {
    updateDisplay();
    echoStruct();
    state_changed = false;
  }

  if (PCF_1_FLAG) {
    processPCF();
  }

  lastRandomSent = state.fingerprint;

  String str = Serial.readStringUntil('\n');

  if (str.length() > 0) {
    //      parseStruct(str);
    //    Serial.println("Read value");
    //    Serial.print(state.Demodulator);
    //    Serial.print("\t");
    //    Serial.print(state.VfoFrequency);
    //    Serial.print("\t");
    //    Serial.print(state.CenterFrequency);
    //    Serial.print("\t");
    //    Serial.print(state.FilterBandwidth);
    //    Serial.print("\t");
    //    Serial.print(state.fingerprint);
    //    Serial.println("");
  }
}

void echoStruct() {
  //  Serial.print(state.Demodulator);
  //  Serial.print("\t");
  //  Serial.print(state.WfmDeemphasisMode);
  //  Serial.print("\t");
  //  Serial.print(state.NoiseBlankerMode);
  //  Serial.print("\t");
  //  Serial.print(state.AgcMode);
  //  Serial.print("\t");
  //  Serial.print(state.AgcThreshold);
  //  Serial.print("\t");
  //  Serial.print(state.NoiseBlankerLevel);
  //  Serial.print("\t");
  //  Serial.print(state.NoiseReductionLevel);
  //  Serial.print("\t");
  //  Serial.print(state.CwPeakFilterThreshold);
  //  Serial.print("\t");
  //  Serial.print(state.AudioVolume);
  //  Serial.print("\t");
  //  Serial.print(state.SP1MinPower);
  //  Serial.print("\t");
  //  Serial.print(state.VfoFrequency);
  //  Serial.print("\t");
  //  Serial.print(state.CenterFrequency);
  //  Serial.print("\t");
  //  Serial.print(state.SP1MinFrequency);
  //  Serial.print("\t");
  //  Serial.print(state.SP1MaxFrequency);
  //  Serial.print("\t");
  //  Serial.print(state.MPXLevel);
  //  Serial.print("\t");
  //  Serial.print(state.FilterBandwidth);
  //  Serial.print("\t");
  //  Serial.print(state.SquelchLevel);
  //  Serial.print("\t");
  //  Serial.print(state.SquelchEnable);
  //  Serial.print("\t");
  //  Serial.print(state.FmNoiseReductionEnable);
  //  Serial.print("\t");
  //  Serial.print(state.AudioMute);
  //  Serial.print("\t");
  //  Serial.print(state.BiasTEnable);
  //  Serial.print("\t");

  //  print_binary(state.Step, 16);
  //  Serial.print(state.Step, BIN);
  //    print_binary(state.Step, 16);
  //  Serial.print("\t");

  //    Serial.print(pcf_1_status, BIN);
  print_binary(pcf_1_status, 16);

  //  Serial.print(state.fingerprint);
  Serial.println("==================");
  //  Serial.println("");
}

void parseStruct(String string) {
  const char *str = string.c_str();
  sscanf(str, "%hu %lu %lu %hu %lu",
         &state.Demodulator,
         &state.VfoFrequency,
         &state.CenterFrequency,
         &state.FilterBandwidth,
         &state.fingerprint);
}

//interrupt service routine by Timer1 interrupt signal
void fillRegisters_isr() {
  if (state_changed) {
    int array[12];
    int temporaryStep = state.Step;
    long int number = state.CenterFrequency;

    digitalWrite(digit_clock_pin, LOW);

    //highlight mute button
    if (state.AudioMute) {
      bitSet(temporaryStep, 3);
    } else {
      bitClear(temporaryStep, 3);
    }

    //highlight bias-T button
    if (state.BiasTEnable) {
      bitSet(temporaryStep, 2);
    } else {
      bitClear(temporaryStep, 2);
    }

    //highlight VFO Mode button
    if (state.VfoMode) {
      bitSet(temporaryStep, 1);
    } else {
      bitClear(temporaryStep, 1);
    }

    //highlight active step place and buttons
    shiftOut(data_pin, bit_clock_pin, LSBFIRST, temporaryStep);
    shiftOut(data_pin, bit_clock_pin, LSBFIRST, temporaryStep >> 8);

    for (int i = 12; i > 0; i--) {
      byte pattern = digit_pattern[number % 10];
      shiftOut(data_pin, bit_clock_pin, MSBFIRST, ~pattern);
      array[i] = number % 10;
      number /= 10;
    }

    digitalWrite(digit_clock_pin, HIGH);
  }
}

//interrupt service routine by PCF interrupt signal
void pcf_isr() {
  PCF_1_FLAG = true;
}

//process PCF_1 fetch and modify state accordingly
void processPCF() {
  PCF_1_FLAG = false;
  pcf_1_status = PCF_1.read16();

  //process Audio Mute button toggle
  if (!bitRead(pcf_1_status, 0))
  {
    state.AudioMute = !state.AudioMute;
  }

  //process Bias-T button toggle
  if (!bitRead(pcf_1_status, 1))
  {
    state.BiasTEnable = !state.BiasTEnable;
  }

  //process VfoMode button toggle
  if (!bitRead(pcf_1_status, 14))
  {
    state.VfoMode = !state.VfoMode;
  }

  //process StepUp button
  if (!bitRead(pcf_1_status, 15))
  {
    stepUp();
  }

  //process StepDown button
  if (!bitRead(pcf_1_status, 7))
  {
    stepDown();
  }

  //process SQL On/Off button
  if (!bitRead(pcf_1_status, 8))
  {
    state.SquelchEnable = !state.SquelchEnable;
  }

  state_changed = true;
}

void encoder_a_isr() {
  uint8_t n = digitalRead(encoderAPinA);
  if ((encoderAPinALast == LOW) && (n == HIGH) && state.SquelchEnable) {
    if (digitalRead(encoderAPinB) == LOW) {
      SQLUp();
      state_changed = true; // Changed the value
    } else {
      SQLDown();
      state_changed = true; // Changed the value
    }
  }
  encoderAPinALast = n;
}

void updateDisplay() {
  printSQL();
  printMode();
}

void printMode() {
  tft.setTextSize(2);
  tft.setTextFont(3);
  char buffer[50];
  uint16_t color;

  
    sprintf(buffer, "Mode: %s", state.VfoMode ? "VFO" : "Memory");
    tft.setTextColor(TFT_WHITE);
    tft.drawRoundRect(289, 0, 188, 34, 5, TFT_WHITE ); //X, Y, W, H, radius, Color
    tft.fillRoundRect(290, 1, 184, 32, 5, TFT_BLACK); //black mask
    tft.drawString(buffer, 296, 0, 2); //string, Y, Y, FontNumber
}

void printSQL() {
  tft.setTextSize(2);
  tft.setTextFont(3);
  char buffer[50];
  uint16_t color;

  if (state.SquelchEnable) {
    sprintf(buffer, "SQL: %d", state.SquelchLevel);
    color = TFT_WHITE;
  } else {
    sprintf(buffer, "SQL: OFF");
    color = TFT_RED;
  }

  tft.drawRoundRect(0, 0, 120, 34, 5, color);
  tft.fillRoundRect(1, 1, 118, 32, 5, TFT_BLACK);  // Black mask
  tft.setTextColor(color);
  tft.drawString(buffer, 6, 0, 2); //string, X, Y, Font number
}

//move place highlighter one step upward
void stepUp() {
  state.Step = state.Step << 1;
  if (state.Step == 0)
  {
    state.Step = 16; //4 bits reserved for buttons
  }
}

//move place highlighter one step downward
void stepDown() {
  state.Step = state.Step >> 1;
  if (state.Step < 16)
  {
    state.Step = 32768; //4 bits reserved for buttons
  }
}

void SQLUp() {
  if (state.SquelchLevel < 100) {
    state.SquelchLevel++;
  }
}

void SQLDown() {
  if (state.SquelchLevel > 0) {
    state.SquelchLevel--;
  }
}

//--------------service helper methods--------------
//print value in binary-way.
void print_binary(int v, int num_places)
{
  int mask = 0, n;

  for (n = 1; n <= num_places; n++)
  {
    mask = (mask << 1) | 0x0001;
  }
  v = v & mask;  // truncate v to specified number of places

  while (num_places)
  {

    if (v & (0x0001 << num_places - 1))
    {
      Serial.print("1");
    }
    else
    {
      Serial.print("0");
    }

    --num_places;
    if (((num_places % 4) == 0) && (num_places != 0))
    {
      Serial.print("_");
    }
  }
}
