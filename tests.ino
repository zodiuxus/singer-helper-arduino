unsigned int sample;
int readings = 1000000;
void getReadings() {
  newTime = micros();
  int micIn;

  Serial.println("Reading...");
  for (int i = 0; i < readings; i++) {
    micIn = analogRead(MIC);
  }

  float conversionTime = (micros() - newTime)/1000000.0;
  Serial.print(F("conversion time: "));
  Serial.print(conversionTime);
  Serial.println(F("uS"));

  Serial.print(F("Max sampling frequency: "));
  Serial.print((1.0/conversionTime)*readings);
  Serial.println(F("Hz"));
}

const int sampleWindow = 50;
unsigned long startMillis;
unsigned int peakToPeak = 0;
unsigned int signalMax = 0;
unsigned int signalMin = 1024;

int peakToPeakF(){
  startMillis = millis();

  while (millis() - startMillis < sampleWindow) {
    sample = analogRead(MIC);
    if (sample < 1024) {
      if (sample > signalMax) {
        signalMax = sample;
      }
      else if (sample < signalMin) {
        signalMin = sample;
      }
    }
  }

  return signalMax - signalMin;
}

void setup() {
  matrix.begin();
  matrix.setTextWrap(false);
  matrix.setBrightness(20);
  matrix.setTextColor(matrix.Color(16, 160, 255));

  Serial.begin(9600);
}

void loop() {
  getReadings(); // we get the max frequency for the microphone from this
  int peaktoPeak = peakToPeakF();
  Serial.println(peakToPeak); // use this if it's connected to a PC
  matrix.print(peakToPeak); // otherwise use this to view it on the matrix, however you're going to have to disable the matrix functions at the bottom
  delay(100);
}