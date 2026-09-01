//Instrument Pitch Tuner - Lucas Pop - 08/18/2026
//Analyses sounds and displays notes through fast fourier transformations of microphone inputs

//Include libraries for frequency transformations (FFT)
#include <Arduino.h>
#include <arduinoFFT.h>

//Define notes for display and the reference frequency F0 at 440Hz or A4
const String notes[12]{"A", "Bb", "B", "C", "Db", "D", "Eb", "E", "F", "Gb", "G", "Ab"};
const float rfreq = 440.0;
int noteOctave = 4;

//Variables for frequency consistency checking
float lastFreq = 0.0;
int consistentCount = 0;
const int requiredConsistentReads = 5; // Adjust for more stricter note consistency
const float freqTolerance = 5.0; // Allowed Hz difference to be considered the same note
const float noiseAmplitudeThreshold = 20.0; // Adjust based on your mic's noise level

//Define the sampling amount for FFT analysis (must be a power of 2)
const uint16_t samples = 128;

//Create arrays to hold frequency components for FFT analysis
float vReal[samples];
float vImag[samples];

//Define microphone pin and default input values for microphone sensor
const int micPin = A0;
int micValue = 0;

//Stratup Serial Monitor
void setup(){
  Serial.begin(9600);
}

void loop() {

  //Define time for obtaining sampling rate  
  unsigned long t0 = micros();

  // Collect samples of microphone input audio
  float sumSquares = 0;
  for (int i = 0; i < samples; i++) {
    float val = analogRead(micPin) - 256;  // Read the microphone input with an offset of 1.25 DC voltage bias
    vReal[i] = val;
    vImag[i] = 0.0;
    sumSquares += (val * val);
  }
  
  // Calculate root mean square (RMS) to determine audio amplitude and compare to minimum amplitude
  float rms = sqrt(sumSquares / samples);
  if (rms < noiseAmplitudeThreshold) {
    consistentCount = 0;
    return;
  }

  //Compute sampling rate of analogRead loop
  unsigned long t1 = micros();
  float realFs = samples * 1e6 / (t1 - t0);

  //Construct FFT variables to use in FFT functions
  ArduinoFFT<float> FFT(vReal, vImag, samples, realFs);

  //Compute samples into components of most common note frequency
  FFT.dcRemoval();
  FFT.windowing(FFTWindow::Hamming, FFTDirection::Forward);
  FFT.compute(FFTDirection::Forward);
  FFT.complexToMagnitude();
  float x = FFT.majorPeak();

  // Check if the frequency is consistent with previous readings
  if (abs(x - lastFreq) < freqTolerance) {
    consistentCount++;
  } else {
    consistentCount = 0;
  }
  lastFreq = x;

  // Only proceed if the frequency has been consistent for a few frames
  if (consistentCount < requiredConsistentReads) {
    return;
  }

  //Compute note semitone relative to reference frequency
  float noteFloat = 12 * ((log(x / rfreq)) / (log(2)));

  //Compute innacuracy
  int noteImpact = 0; 
  float cents = 100 * (noteFloat - int(noteFloat));
  if (cents > 50) {
    cents = cents - 100;
    noteImpact = 1; //Note is sharper
  } else if (cents < -50) {
    cents = cents + 100;
    noteImpact = -1; //Note is flatter
  }

  //Calculate octave value using the specific reference note A4
  int note = int(noteFloat);
  if (note/3 >= 1) {
    noteOctave = (4 + 1 + ((note - 3) / 12)); /*Any note 3 semitones higher or more is at least one octave above
    every twelve semitones is another octave more*/
  } else if (note/9 <= -1) {
    noteOctave = (4 - 1 + ((note + 9) / 12)); /*Any note 9 semitones lower or less is at least one octave below
    every twelve semitones is another octave less*/
  } else {
    noteOctave = 4;
  }

  //Convert note to its index in the list
  if (note < 0) {
    note = ((note % 12) + 12) % 12 + noteImpact; //Negative semitones are converted to their equivalent positive index
  } else {
    note = note % 12 + noteImpact;
  }

  //Display note letter, octave, and inaccuracy with delay for reading ability
  String sign = (cents >= 0) ? "+" : "-";
  Serial.println(notes[note] + String(noteOctave) + " " + sign + String(abs((int)cents)));
  delay(300);
}