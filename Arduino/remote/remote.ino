/* SDRuno Bridge plugin mocking firmware v0.1
 *
 */
#include <TimerOne.h>


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
  int         Step                    = 16;         //trailing four bits are reserved for aux and does not mapped to step place highlighter
  bool        VfoMode                 = false;      //
  
  long int    fingerprint;                          //hash-like substitute. @ToDo: rework me later
} state;

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

long lastRandomSent;

void setup() {
  Serial.begin(9600);
  State state;

  //LED pins setup
  pinMode(data_pin,         OUTPUT);
  pinMode(bit_clock_pin,    OUTPUT);
  pinMode(digit_clock_pin,  OUTPUT);

  //Timer interrupt for display freq;
  Timer1.initialize(1000000); //timing for 1s
  Timer1.attachInterrupt(fillRegisters);
}

void loop() {
  state.fingerprint = random();

  lastRandomSent = state.fingerprint;
  updateDemodulator();
  updateVfoFrequency();
  updateCenterFrequency();
  updateFilterBandwidth();
  updateStep();
  updateMute();
  updateBiasT();
  updateMode();

  echoStruct();

  //  delay(1000); //wait 1000 mS
  String str = Serial.readStringUntil('\n');

  //  if (str.length() > 0) {
  //    parseStruct(str);
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
  //  }
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
  Serial.print("\t");
  
//  Serial.print(state.fingerprint);
  Serial.println("");
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
  
void fillRegisters() {
  int array[12];
  int temporaryStep = state.Step;
  long int number = state.CenterFrequency;

  digitalWrite(digit_clock_pin, LOW);

  //highlight mute button
  if(state.AudioMute){
    bitSet(temporaryStep, 3);
  }else{
    bitClear(temporaryStep, 3);
  }

  //highlight bias-T button
  if(state.BiasTEnable){
    bitSet(temporaryStep, 2);
  }else{
    bitClear(temporaryStep, 2);
  }

  //highlight VFO Mode button
  if(state.VfoMode){
    bitSet(temporaryStep, 1);  
  }else{
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
  print_binary(temporaryStep, 16);

  digitalWrite(digit_clock_pin, HIGH);
}

void updateDemodulator() {
  if (state.Demodulator < 13)
  {
    state.Demodulator += 1;
  } else {
    state.Demodulator = 0;
  }
}

void updateCenterFrequency() {
  if (state.CenterFrequency >= 3654320 && state.CenterFrequency <= 456300000) // ~7,6 MHz <= VFO <= 443,3 MHz
  {
    state.CenterFrequency -= 1;
  } else if (state.CenterFrequency = 3654320) {
    state.CenterFrequency = 3654320;
  }
}

void updateStep() {
  state.Step = state.Step << 1;
  if(state.Step == 0)
  {
    state.Step = 16; //4 bits reserved for buttons
  }  
}

void stepUp(){}
void stepDown(){}
void updateVfoFrequency() {
  if (state.VfoFrequency >= 3654320 && state.VfoFrequency <= 456300000) // ~7,6 MHz <= VFO <= 443,3 MHz
  {
    state.VfoFrequency += 1;
  } else if (state.VfoFrequency = 456300000) {
    state.VfoFrequency = 3654320;
  }
}

void updateMute(){
  //dummy code
  if(state.CenterFrequency%2==0){
    state.AudioMute = true;
  } else {
    state.AudioMute = false;
  }  
}

void updateBiasT(){
  //dummy code
  if(state.CenterFrequency%2==1){
    state.BiasTEnable = true;
  }else{
    state.BiasTEnable = false;
  }
}

void updateMode(){
  //dummy code
  if(state.CenterFrequency%3==0){
    state.VfoMode = true;
  }else{
    state.VfoMode = false;
  }
}
  
void updateFilterBandwidth() {
  if (state.FilterBandwidth >= 3300 && state.FilterBandwidth <= 20000) // 4,3 KHz <= FilterBandwidth <= 20 KHz
  {
    state.FilterBandwidth += 1;
  } else if (state.FilterBandwidth = 20000) {
    state.FilterBandwidth = 4300;
  }
}


//service helper methods
void print_binary(int v, int num_places)
{
    int mask=0, n;

    for (n=1; n<=num_places; n++)
    {
        mask = (mask << 1) | 0x0001;
    }
    v = v & mask;  // truncate v to specified number of places

    while(num_places)
    {

        if (v & (0x0001 << num_places-1))
        {
             Serial.print("1");
        }
        else
        {
             Serial.print("0");
        }

        --num_places;
        if(((num_places%4) == 0) && (num_places != 0))
        {
            Serial.print("_");
        }
    }
}
