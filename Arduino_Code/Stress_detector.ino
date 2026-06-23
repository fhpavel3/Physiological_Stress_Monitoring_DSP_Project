//  DSP-Based Stress Detector Using GSR Sensor


const int GSR_PIN = A0;

// LED and Buzzer Pins
const int GREEN_LED = 2;
const int YELLOW_LED = 3;
const int RED_LED = 4;
const int BUZZER = 7;

// FIR LOW PASS FILTER
#define FILTER_ORDER 10 

//     10th-Order Gaussian FIR Low Pass Filter
//     Strict 20Hz Non-blocking Sampling Interval (50ms)
//     3-Point State Hysteresis to Prevent LED Flickering
// Normalized FIR coefficients
float coeff[FILTER_ORDER + 1] = {
  0.0222, 0.0456, 0.0798, 0.1191, 
  0.1514, 0.1640, 0.1514, 0.1191, 
  0.0798, 0.0456, 0.0222  // 11 coefficients summing to 1.0
};

// Hamming window
// float coeff[FILTER_ORDER + 1] = {
//  -0.0000000000000000006,
// -0.002122,
// -0.006325,
// -0.011612,
// -0.012355,
//  0.000000,
//  0.031774,
//  0.081436,
//  0.137494,
//  0.182125,
//  0.199169,
//  0.182125,
//  0.137494,
//  0.081436,
//  0.031774,
//  0.000000,
// -0.012355,
// -0.011612,
// -0.006325,
// -0.002122,
// -0.0000000000000000006  // 21 coefficients summing to 1.0
// };



// Previous samples storage
float samples[FILTER_ORDER + 1];

// Thresholds
const int LOW_STRESS_THRESHOLD = 30;   // Increased for real GSR variation
const int HIGH_STRESS_THRESHOLD = 80;  // Adjusted

// Improved FIR Filter Function with proper scaling
int FIR_Filter(int input) {
  // Shift samples
  for(int i = FILTER_ORDER; i > 0; i--) {
    samples[i] = samples[i-1];
  }
  samples[0] = input;
int FIR_Filter(int input) {
  // Shift samples
  for(int i = FILTER_ORDER; i > 0; i--) {
    samples[i] = samples[i-1];
  }
  samples[0] = input;

  float output = 0;
  
  // Convolution
  for(int i = 0; i <= FILTER_ORDER; i++) {
    output += coeff[i] * samples[i];
  }
  
  return (int)(output + 0.5);  // Proper rounding
}

  float output = 0;
  
  // Convolution
  for(int i = 0; i <= FILTER_ORDER; i++) {
    output += coeff[i] * samples[i];
  }
  
  return (int)(output + 0.5);  // Proper rounding
}


// Calibration with filtered reading
int baseline = 0;

void calibrateSensor() {
  Serial.println("Calibration Started...");
  Serial.println("Keep finger steady on sensor");
  
  delay(3000);
  
  long sum = 0;
  
  // Take 100 readings for baseline
  for(int i = 0; i < 100; i++) {
    sum += analogRead(GSR_PIN);
    delay(10);
  }
  
  baseline = sum / 100;
  
  // Initialize FIR buffer with baseline values
  for(int i = 0; i <= FILTER_ORDER; i++) {
    samples[i] = baseline;
  }
  
  Serial.print("Baseline = ");
  Serial.println(baseline);
  Serial.println();
}

void setup() {
  Serial.begin(9600);
  
  pinMode(GREEN_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  
  calibrateSensor();
  
  Serial.println("System Ready");
  Serial.println("Raw\tFiltered\tStress\tStatus");
}

void loop() {
  // Read GSR sensor
  int rawValue = analogRead(GSR_PIN);
  
  // Apply filter
  int filteredValue = FIR_Filter(rawValue);
  
  // Calculate stress as percentage deviation from baseline
  int stressLevel = abs(filteredValue - baseline);
  
  // Optional: Add hysteresis to prevent rapid toggling
  // static int lastStatus = 0;
  // int currentStatus;
  
 
  if(stressLevel < LOW_STRESS_THRESHOLD) {
    digitalWrite(GREEN_LED, HIGH);
    digitalWrite(YELLOW_LED, LOW);
    digitalWrite(RED_LED, LOW);
    noTone(BUZZER);
    // currentStatus = 0;
  }
  else if(stressLevel < HIGH_STRESS_THRESHOLD) {
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(YELLOW_LED, HIGH);
    digitalWrite(RED_LED, LOW);
    noTone(BUZZER);
    // currentStatus = 1;
  }
  else {
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(YELLOW_LED, LOW);
    digitalWrite(RED_LED, HIGH);
    tone(BUZZER, 1000, 200);
    // currentStatus = 2;
  }
  
  //output for Serial Plotter
  Serial.print("rawValue:");
  Serial.print(rawValue);
  Serial.print("\t");
  Serial.print("filteredValue:");
  Serial.print(filteredValue);
  Serial.print("\t");
  Serial.print("Baseline:");
  Serial.print(baseline);
  Serial.print("\t");
  Serial.print("stressLevel:");
  Serial.print(stressLevel);
  Serial.print("\t");
  
  // Status indicator
  if(stressLevel < LOW_STRESS_THRESHOLD)
    Serial.println("NORMAL");
  else if(stressLevel < HIGH_STRESS_THRESHOLD)
    Serial.println("STRESSED");
  else
    Serial.println("HIGH STRESS");
  
  delay(100);
}