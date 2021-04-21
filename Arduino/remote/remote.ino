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

//#include "lib/rotary.cpp"
//Rotary r = Rotary(4, 3);

//pinChangeInerrupts routine. Used this because of exhausted hardware interrupts
#include "PinChangeInterrupt.h"

//Rotary encoder routine
int encoderAPinA = 66; //A12
int encoderAPinB = 67; //A13
int encoderBPinA = 68; //A14
int encoderBPinB = 69; //A15
int encoderMPinA = 65;
int encoderMPinB = 64;
uint8_t encoderAPinALast = LOW;
uint8_t encoderBPinALast = LOW;
uint8_t encoderMPinALast = LOW;

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
  int         AudioVolume             = 0;
  int8_t      SP1MinPower             = 0;
  uint32_t    VfoFrequency            = 0;          //VFO Freq,                Hz
  uint32_t    CenterFrequency         = 443300000;  //Center Frequency,        Hz
  uint32_t    SP1MinFrequency         = 0;
  uint32_t    SP1MaxFrequency         = 0;
  long int    MPXLevel                = 0;
  int         FilterBandwidth         = 4300;       //Filter bandwith,         Hz
  int         SquelchLevel            = 0;
  bool        SquelchEnable           = false;
  bool        FmNoiseReductionEnable  = false;
  bool        AudioMute               = false;
  bool        BiasTEnable             = false;
  //non transferable params
  uint16_t    Step                    = 16;         //trailing four bits are reserved for aux and does not mapped to step place highlighter
  bool        VfoMode                 = false;      //
  bool        Att                     = false;      //Attenuator mode
  bool        Preamp                  = false;      //Pre-amp mode

  long int    fingerprint;                          //hash-like substitute. @ToDo: rework me later
} state;

bool state_changed = true;
bool update_display = true;
//@todo: add dynamic state change flag system or just spread a bunch of bool flags

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

  //  pinMode(encoderAPinA, INPUT_PULLUP); Encoder-A (SQL)
  //  pinMode(encoderAPinB, INPUT_PULLUP); Encoder-A (SQL)
  //  pinMode(encoderBPinA, INPUT_PULLUP); Encoder-B (Vol)
  //  pinMode(encoderBPinB, INPUT_PULLUP); Encoder-B (Vol)
  attachPinChangeInterrupt(digitalPinToPinChangeInterrupt(encoderAPinA), encoder_a_isr, CHANGE);
  attachPinChangeInterrupt(digitalPinToPinChangeInterrupt(encoderBPinA), encoder_b_isr, CHANGE);
  pinMode(encoderMPinA, INPUT_PULLUP); //Main Encoder
  pinMode(encoderMPinB, INPUT_PULLUP); //Main Encoder
  attachPinChangeInterrupt(digitalPinToPinChangeInterrupt(encoderMPinA), encoder_m_isr, CHANGE);
  attachPinChangeInterrupt(digitalPinToPinChangeInterrupt(encoderMPinB), encoder_m_isr, CHANGE);

  //TFT init
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(0x0000);
}

void loop() {
  state.fingerprint = random();

  if (state_changed) {
    echoStruct();
    state_changed = false;
  }

  if (update_display) {
    updateDisplay();
    update_display = false;
  }

  if (PCF_1_FLAG) {
    processPCF();
  }

  lastRandomSent = state.fingerprint;

  String str = Serial.readStringUntil('\n');

  if (str.length() > 0) {
    parseStruct(str);
  }
}

void echoStruct() {
  Serial.print(state.Demodulator);
  Serial.print("\t");
  Serial.print(state.WfmDeemphasisMode);
  Serial.print("\t");
  Serial.print(state.NoiseBlankerMode);
  Serial.print("\t");
  Serial.print(state.AgcMode);
  Serial.print("\t");
  Serial.print(state.AgcThreshold);
  Serial.print("\t");
  Serial.print(state.NoiseBlankerLevel);
  Serial.print("\t");
  Serial.print(state.NoiseReductionLevel);
  Serial.print("\t");
  Serial.print(state.CwPeakFilterThreshold);
  Serial.print("\t");
  Serial.print(state.AudioVolume);
  Serial.print("\t");
  Serial.print(state.SP1MinPower);
  Serial.print("\t");
  Serial.print(state.VfoFrequency);
  Serial.print("\t");
  Serial.print(state.CenterFrequency);
  Serial.print("\t");
  Serial.print(state.SP1MinFrequency);
  Serial.print("\t");
  Serial.print(state.SP1MaxFrequency);
  Serial.print("\t");
  Serial.print(state.MPXLevel);
  Serial.print("\t");
  Serial.print(state.FilterBandwidth);
  Serial.print("\t");
  Serial.print(state.SquelchLevel);
  Serial.print("\t");
  Serial.print(state.SquelchEnable);
  Serial.print("\t");
  Serial.print(state.FmNoiseReductionEnable);
  Serial.print("\t");
  Serial.print(state.AudioMute);
  Serial.print("\t");
  Serial.print(state.BiasTEnable);
  Serial.print("\t");
  Serial.print(state.fingerprint);
  Serial.println("");
}

void parseStruct(String string) {
  const char *str = string.c_str();
  //  sscanf(str, "%hu %lu %lu %hu %lu",
  //         &state.Demodulator,
  //         &state.VfoFrequency,
  //         &state.CenterFrequency,
  //         &state.FilterBandwidth,
  //         &state.fingerprint);
}

//interrupt service routine by Timer1 interrupt signal
void fillRegisters_isr() {
  if (state_changed) {
    int array[12];
    int temporaryStep = state.Step;
    uint32_t number = state.CenterFrequency;

    digitalWrite(digit_clock_pin, LOW);

    //    //highlight mute button
    //    if (state.AudioMute) {
    //      bitSet(temporaryStep, 3);
    //    } else {
    //      bitClear(temporaryStep, 3);
    //    }

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

  //process Audio Mute button toggle //Red!
  //  if (!bitRead(pcf_1_status, 0))
  //  {
  //    state.AudioMute = !state.AudioMute;
  //    update_display = true;
  //  }

  //process Bias-T button toggle
  if (!bitRead(pcf_1_status, 1))
  {
    state.BiasTEnable = !state.BiasTEnable;
  }

  //process VfoMode button toggle
  if (!bitRead(pcf_1_status, 14))
  {
    state.VfoMode = !state.VfoMode;
    update_display = true;
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
    update_display = true;
  }

  //process Mute On/Off button
  if (!bitRead(pcf_1_status, 11))
  {
    state.AudioMute = !state.AudioMute;
    update_display = true;
  }

  //process Attenuator Toggle button
  if (!bitRead(pcf_1_status, 6))
  {
    state.Att = !state.Att;
    update_display = true;
  }

  //process Pre-Amp Toggle button
  if (!bitRead(pcf_1_status, 5))
  {
    state.Preamp = !state.Preamp;
    update_display = true;
  }

  state_changed = true;
}

//process SQL Encoder Inc/Dec
void encoder_a_isr() {
  uint8_t n = digitalRead(encoderAPinA);
  if ((encoderAPinALast == LOW) && (n == HIGH) && state.SquelchEnable) {
    if (digitalRead(encoderAPinB) == LOW) {
      SQLUp();
    } else {
      SQLDown();
    }
    state_changed = true; // Changed the value
    update_display = true;
  }
  encoderAPinALast = n;
}

//process Volume Encoder Inc/Dec
void encoder_b_isr() {
  uint8_t n = digitalRead(encoderBPinA);
  if ((encoderBPinALast == LOW) && (n == HIGH) && !state.AudioMute) {
    if (digitalRead(encoderBPinB) == LOW) {
      volDown();
    } else {
      volUp();
    }
    state_changed = true; // Changed the value
    update_display = true;
  }
  encoderBPinALast = n;
}

//process main encoder
void encoder_m_isr() {
  uint8_t n = digitalRead(encoderMPinA);
  if ((encoderMPinALast == HIGH) && (n == LOW)) {
    if (digitalRead(encoderMPinB) == HIGH) {
      mainEncInc();
    } else {
      mainEncDec();
    }
    state_changed = true; // Changed the value
  }
  encoderMPinALast = n;
}

void updateDisplay() {
  printSQL();
  printMode();
  printVol();

  printAtt();
  printPreamp();
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

  tft.drawRoundRect(0, 0, 130, 34, 5, color);
  tft.fillRoundRect(1, 1, 128, 32, 5, TFT_BLACK);  // Black mask
  tft.setTextColor(color);
  tft.drawString(buffer, 6, 0, 2); //string, X, Y, Font number
}

void printVol() {
  tft.setTextSize(2);
  tft.setTextFont(3);
  char buffer[50];
  uint16_t color;

  if (!state.AudioMute) {
    sprintf(buffer, "Vol: %d", state.AudioVolume);
    color = TFT_WHITE;
  } else {
    sprintf(buffer, "Vol: OFF");
    color = TFT_RED;
  }

  tft.drawRoundRect(0, 38, 130, 34, 5, color);
  tft.fillRoundRect(1, 39, 128, 32, 5, TFT_BLACK);
  tft.setTextColor(color);
  tft.drawString(buffer, 6, 38, 2);
}

void printAtt() {
  tft.setTextSize(2);
  tft.setTextFont(3);
  char buffer[10];
  uint16_t color;
  sprintf(buffer, "Att");

  if (!state.Att) {
    color = TFT_WHITE;
  } else {
    color = TFT_RED;
  }

  tft.drawRoundRect(0, 286, 70, 34, 5, color);
  tft.fillRoundRect(1, 287, 68, 32, 5, TFT_BLACK);
  tft.setTextColor(color);
  tft.drawString(buffer, 16, 286, 2);
}

void printPreamp() {
  tft.setTextSize(2);
  tft.setTextFont(3);
  char buffer[10];
  uint16_t color;
  sprintf(buffer, "Preamp");

  if (!state.Preamp) {
    color = TFT_WHITE;
  } else {
    color = TFT_RED;
  }

  tft.drawRoundRect(76, 286, 120, 34, 5, color);
  tft.fillRoundRect(77, 287, 118, 32, 5, TFT_BLACK);
  tft.setTextColor(color);
  tft.drawString(buffer, 92, 286, 2);
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
  if (state.SquelchLevel < 220) //max value that affects SQL bar
  {
    state.SquelchLevel++;
  }
}

void SQLDown() {
  if (state.SquelchLevel > 0) {
    state.SquelchLevel--;
  }
}

void volUp() {
  if (state.AudioVolume < 220) //max value that affects Volume bar
  {
    state.AudioVolume++;
  }
}

void volDown() {
  if (state.AudioVolume > 0) {
    state.AudioVolume--;
  }
}

void mainEncInc() {
  uint32_t factor = castStep();
  if(state.CenterFrequency+factor <= 3000000000){
    state.CenterFrequency += factor;
  }else{
    state.CenterFrequency = 3000000000;
  }
}

void mainEncDec() {
  uint32_t factor = castStep();
  if(state.CenterFrequency - factor > 0 && state.CenterFrequency - factor < 3000000000){
    state.CenterFrequency -= factor;
  }
}

long int castStep() {
  unsigned long long int factor = 0;
  switch (state.Step) {
    case 16:
      factor = 1;
      break;
    case 32:
      factor = 10;
      break;
    case 64:
      factor = 100;
      break;
    case 128:
      factor = 1000;
      break;
    case 256:
      factor = 10000;
      break;
    case 512:
      factor = 100000;
      break;
    case 1024:
      factor = 1000000;
      break;
    case 2048:
      factor = 10000000;
      break;
    case 4096:
      factor = 100000000;
      break;
    case 8192:
      factor = 1000000000;
      break;
    case 16384:
      factor = 10000000000;
      break;
    case 32768:
      factor = 100000000000;
      break;
  }

  return factor;
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
