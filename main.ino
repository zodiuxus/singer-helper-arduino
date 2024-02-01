#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>
#include <arduinoFFT.h>

#define PIN 6
#define BTN_NOTE_UP 3
#define BTN_NOTE_DOWN 2
#define MIC A6

#define SAMPLES 64
#define SAMPLING_FREQ 8928
#define AMPLITUDE 1000
#define NUM_BANDS 16
#define NOISE 130

Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(32, 8, PIN,
  NEO_MATRIX_BOTTOM     + NEO_MATRIX_RIGHT +
  NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG,
  NEO_GRB            + NEO_KHZ800);

unsigned int samplingPerioduS;
int bandValues[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
double vReal[SAMPLES];
double vImag[SAMPLES];

void setup() {
  matrix.begin();
  matrix.setTextWrap(false);
  matrix.setBrightness(20);
  matrix.setTextColor(matrix.Color(16, 160, 255));

  pinMode(PIN, OUTPUT);
  pinMode(MIC, INPUT);

  pinMode(BTN_NOTE_UP, INPUT_PULLUP);
  pinMode(BTN_NOTE_DOWN, INPUT_PULLUP);

  samplingPerioduS = round(1000000 * (1.0 / SAMPLING_FREQ));
  
}

void displayUp() {
  displayCenter();
  matrix.drawPixel(23, 2, matrix.Color(255, 80, 80));
  matrix.drawPixel(25, 2, matrix.Color(255, 80, 80));
  matrix.drawPixel(22, 3, matrix.Color(255, 80, 80));
  matrix.drawPixel(26, 3, matrix.Color(255, 80, 80));
}

void displayCenter() {
  //vertically: LEDS 1-6
  matrix.drawFastVLine(24, 1, 6, matrix.Color(255, 80, 80));
}

void displayDown() {
  displayCenter();
  matrix.drawPixel(23, 5, matrix.Color(255, 80, 80));
  matrix.drawPixel(25, 5, matrix.Color(255, 80, 80));
  matrix.drawPixel(22, 4, matrix.Color(255, 80, 80));
  matrix.drawPixel(26, 4, matrix.Color(255, 80, 80));
}

void displayLevel() {
  matrix.drawRect(20, 3, 7, 2, matrix.Color(50, 255, 96));
}

arduinoFFT FFT = arduinoFFT(vReal, vImag, SAMPLES, SAMPLING_FREQ);

int noteCounter = 0;

int freqs[16] = {131, 143, 157, 173, 189, 208, 228, 250, 274, 301, 330, 362, 397, 435, 477, 523};
char *notes[16] = {"C3", "D3", "E3b", "F3", "G3b", "A3b", "B3b", "B3", "D4b", "D4", "E4", "G4b", "G4", "A4", "B4b", "C4"};

int lastUpState = 0;
int lastDownState = 0;
int upState = 0;
int downState = 0;

unsigned long newTime;

void loop() {  
  upState = digitalRead(BTN_NOTE_UP);
  downState = digitalRead(BTN_NOTE_DOWN);

  int whichState = 0;
  
  matrix.fillScreen(0);
  matrix.setCursor(1, 0);

  for (int i = 0; i < NUM_BANDS; i++) {
    bandValues[i]=0;
  }
  
  if (upState != lastUpState) {
    if (upState == HIGH) {
      noteCounter = (noteCounter + 1) % 16;
    }
  }

  if (downState != lastDownState) {
    if (downState == HIGH) {
      if (noteCounter == 0) {
        noteCounter = 16;
      }
      noteCounter = (noteCounter - 1) % 16;
    }
  }

  lastUpState = upState;
  lastDownState = downState;

  for (int i = 0; i < SAMPLES; i++) {
    newTime = micros();
    vReal[i] = analogRead(MIC);
    vImag[i] = 0;
    while (micros() < (newTime + samplingPerioduS)) {}
  }

  FFT.DCRemoval();
  FFT.Windowing(FFT_WIN_TYP_HAMMING, FFT_FORWARD);
  FFT.Compute(FFT_FORWARD);
  FFT.ComplexToMagnitude();

  for (int i = 2; i < (SAMPLES/2); i++){
    if (vReal[i] > NOISE) {
      //16 bands, 0,52325kHz top band
      if (i<=1 )           bandValues[0]  += (int)vReal[i];
      if (i>1   && i<=1  ) bandValues[1]  += (int)vReal[i];
      if (i>1   && i<=1  ) bandValues[2]  += (int)vReal[i];
      if (i>1   && i<=1  ) bandValues[3]  += (int)vReal[i];
      if (i>1   && i<=1  ) bandValues[4]  += (int)vReal[i];
      if (i>1   && i<=2  ) bandValues[5]  += (int)vReal[i];
      if (i>2   && i<=2  ) bandValues[6]  += (int)vReal[i];
      if (i>2   && i<=2  ) bandValues[7]  += (int)vReal[i];
      if (i>2   && i<=2  ) bandValues[8]  += (int)vReal[i];
      if (i>2   && i<=2  ) bandValues[9]  += (int)vReal[i];
      if (i>2   && i<=2  ) bandValues[10]  += (int)vReal[i];
      if (i>2   && i<=3  ) bandValues[11]  += (int)vReal[i];
      if (i>3   && i<=3  ) bandValues[12]  += (int)vReal[i];
      if (i>3   && i<=3  ) bandValues[13]  += (int)vReal[i];
      if (i>3   && i<=4  ) bandValues[14]  += (int)vReal[i];
      if (i>4            ) bandValues[15]  += (int)vReal[i];
    }
  }
  matrix.print(notes[noteCounter]);

  if (bandValues[noteCounter] >= 0.95*freqs[noteCounter] && bandValues[noteCounter] <= 1.05*freqs[noteCounter]) {
    // print out a green line on the matrix
    whichState = 0;

  }
  if (bandValues[noteCounter] < 0.95*freqs[noteCounter]) {
    // print out a red arrow up on the matrix
    whichState = 1;

  }
  if (bandValues[noteCounter] > 1.05*freqs[noteCounter]) {
    // print out a red arrow down on the matrix
    whichState = 2;

  }

  if (whichState == 0) {
    displayLevel();
    matrix.show();
  }
  if (whichState == 1) {
    displayUp();
    matrix.show();
  }
  if (whichState == 2) {
    displayDown();
    matrix.show();
  }

  matrix.show();
  delay(100);
}
