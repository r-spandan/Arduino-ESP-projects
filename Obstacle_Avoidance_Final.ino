#include <Servo.h>

// Ultrasonic sensor pins
#define Echo 3
#define Trig 2

// Servo pin
#define servoPin 5
#define Speed 180  // Increased for more power
#define spoint 90

// Variables
int distance;
Servo servo;

// Motor Driver Pins - TANK STYLE CONFIGURATION
// Left motors (connected together)
#define LeftMotor_ENA 6   // Enable left motors
#define LeftMotor_IN1 7   // Left motors forward
#define LeftMotor_IN2 8   // Left motors backward

// Right motors (connected together)
#define RightMotor_ENB 11  // Enable right motors
#define RightMotor_IN3 9   // Right motors forward
#define RightMotor_IN4 10  // Right motors backward

// Turning variables
int leftDist, rightDist;

void setup() {
  // Ultrasonic sensor setup
  pinMode(Trig, OUTPUT);
  pinMode(Echo, INPUT);
  
  // Servo setup
  servo.attach(servoPin);
  servo.write(spoint);
  delay(500);
  
  // Motor driver setup
  pinMode(LeftMotor_ENA, OUTPUT);
  pinMode(LeftMotor_IN1, OUTPUT);
  pinMode(LeftMotor_IN2, OUTPUT);
  pinMode(RightMotor_ENB, OUTPUT);
  pinMode(RightMotor_IN3, OUTPUT);
  pinMode(RightMotor_IN4, OUTPUT);
  
  // Set motor speed - INCREASED for more torque
  analogWrite(LeftMotor_ENA, Speed);
  analogWrite(RightMotor_ENB, Speed);
  
  delay(1000);
}

void loop() {
  distance = ultrasonic();
  
  if (distance <= 20) {
    Stop();
    delay(300);
    
    // Move backward a bit
    backward();
    delay(800);
    Stop();
    delay(300);
    
    // Check left and right distances
    leftDist = leftsee();
    delay(300);
    rightDist = rightsee();
    delay(300);
    servo.write(spoint);
    delay(300);
    
    // DECISION MAKING WITH MORE TURNING POWER
    if (leftDist > 30 && rightDist > 30) {
      // Both paths clear, choose the clearer one
      if (leftDist > rightDist) {
        pivotLeftAlt();  // Using alternative turning method
        delay(1000);
      } else {
        pivotRightAlt();  // Using alternative turning method
        delay(1000);
      }
    } else if (leftDist > 25) {
      pivotLeftAlt();  // Using alternative turning method
      delay(1200);
    } else if (rightDist > 25) {
      pivotRightAlt();  // Using alternative turning method
      delay(1200);
    } else {
      // Both paths blocked, turn around with more power
      pivotRightAlt();  // Using alternative turning method
      delay(1500);
    }
    
    Stop();
    delay(300);
  } else {
    forward();
  }
}

int ultrasonic() {
  digitalWrite(Trig, LOW);
  delayMicroseconds(2);
  digitalWrite(Trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(Trig, LOW);
  
  long duration = pulseIn(Echo, HIGH, 30000);
  if (duration == 0) return 200;
  
  int dist = duration * 0.034 / 2;
  return (dist > 200 || dist < 2) ? 200 : dist;
}

int rightsee() {
  servo.write(30);
  delay(600);
  int dist = ultrasonic();
  delay(100);
  return dist;
}

int leftsee() {
  servo.write(150);
  delay(600);
  int dist = ultrasonic();
  delay(100);
  return dist;
}

void forward() {
  digitalWrite(LeftMotor_IN1, HIGH);
  digitalWrite(LeftMotor_IN2, LOW);
  digitalWrite(RightMotor_IN3, HIGH);
  digitalWrite(RightMotor_IN4, LOW);
}

void backward() {
  digitalWrite(LeftMotor_IN1, LOW);
  digitalWrite(LeftMotor_IN2, HIGH);
  digitalWrite(RightMotor_IN3, LOW);
  digitalWrite(RightMotor_IN4, HIGH);
}

// Standard pivot functions (one side forward, one side backward)
void pivotRight() {
  digitalWrite(LeftMotor_IN1, HIGH);
  digitalWrite(LeftMotor_IN2, LOW);
  digitalWrite(RightMotor_IN3, LOW);
  digitalWrite(RightMotor_IN4, HIGH);
}

void pivotLeft() {
  digitalWrite(LeftMotor_IN1, LOW);
  digitalWrite(LeftMotor_IN2, HIGH);
  digitalWrite(RightMotor_IN3, HIGH);
  digitalWrite(RightMotor_IN4, LOW);
}

// Alternative turning functions (one side forward, one side STOPPED)
void pivotRightAlt() {
  // Left motors forward, Right motors STOP
  digitalWrite(LeftMotor_IN1, HIGH);
  digitalWrite(LeftMotor_IN2, LOW);
  digitalWrite(RightMotor_IN3, LOW);
  digitalWrite(RightMotor_IN4, LOW);
}

void pivotLeftAlt() {
  // Right motors forward, Left motors STOP
  digitalWrite(LeftMotor_IN1, LOW);
  digitalWrite(LeftMotor_IN2, LOW);
  digitalWrite(RightMotor_IN3, HIGH);
  digitalWrite(RightMotor_IN4, LOW);
}

void Stop() {
  digitalWrite(LeftMotor_IN1, LOW);
  digitalWrite(LeftMotor_IN2, LOW);
  digitalWrite(RightMotor_IN3, LOW);
  digitalWrite(RightMotor_IN4, LOW);
}