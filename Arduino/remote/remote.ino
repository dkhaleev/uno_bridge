/* SDRuno Bridge plugin mocking firmware v0.1
 *
 */

struct __attribute__((__packed__)) State {
  int8_t      DT      = 0; //Demodulator type,        Enum:
  //0 -
  //1 -
  //2 -
  //3 -
  //...
  //x -
  long int    VFOFreq = 3654320; //VFO Freq,                Hz
  long int    CFreq   = 443300000; //Center Frequency,        Hz
  int         FB      = 4300; //Filter bandwith,         Hz
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
  updateDT();
  updateVFOFreq();
  updateCFreq();
  updateFB();

  //  Serial.write((byte*)&state, sizeof(state));
  Serial.print(state.DT);
  Serial.print("\t");
  Serial.print(state.VFOFreq);
  Serial.print("\t");
  Serial.print(state.CFreq);
  Serial.print("\t");
  Serial.print(state.FB);
  Serial.print("\t");
  Serial.print(state.fingerprint);
  Serial.println("");

  delay(1000); //wait 1000 mS
  String str = Serial.readStringUntil('\n');
  
  if (str.length() > 0) {
    parseStruct(str);
    Serial.println("Read value");
    Serial.print(state.DT);
    Serial.print("\t");
    Serial.print(state.VFOFreq);
    Serial.print("\t");
    Serial.print(state.CFreq);
    Serial.print("\t");
    Serial.print(state.FB);
    Serial.print("\t");
    Serial.print(state.fingerprint);
    Serial.println("");
  }
}

void parseStruct(String string) {
  const char *str = string.c_str();
  sscanf(str, "%hu %lu %lu %hu %lu",
         &state.DT,
         &state.VFOFreq,
         &state.CFreq,
         &state.FB,
         &state.fingerprint);
}

void updateDT() {
  if (state.DT < 6)
  {
    state.DT += 1;
  } else {
    state.DT = 0;
  }
}
void updateCFreq() {
  if (state.CFreq >= 3654320 && state.CFreq <= 456300000) // ~7,6 MHz <= VFO <= 443,3 MHz
  {
    state.CFreq -= 1;
  } else if (state.CFreq = 3654320) {
    state.CFreq = 3654320;
  }
}

void updateVFOFreq() {
  if (state.VFOFreq >= 3654320 && state.VFOFreq <= 456300000) // ~7,6 MHz <= VFO <= 443,3 MHz
  {
    state.VFOFreq += 1;
  } else if (state.VFOFreq = 456300000) {
    state.VFOFreq = 3654320;
  }
}

void updateFB() {
  if (state.FB >= 3300 && state.FB <= 20000) // 4,3 KHz <= FB <= 20 KHz
  {
    state.FB += 1;
  } else if (state.FB = 20000) {
    state.FB = 4300;
  }
}
