/**
 * Project: ESP32 Smart Digital Scale
 * Description: Non-blocking digital weight scale using ESP32, HX711 24-bit ADC, 
 *              5kg Strain Gauge Load Cell, and 0.96" I2C SSD1306 OLED Display.
 * Author: Shohidul Islam Remon
 */

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "HX711.h"

// --- Pin Definitions ---
#define HX711_DOUT_PIN 17  // HX711 DT  connected to ESP32 GPIO 17 (TX2)
#define HX711_SCK_PIN  16  // HX711 SCK connected to ESP32 GPIO 16 (RX2)
#define OLED_SDA_PIN   21  // OLED SDA  connected to ESP32 GPIO 21
#define OLED_SCL_PIN   22  // OLED SCL  connected to ESP32 GPIO 22
#define BUZZER_PIN     25  // Buzzer (+) connected to ESP32 GPIO 25

// --- OLED Configuration ---
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

HX711 scale;

// --- Calibration & Measurement Variables ---
float calibration_factor = 420.0; // Adjust based on known calibration weight
float weight = 0.0;
unsigned long last_display_time = 0;
const unsigned long REFRESH_INTERVAL_MS = 250; // Synced with HX711 10Hz sample rate

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\n=== ESP32 DIGITAL SCALE STARTING ===");

  // Initialize Buzzer (Power-on short beep)
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, HIGH);
  delay(100);
  digitalWrite(BUZZER_PIN, LOW);

  // Initialize I2C OLED Display
  Wire.begin(OLED_SDA_PIN, OLED_SCL_PIN);
  if (display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
    display.clearDisplay();
    display.setTextColor(WHITE);
    display.setTextSize(1);
    display.setCursor(20, 25);
    display.println("Scale Ready...");
    display.display();
  } else {
    Serial.println("[ERROR] SSD1306 OLED allocation failed!");
  }

  // Initialize HX711 Load Cell Amplifier
  scale.begin(HX711_DOUT_PIN, HX711_SCK_PIN);
  scale.set_scale(calibration_factor);

  // Non-blocking Tare on startup
  if (scale.wait_ready_timeout(1000)) {
    scale.tare();
    Serial.println("[OK] HX711 Initialized and Zeroed (Tared).");
  } else {
    Serial.println("[ERROR] HX711 timeout during startup. Check wiring and 5V power.");
  }
}

void loop() {
  // Update display and serial telemetry every 250ms (Flicker-free)
  if (millis() - last_display_time >= REFRESH_INTERVAL_MS) {
    last_display_time = millis();

    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(25, 0);
    display.println("DIGITAL SCALE");
    display.drawLine(0, 10, 128, 10, WHITE);

    // Poll HX711 with 100ms timeout to prevent microcontroller freeze
    if (scale.wait_ready_timeout(100)) {
      weight = scale.get_units(3); // Average 3 samples for stable readout
      if (weight < 0.0) {
        weight = 0.0;
      }

      // 1. Serial Monitor Telemetry
      Serial.print("Weight: ");
      Serial.print(weight, 1);
      Serial.println(" g");

      // 2. OLED Display Output
      display.setCursor(10, 25);
      display.setTextSize(3);
      display.print(weight, 1);

      display.setTextSize(1);
      display.setCursor(105, 38);
      display.println("g");
    } else {
      // Hardware Fault Diagnostic Screen
      Serial.println("[ERROR] HX711 Disconnected!");
      display.setCursor(0, 25);
      display.setTextSize(1);
      display.println("HX711 SENSOR ERROR!");
      display.setCursor(0, 42);
      display.println("Check 5V Power/Wire");
    }

    display.display();
  }
}
