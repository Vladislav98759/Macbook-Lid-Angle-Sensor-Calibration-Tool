#include <SPI.h>
#define CS_PIN D8 // Replace with the pin number to which CS is connected
#define BUTTON_PIN 0 // Replace with the pin number to which button is connected
#define LED_BUILTIN 2 // Replace with the pin number to which led is connected
String inputString = ""; // String for storing incoming data
bool stringComplete = false;  // Flag for completion of reading
int mode = 0;
unsigned long buttonPressTime = 0;
bool buttonReleased = true;

void setup() {
  Serial.begin(9600);
  SPI.begin();
  pinMode(CS_PIN, OUTPUT);
  digitalWrite(CS_PIN, HIGH);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  inputString.reserve(200);
}
void loop() {
  digitalWrite(LED_BUILTIN, HIGH);
  int buttonState = digitalRead(BUTTON_PIN);

  if (buttonState == LOW) {
    if (buttonPressTime == 0) {
      buttonPressTime = millis();
    } else if (millis() - buttonPressTime > 1000) { 
      digitalWrite(LED_BUILTIN, LOW); 
    }
    buttonReleased = false;
  } else {
    if (!buttonReleased && buttonPressTime != 0 && millis() - buttonPressTime < 1000) {
      mode++;
      if (mode > 3) mode = 1;

      for (int i = 0; i < mode; i++) {
        digitalWrite(LED_BUILTIN, HIGH);
        delay(200);
        digitalWrite(LED_BUILTIN, LOW); 
        delay(200);
        digitalWrite(LED_BUILTIN, HIGH);
        delay(200);
      }
    } else if (!buttonReleased && buttonPressTime != 0 && millis() - buttonPressTime > 1000) {
      digitalWrite(LED_BUILTIN, HIGH); 
      if (mode == 1) {
        checkFlash();
      } else if (mode == 2) {
        checkFlash();
      } else if (mode == 3) {
        while (digitalRead(BUTTON_PIN) == HIGH) {
          sensorPosition();
        }
      }
    }
    buttonPressTime = 0;
    buttonReleased = true;
  }
  // Check for new data in the console
  if (stringComplete) {
    if (inputString.startsWith("zeroangle")) {
      zeroAngle();
    }
    else if(inputString.startsWith("calAir")) {
      calibrateAir();
    }
    else if(inputString.startsWith("calPro")) {
      calibratePro();
    }
    else if(inputString.startsWith("angle")) {
      angle();
    }
    
    // Clear data
    inputString = "";
    stringComplete = false;
  }
}

void checkFlash() {
  uint16_t originalValue, checkValue;

  // Read the original value from register 0
  digitalWrite(CS_PIN, LOW);
  SPI.transfer16((0b010 <<13) | (0 <<8)); // Read command + register address
  digitalWrite(CS_PIN, HIGH);
  delay(1);
  digitalWrite(CS_PIN, LOW);
  originalValue = SPI.transfer16(0); // Reading register value
  digitalWrite(CS_PIN, HIGH);

  // Write the original value +1 to register 0
  writeRegister(0, (originalValue & 0xFF) + 1);
  delay(1);

  // Check successful writing
  digitalWrite(CS_PIN, LOW);
  SPI.transfer16((0b010 <<13) | (0 <<8)); // Read command + register address
  digitalWrite(CS_PIN, HIGH);
  delay(1);
  digitalWrite(CS_PIN, LOW);
  checkValue = SPI.transfer16(0); // Reading register value
  digitalWrite(CS_PIN, HIGH);

  if ((checkValue &0xFF) == ((originalValue &0xFF) + 1)) {
    // If the write was successful, execute the calibratePro() or calibrateAir() function depending on the mode
    if (mode == 1) {
      calibratePro();
    } else if (mode == 2) {
      calibrateAir();
    }
  } else if ((originalValue &0xFF) != 0) {
    // If the write failed and the original value is not zero, blink the LED six times
    for(int i = 0; i < 6; i++) {
      digitalWrite(LED_BUILTIN, LOW); // Turn on the LED
      delay(100); // Pause for .1 seconds
      digitalWrite(LED_BUILTIN, HIGH); // Turn off the LED
      delay(100); // Pause for .1 seconds
    }
  } else {
    // If the write failed and the original value is zero, blink the LED three times
    for(int i = 0; i < 3; i++) {
      digitalWrite(LED_BUILTIN, LOW); // Turn on the LED
      delay(100); // Pause for .1 seconds
      digitalWrite(LED_BUILTIN, HIGH); // Turn off the LED
      delay(100); // Pause for .1 seconds
    }
  }
}

//Calibration function
void calibratePro() {
  //Basic calibrations for pro models
  writeRegister(10, 0);
  writeRegister(28, 0);
  writeRegister(29, 0);
  writeRegister(2, 0);
  writeRegister(3, 0);
  writeRegister(4, 232);
  writeRegister(5, 3);
  writeRegister(6, 1);
  writeRegister(7, 135);
  writeRegister(8, 4);
  writeRegister(9, 0);
  writeRegister(11, 248);
  writeRegister(12, 130);
  writeRegister(13, 0);
  writeRegister(14, 128);
  writeRegister(15, 201);
  writeRegister(16, 20);
  writeRegister(17, 1);
  writeRegister(18, 147);
  writeRegister(22, 1);
  writeRegister(30, 255);
  zeroAngle();
  delay(1);
  storeAllRegToNVM();
  delay(1);
  restoreAllRegFromNVM();
  delay(1);
  writeRegister(10, 0);
  storeAllRegToNVM();
  delay(100);
  checkAngle();
}

void calibrateAir() {
  //Basic calibrations for air models
  writeRegister(10, 202);
  writeRegister(28, 0);
  writeRegister(29, 0);
  writeRegister(2, 144);
  writeRegister(3, 1);
  writeRegister(4, 232);
  writeRegister(5, 3);
  writeRegister(6, 61);
  writeRegister(7, 135);
  writeRegister(8, 59);
  writeRegister(9, 130);
  writeRegister(11, 246);
  writeRegister(12, 130);
  writeRegister(13, 0);
  writeRegister(14, 128);
  writeRegister(15, 201);
  writeRegister(16, 20);
  writeRegister(17, 1);
  writeRegister(18, 147);
  writeRegister(22, 1);
  writeRegister(30, 255);
  zeroAngle();
  delay(1);
  storeAllRegToNVM();
  delay(1);
  restoreAllRegFromNVM();
  writeRegister(10, 202);
  delay(100);
  checkAngle();
}

// Function to determine the current angle
float angle(){
  uint16_t angle;
  // Reading the angular position of the sensor in uint16_t format
  digitalWrite(CS_PIN, LOW);
  angle = SPI.transfer16(0); // Reading the angle
  digitalWrite(CS_PIN, HIGH);

  float angleInDegrees = map(angle, 0, UINT16_MAX, 0, 360); // Conversion from uint16_t format to degrees

  Serial.print("Current sensor angle position: ");
  Serial.println(angleInDegrees); // Output of the angle in degrees

  return angleInDegrees;
}

// Function that blinks an LED if it thinks the lid is open.
void sensorPosition() {
  float currentAngleDegrees = angle();
  // Check sensor value
  if ((currentAngleDegrees >= 130 && currentAngleDegrees <= 360) || (currentAngleDegrees >= 0 && currentAngleDegrees <= 5)) {
    digitalWrite(LED_BUILTIN, HIGH); // Turn off the LED
  } else {
    digitalWrite(LED_BUILTIN, LOW); // Turn on the LED
    delay(10);
  }
}

// Function that performs a check after calibration and signals with an LED about the results.
void checkAngle() {
  float currentAngleDegrees = angle(); // Using the angle function to determine the current angle value

  if (round(currentAngleDegrees) == 0 || round(currentAngleDegrees) == 360) {
    digitalWrite(LED_BUILTIN, LOW); // Turn on the LED
    delay(3000); // Pause for three seconds
    digitalWrite(LED_BUILTIN, HIGH); // Turn off the LED
  } else {
    if (mode == 1) {
      calibratePro();
    } else if (mode == 2) {
      calibrateAir();
    }
  }
}
// Function for reading data from the console
void serialEvent() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    inputString += inChar;
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
}

// Function to write values to a register
void writeRegister(int registerAddress, int registerValue) {
  // Conversion of decimal value to a 16-bit format
  uint16_t registerValue16Bit = (uint16_t)registerValue;

  // Writing value to a register
  digitalWrite(CS_PIN, LOW);
  SPI.transfer16((0b100 <<13) | (registerAddress <<8) | registerValue16Bit); // Write command + register address + value
  digitalWrite(CS_PIN, HIGH);

   delay(1);

   // Checking successful writing
   digitalWrite(CS_PIN, LOW);
   SPI.transfer16((0b010 <<13) | (registerAddress <<8)); // Read command + register address
   digitalWrite(CS_PIN, HIGH);
   delay(1);
   digitalWrite(CS_PIN, LOW);
   uint16_t checkValue = SPI.transfer16(0); // Reading register value
   digitalWrite(CS_PIN, HIGH);

   if ((checkValue &0xFF) == registerValue16Bit) {
     Serial.print("Register ");
     Serial.print(registerAddress);
     Serial.println(" write successful");
   } else {
     Serial.print("Write to register ");
     Serial.print(registerAddress);
     Serial.println(" failed. Retrying...");

     // Retry writing value to a register
     digitalWrite(CS_PIN, LOW);
     SPI.transfer16((0b100 <<13) | (registerAddress <<8) | registerValue16Bit); // Write command + register address + value
     digitalWrite(CS_PIN, HIGH);

     delay(1);

     // Checking successful writing after retry
     digitalWrite(CS_PIN, LOW);
     SPI.transfer16((0b010 <<13) | (registerAddress <<8)); // Read command + register address
     digitalWrite(CS_PIN, HIGH);
     delay(1);
     digitalWrite(CS_PIN, LOW);
     checkValue = SPI.transfer16(0); // Reading register value
     digitalWrite(CS_PIN, HIGH);

     if ((checkValue &0xFF) == registerValue16Bit) {
       Serial.print("Register ");
       Serial.print(registerAddress);
       Serial.println(" write successful after retry");
     } else {
       Serial.print("Write to register ");
       Serial.print(registerAddress);
       Serial.println(" failed again");
     }
   }
}


// Function to set the current value as zero
void zeroAngle() {
  float currentAngleDegrees = angle(); // Using the angle function to determine the current angle value

  digitalWrite(CS_PIN, LOW);
  SPI.transfer16((0b010 << 13) | (0x0 << 8)); // Read command + register address
  digitalWrite(CS_PIN, HIGH);
  delay(1);
  digitalWrite(CS_PIN, LOW);
  uint16_t zeroAngleLow = SPI.transfer16(0); // Reading register value
  digitalWrite(CS_PIN, HIGH);

  digitalWrite(CS_PIN, LOW);
  SPI.transfer16((0b010 << 13) | (0x1 << 8)); // Read command + register address
  digitalWrite(CS_PIN, HIGH);
  delay(1);
  digitalWrite(CS_PIN, LOW);
  uint16_t zeroAngleHigh = SPI.transfer16(0); // Reading register value
  digitalWrite(CS_PIN, HIGH);

  uint16_t zeroAngle = (zeroAngleHigh << 8) | (zeroAngleLow & 0xFF);

  float zeroAngleDegrees = map(zeroAngle, 0, UINT16_MAX, 0, 360); // Conversion from uint16_t format to degrees

  float newZeroAngleDegrees = zeroAngleDegrees + currentAngleDegrees; // Calculation of the new zero angle

  if (newZeroAngleDegrees > 360) {
    newZeroAngleDegrees -= 360; // If the angle is greater than 360 degrees, subtracting by 360 degrees
  }

   uint16_t newZeroAngle = round((newZeroAngleDegrees / 360.0) * UINT16_MAX); // Calculation of the new zero angle in uint16_t format

   uint16_t checkValue;
   do {
     // Writing the new zero angle to the sensor registers
     digitalWrite(CS_PIN, LOW);
     SPI.transfer16((0b100 << 13) | (0x0 << 8) | (newZeroAngle & 0xFF)); // Writing the lower eight bits of the new zero angle
     digitalWrite(CS_PIN, HIGH);

     delay(1);

     digitalWrite(CS_PIN, LOW);
     SPI.transfer16((0b100 << 13) | (0x1 << 8) | ((newZeroAngle >>8) &0xFF)); // Writing the upper eight bits of the new zero angle
     digitalWrite(CS_PIN, HIGH);

     delay(1);

      // Checking successful writing and current sensor value
      digitalWrite(CS_PIN, LOW);
      SPI.transfer16((0b010 <<13) | (0x0 <<8)); // Read command + register address
      digitalWrite(CS_PIN, HIGH);
      delay(1);
      digitalWrite(CS_PIN, LOW);
      uint16_t checkValueLow = SPI.transfer16(0); // Reading register value
      digitalWrite(CS_PIN, HIGH);

      digitalWrite(CS_PIN, LOW);
      SPI.transfer16((0b010 <<13) | (0x1 <<8)); // Read command + register address
      digitalWrite(CS_PIN, HIGH);
      delay(1);
      digitalWrite(CS_PIN, LOW);
      uint16_t checkValueHigh = SPI.transfer16(0); // Reading register value
      digitalWrite(CS_PIN, HIGH);

      checkValue = (checkValueHigh <<8) | (checkValueLow &0xFF);

   } while (checkValue != newZeroAngle);

   Serial.println("Zero value set");
}



//Function to write all registers to NVM
void storeAllRegToNVM() {
    digitalWrite(CS_PIN, LOW);
    SPI.transfer16(0b110 <<13); // Command to write all registers to NVM 
    digitalWrite(CS_PIN, HIGH);
    Serial.println("All registers written to NVM");
}

//Function to restore all registers from NVM
void restoreAllRegFromNVM() {
    digitalWrite(CS_PIN, LOW);
    SPI.transfer16(0b101 <<13); // Command to restore all registers from NVM 
    digitalWrite(CS_PIN, HIGH);
    Serial.println("All registers restored from NVM");
}
