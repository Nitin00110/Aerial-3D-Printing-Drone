/*
  ExtruderControl.ino
  Lightweight firmware for aerial 3D printing extruder
  - Communicates with Raspberry Pi over Serial
  - Controls stepper motor for filament feed
  - Controls heater for hotend
  - Includes basic safety checks
*/

#include <AccelStepper.h>

// -------------------- Pin Definitions --------------------
const int dirPin = 4;       // Stepper direction pin
const int stepPin = 5;      // Stepper step pin
const int heaterPin = 10;   // MOSFET for heater cartridge
const int thermistorPin = A0; // Thermistor input pin

// -------------------- Motor Setup --------------------
AccelStepper extruderMotor(AccelStepper::DRIVER, stepPin, dirPin);

// -------------------- Temperature Config --------------------
const int minTemp = 180;   // °C - nozzle must reach before extrusion
const int maxTemp = 250;   // °C - safety cutoff
int targetTemp = 200;      // Default target temperature

// -------------------- Functions --------------------

// Dummy conversion: replace with thermistor lookup/Steinhart-Hart
int readTemp() {
  int raw = analogRead(thermistorPin);
  return map(raw, 0, 1023, 20, 300);  // approx conversion
}

void heatControl() {
  int temp = readTemp();
  if (temp < targetTemp && temp < maxTemp) {
    digitalWrite(heaterPin, HIGH);  // turn heater ON
  } else {
    digitalWrite(heaterPin, LOW);   // turn heater OFF
  }
}

// -------------------- Setup --------------------
void setup() {
  Serial.begin(115200);   // Communication with Raspberry Pi
  pinMode(heaterPin, OUTPUT);
  digitalWrite(heaterPin, LOW);

  extruderMotor.setMaxSpeed(1000);      // steps/sec
  extruderMotor.setAcceleration(500);   // steps/sec^2

  Serial.println("Extruder Control Ready");
}

// -------------------- Main Loop --------------------
void loop() {
  heatControl();

  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');

    // Extrude command: E<number>
    if (command.startsWith("E")) {
      int steps = command.substring(1).toInt();
      if (readTemp() >= minTemp) {   // Only extrude if hot
        extruderMotor.move(steps);
        while (extruderMotor.distanceToGo() != 0) {
          extruderMotor.run();
        }
        Serial.println("Extrusion Done");
      } else {
        Serial.println("Error: Nozzle not hot enough!");
      }
    }

    // Set target temperature: T<number>
    else if (command.startsWith("T")) {
      targetTemp = command.substring(1).toInt();
      Serial.print("Target Temp set to: ");
      Serial.println(targetTemp);
    }

    // Emergency Stop: S
    else if (command.startsWith("S")) {
      extruderMotor.stop();
      digitalWrite(heaterPin, LOW);
      Serial.println("Emergency Stop Activated!");
    }
  }
}
