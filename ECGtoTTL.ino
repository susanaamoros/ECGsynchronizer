// Steps to adjust the threshold value:
// 1. Set-up the Heart Rate (beats/min) you expect to work with
//    in the ECG parameters section
// 2. Adjust the transformation you want to perform during 1 period
//    in the Code variables section, x variable (log10(final value) - log10(max value))

// Input and Output pins
int inputPin = A0; // Pin where the ECG will be read
int outputPin = 4; // Pin where the TTL will be generated

// ECG parameters
float HeartRate = 60; // HeartRate in beats/min

// Code variables
int analogValue[2]; // Variable to store 2 ECG samples
int der1Value = 0; // Variable to store the der between the 2 ECG samples
float threshold = 10; // Variable to trigger the HIGH level of the TTL
int SamplRate = 1 / (5.26 * 1E-3); // dt (ms) between consecutive samples from the ECG significantly reduces the SamplRate
//float x = (1 - pow(10, (log10(17) - log10(23)) / (SamplRate*HeartRate / 60))); // REAL ECG
float x = (1 - pow(10, (log10(23) - log10(30)) / (SamplRate*HeartRate / 60))); // DEMO ECG

// Time variables
// Delays
int dtMultiParam = 25; // Delay (ms) introduced by the multi-parametric
int dt = 5; // Time distance between consecutive samples (ms)
// Millis
unsigned long t0 = 0;
unsigned long t00 = 0;
unsigned long tTTL = 0;
unsigned long Tms = 0;
unsigned long T = 0;

void setup() {
  Serial.begin(9600);
  pinMode(outputPin, OUTPUT);
  pinMode(inputPin, INPUT);
}

void loop() {
  // Store when the new iteration starts
  unsigned long t = millis();

  // Return TTL value to 0 V if it has been HIGH for more than 25 ms
  if ( t - tTTL >= 25) {
    digitalWrite(outputPin, LOW);
  }

  // Acquire two samples at dt (ms) from each other
  analogValue[0] = analogRead(A0);
  delay(dt);
  analogValue[1] = analogRead(A0);

  // Compute the difference between them
  der1Value = analogValue[1] - analogValue[0];

  // Compute the heart rate & update the threshold
  if (der1Value > threshold && t - t0 > 20) {
    t00 = t0;
    t0 = millis();
    Tms = t0 - t00; // T (ms)
    T = 60 / (Tms * 1E-3); // Heart rate (beats/min)
    Serial.print("Heart rate: "); Serial.println(T);
    //Serial.print("B:"); Serial.println(threshold);
    threshold = der1Value;
    //Serial.print("A:"); Serial.println(threshold);
  } else {
    threshold = threshold * (1 - x);
  }

  // Generate the TTL output signal
  if ( t - t0 > Tms - dtMultiParam) {
    tTTL = millis();
    digitalWrite(outputPin, HIGH);
  }
}
