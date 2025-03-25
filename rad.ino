/*This code was inspired in part by user valerioceccoli on arduino project hub
valerioceccoli's profile: https://projecthub.arduino.cc/valerioceccoli


 This example was based on the TEA5767 library published by 
  Marcos R. Oliveira at: https://github.com/mroger/TEA5767

*/

#include <EEPROM.h>
#include <Wire.h>
#include <TEA5767N.h>
#include <LiquidCrystal_I2C.h>


#define LED_PIN 3 // Pin connected to LED terminal on the LCD

LiquidCrystal_I2C lcd(0x27, 16, 2);  // Initialize LCD with address 0x27 and 16x2 size

// Constants:
TEA5767N Radio = TEA5767N();  // Using standard I2C pins A4 and A5 for the radio

// Variables:
int P1 = 2;  // Button to increase frequency
int B2 = 4;  // Button to decrease frequency
float freq = 87.60;  // Initial frequency
float prevFreq = 87.60;  // To track changes and write to EEPROM only when needed
int address = 0;  // EEPROM address (must be an integer)

void setup() {
  // Init Serial for debugging
  Serial.begin(9600);
  
  // Init LCD
  lcd.init();
  lcd.backlight();
  
  pinMode(LED_PIN, OUTPUT);
  analogWrite(LED_PIN, 50);  // Adjust LCD brightness
  
  // Initialize buttons
  pinMode(P1, INPUT_PULLUP);  // Set P1 as input with internal pull-up resistor
  pinMode(B2, INPUT_PULLUP);  // Set B2 as input with internal pull-up resistor

  // Read the stored frequency from EEPROM
  EEPROM.get(address, freq);  // Load frequency from EEPROM
  if (freq < 87.60 || freq > 108.00) freq = 87.60;  // Reset to default if out of FM range
  prevFreq = freq;  // Initialize previous frequency

  Radio.selectFrequency(freq);  // Set the radio to the stored frequency

  // Display the frequency on the LCD
  lcd.setCursor(0, 0);
  lcd.print(freq, 2);  // Display the frequency with 2 decimal points
  lcd.setCursor(6, 0);
  lcd.print(" MHz");
  
  Serial.println("System initialized. Current Frequency: " + String(freq));
}

void loop() {
  bool freqChanged = false;  // To track if frequency was modified

  // Check if the "Increase Frequency" button is pressed
  if (digitalRead(P1) == LOW) {
    freq += 0.1;  // Increase frequency by 0.1 MHz
    if (freq > 108.00) freq = 108.00;  // Limit to max FM range
    freqChanged = true;
    //delay(300);  // Delay to prevent repeated triggers
  }

  // Check if the "Decrease Frequency" button is pressed
  if (digitalRead(B2) == LOW) {
    freq -= 0.1;  // Decrease frequency by 0.1 MHz
    if (freq < 87.60) freq = 87.60;  // Limit to min FM range
    freqChanged = true;
    //delay(300);  // Delay to prevent repeated triggers
  }

  // Update frequency only if changed
  if (freqChanged) {
    Radio.selectFrequency(freq);  // Update the radio frequency

    // Update the LCD display
    lcd.setCursor(0, 0);
    lcd.print("                ");  // Clear the line
    lcd.setCursor(0, 0);
    lcd.print(freq, 2);  // Print frequency with 2 decimal places
    lcd.setCursor(6, 0);
    lcd.print(" MHz");
    
    Serial.println("Frequency adjusted to: " + String(freq));
  }

  // Write to EEPROM only if the frequency was modified and is different from the stored value
  if (freqChanged && freq != prevFreq) {
    EEPROM.put(address, freq);  // Store the updated frequency in EEPROM
    prevFreq = freq;  // Update previous frequency
    Serial.println("Frequency saved to EEPROM: " + String(freq));
  }
}

