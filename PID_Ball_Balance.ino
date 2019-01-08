// PID Ball Beam Balance
// Jugal & Nathan
// 2018-11-29

// Variables

int triggerPin = 5;
int echoPin = 4;

int servoPin = 10;


float duration;
float distance;

double target = 15;

float PIDCorrection = 0;
int maxCorrection = 40;
int minCorrection = -40;
int servoCorrection = 0;

int errors[5];
int errorAverage = 0;

// P Variables
float Wp = 3;
double Error = 0;
float Cp;

// I Variables
float Wi = 0.001;
double cumError = 0;
float Ci;

// D Variables
float Wd = -2;
double slopeError = 0;
double lastError = 0;

float Cd;

// Setting up pin modes and establishing a link to the arduino
void setup() {
  Serial.begin(9600);
  pinMode(triggerPin, OUTPUT);
  pinMode(echoPin, INPUT);

}


// Code that arduino runs on loop
void loop() {
  // Ultrasonic sensor pinging
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);

  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);

  digitalWrite(triggerPin, LOW);

  // Distance calculation based on ping time
  duration = pulseIn(echoPin, HIGH);
  distance = duration / 58.8235;
  Serial.println(distance); //Printing distance to the computer

  //-------------------------------------
  if (distance < 40) {        // Limiting max distance so that spikes are avoided in the data
    Error = target - distance;  //error calculation

    errors[4] = errors[3];      // Array holding the last 5 error values
    errors[3] = errors[2];
    errors[2] = errors[1];
    errors[1] = errors[0];
    errors[0] = Error;

    errorAverage = (errors[0] + errors[1] + errors[2] + errors[3] + errors[4]) / 5;   // Averaging out error to get a more smooth response 
    Cp = Wp * errorAverage;       // P Value

    cumError = cumError + errorAverage;   // Cumalative Value
    Ci = Wi * cumError;       // I Value

    slopeError = errorAverage - lastError;  // Derivative
    lastError = errorAverage;             // Setting last error to calculate slope next loop 
    Cd = Wd * slopeError; // D Value

    if (Ci > 20) {          // Capping out I Value for anti Windup 
      Ci = 20;
    }
    if (Ci < -20) {         // ^ Same
      Ci = -20;
    }
  
          
    PIDCorrection = Cp + Ci + Cd;       // Adding up values for correction

    if (PIDCorrection < -30) {          // Capping PID Correction so that its in range of the servo
      PIDCorrection = -30;
    }
    if (PIDCorrection > 30) {           // ^ Same
      PIDCorrection = 30;
    }
    Serial.println(175 - PIDCorrection);    // Printing servo signal for debug
  }
  analogWrite(servoPin, 175 - PIDCorrection);    // Setting Servo position 

  delay(10);
}


