/* SDRuno Bridge plugin mocking firmware v0.1
 *
 */

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
  long int    VfoFrequency            = 0; //VFO Freq,                Hz
  long int    CenterFrequency         = 443300000; //Center Frequency,        Hz
  long int    SP1MinFrequency         = 0;
  long int    SP1MaxFrequency         = 0;
  long int    MPXLevel                = 0;
  int         FilterBandwidth         = 4300; //Filter bandwith,         Hz
  int         SquelchLevel            = 0;
  bool        SquelchEnable           = false;
  bool        FmNoiseReductionEnable  = false;
  bool        AudioMute               = false;
  bool        BiasTEnable             = false;
  long int    fingerprint;
} state;

long lastRandomSent;

void setup() {
  Serial.begin(9600);
  State state;
}

void loop() {
  state.fingerprint = random();

  lastRandomSent = state.fingerprint;
  updateDemodulator();
//  updateVfoFrequency();
  updateCenterFrequency();
  updateFilterBandwidth();

  echoStruct();

  delay(1000); //wait 1000 mS
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
  sscanf(str, "%hu %lu %lu %hu %lu",
         &state.Demodulator,
         &state.VfoFrequency,
         &state.CenterFrequency,
         &state.FilterBandwidth,
         &state.fingerprint);
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

void updateVfoFrequency() {
  if (state.VfoFrequency >= 3654320 && state.VfoFrequency <= 456300000) // ~7,6 MHz <= VFO <= 443,3 MHz
  {
    state.VfoFrequency += 1;
  } else if (state.VfoFrequency = 456300000) {
    state.VfoFrequency = 3654320;
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
