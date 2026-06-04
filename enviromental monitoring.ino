#include <SoftwareSerial.h>
#include <DHT.h>

SoftwareSerial BT(4, 5); // RX, TX → HC-05 TX→4, HC-05 RX→5
#define DHT_PIN 6
#define MQ2_ANALOG A0
#define MQ2_DIGITAL 12
#define BUZZER_PIN 8
#define RED_LED 7

DHT dht(DHT_PIN, DHT11);

int airQualityThreshold = 300;
bool alarmEnabled = true;
float lastTemperature = NAN;
float lastHumidity = NAN;

void setup() {
  pinMode(RED_LED, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(MQ2_DIGITAL, INPUT);
  pinMode(DHT_PIN, INPUT_PULLUP);
  
  digitalWrite(RED_LED, LOW);
  digitalWrite(BUZZER_PIN, LOW);
  
  dht.begin();
  BT.begin(9600);
  Serial.begin(9600);
  
  BT.println("System Ready! Commands: T,H,M,A,S");
  BT.println("T=Temperature, H=Humidity, M=Air Quality, A=All, S=Stop alarm");
}

void readDHT() {
  for (int i = 0; i < 3; i++) {
    lastTemperature = dht.readTemperature();
    lastHumidity = dht.readHumidity();
    
    if (!isnan(lastTemperature) && !isnan(lastHumidity)) {
      return;
    }
    delay(50);
  }
  lastTemperature = NAN;
  lastHumidity = NAN;
}

void checkAirQuality() {
  int airValue = analogRead(MQ2_ANALOG);
  int smokeDetected = digitalRead(MQ2_DIGITAL);
  
  // Only activate alarm if needed
  if (alarmEnabled && (airValue > airQualityThreshold || smokeDetected == HIGH)) {
    digitalWrite(RED_LED, HIGH);
    digitalWrite(BUZZER_PIN, HIGH);
    delay(2000);
    digitalWrite(BUZZER_PIN, LOW);
    // LED stays on as warning until next reading
  } else {
    digitalWrite(RED_LED, LOW);
    digitalWrite(BUZZER_PIN, LOW);
  }
  
  // Return the values (but don't print automatically)
  return airValue;
}

void loop() {
  // NO automatic readings - only process commands
  if (BT.available() > 0) {
    char command = BT.read();
    
    switch(command) {
      case 'T': // Temperature
        readDHT();
        if (!isnan(lastTemperature)) {
          BT.print("Temperature: ");
          BT.print(lastTemperature);
          BT.println(" °C");
        } else {
          BT.println("Temperature read failed");
        }
        break;
        
      case 'H': // Humidity
        readDHT();
        if (!isnan(lastHumidity)) {
          BT.print("Humidity: ");
          BT.print(lastHumidity);
          BT.println(" %");
        } else {
          BT.println("Humidity read failed");
        }
        break;
        
      case 'M': // Air Quality - ONLY when requested
        {
          int airValue = analogRead(MQ2_ANALOG);
          int smokeDetected = digitalRead(MQ2_DIGITAL);
          BT.print("Air Quality: ");
          BT.println(airValue);
          BT.print("Smoke detected: ");
          BT.println(smokeDetected ? "YES" : "NO");
          
          // Check and activate alarm if needed
          if (airValue > airQualityThreshold || smokeDetected == HIGH) {
            digitalWrite(RED_LED, HIGH);
            digitalWrite(BUZZER_PIN, HIGH);
            delay(2000);
            digitalWrite(BUZZER_PIN, LOW);
            BT.println("WARNING: Poor air quality!");
          } else {
            digitalWrite(RED_LED, LOW);
            digitalWrite(BUZZER_PIN, LOW);
          }
        }
        break;
        
      case 'A': // All sensors
        readDHT();
        int airValue = analogRead(MQ2_ANALOG);
        int smokeDetected = digitalRead(MQ2_DIGITAL);
        
        BT.print("Status - Temp: ");
        BT.print(lastTemperature);
        BT.print("°C, Humidity: ");
        BT.print(lastHumidity);
        BT.print("%, Air: ");
        BT.print(airValue);
        BT.print(", Smoke: ");
        BT.println(smokeDetected ? "YES" : "NO");
        
        // Check and activate alarm if needed
        if (airValue > airQualityThreshold || smokeDetected == HIGH) {
          digitalWrite(RED_LED, HIGH);
          digitalWrite(BUZZER_PIN, HIGH);
          delay(2000);
          digitalWrite(BUZZER_PIN, LOW);
        } else {
          digitalWrite(RED_LED, LOW);
          digitalWrite(BUZZER_PIN, LOW);
        }
        break;
        
      case 'S': // Stop alarm
        digitalWrite(RED_LED, LOW);
        digitalWrite(BUZZER_PIN, LOW);
        BT.println("Alarm stopped");
        break;
    }
  }
  
  delay(100);
}
