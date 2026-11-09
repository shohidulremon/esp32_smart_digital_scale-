# ⚖️ ESP32 Smart Digital Scale

![Platform](https://img.shields.io/badge/Platform-ESP32-blue?style=flat-square&logo=espressif)
![Language](https://img.shields.io/badge/Language-C%2B%2B%20%2F%20Arduino-00979D?style=flat-square&logo=arduino)
![Display](https://img.shields.io/badge/Display-SSD1306%20OLED-orange?style=flat-square)
![Sensor](https://img.shields.io/badge/Sensor-HX711%2024--Bit%20ADC-green?style=flat-square)

A standalone, non-blocking digital weight scale built with an **ESP32 microcontroller**, a **5kg Strain Gauge Load Cell**, an **XFW-HX711 24-bit ADC amplifier**, and a **0.96" I2C OLED display**. Engineered for responsive real-time measurement with built-in hardware fault diagnostics.

---

## 📂 Repository Structure

```text
esp32-smart-digital-scale/
├── ESP32_Smart_Scale/
│   └── ESP32_Smart_Scale.ino   # Main firmware source code
├── README.md                   # Project documentation
├── .gitignore                  # Git ignore rules
└── LICENSE                     # MIT License
```

---

## ✨ Key Features

- **Real-Time Precision Readout:** Measures live weight in grams (`g`) using a 24-bit ADC (HX711) and renders it in large typography on a local OLED screen.
- **Non-Blocking Sensor Polling:** Uses timeout-protected polling (`wait_ready_timeout`) so loose wires or sensor disconnects never freeze the ESP32 main loop.
- **Live Hardware Diagnostics:** Automatically detects HX711 power/signal loss and alerts the user directly on the OLED screen (`HX711 SENSOR ERROR!`).
- **Flicker-Free Display Sync:** Matches the I2C OLED refresh interval (`250ms`) with the HX711's `10Hz` sampling rate to eliminate screen blinking and false disconnect triggers.
- **Auto-Tare on Boot:** Automatically zeroes out the base platform weight upon startup.
- **Dual Output:** Simultaneous live telemetry on both the 0.96" OLED screen and the PC Serial Monitor (`115200 baud`).

---

## 🛠️ Hardware Components

| Component | Specification | Quantity |
| :--- | :--- | :---: |
| **Microcontroller** | ESP32 DevKit V1 (ESP-WROOM-32, CP2102 UART Bridge) | 1 |
| **Load Cell** | 5kg Strain Gauge Cantilever Beam Load Cell | 1 |
| **ADC Amplifier** | XFW-HX711 24-Bit Precision Weight Sensor Module | 1 |
| **Display** | 0.96" I2C OLED Display (`128x64`, SSD1306, `0x3C`) | 1 |
| **Alert Module** | 5V / 3.3V Active Piezo Buzzer | 1 |
| **Connectors** | Breadboard & Jumper Wires | 1 Set |

---

## 🔌 System Wiring & Line Diagram

```text
[ 5kg Load Cell ]
       │
       ├── RED (Excitation+)   ───────────────> [ HX711: E+ ]
       ├── BLACK (Excitation-) ───────────────> [ HX711: E- ]
       ├── WHITE (Signal-)     ───────────────> [ HX711: A- ]
       └── GREEN (Signal+)     ───────────────> [ HX711: A+ ]
                                                (B- & B+ unused)
                                                     │
                                                     │ (24-Bit ADC Data & Power)
                                                     ▼
[ XFW-HX711 Module ]                            [ ESP32 DevKit V1 ]
       │                                             │
       ├── VCC  ──────────────────────────────────>  ├── [ VIN / 5V ]
       ├── GND  ──────────────────────────────────>  ├── [ GND ]
       ├── DT   ──────────────────────────────────>  ├── [ GPIO 17 (TX2) ]
       └── SCK  ──────────────────────────────────>  └── [ GPIO 16 (RX2) ]
                                                     │
                                                     │ (I2C Display Bus)
                                                     ▼
[ 0.96" OLED Display (I2C) ]                    [ ESP32 DevKit V1 ]
       │                                             │
       ├── VCC  ──────────────────────────────────>  ├── [ 3.3V ]
       ├── GND  ──────────────────────────────────>  ├── [ GND ]
       ├── SDA  ──────────────────────────────────>  ├── [ GPIO 21 ]
       └── SCL  ──────────────────────────────────>  └── [ GPIO 22 ]
                                                     │
                                                     │ (Alert Signal)
                                                     ▼
[ Piezo Buzzer ]                                [ ESP32 DevKit V1 ]
       │                                             │
       ├── (+)  ──────────────────────────────────>  └── [ GPIO 25 ]
       └── (-)  ──────────────────────────────────>      [ GND ]
```

---

## 📌 Pinout Summary Table

| Module | Module Pin | ESP32 Pin | Notes |
| :--- | :--- | :--- | :--- |
| **HX711** | `VCC` | `VIN` (`5V`) | 5V ensures stable internal voltage reference |
| **HX711** | `GND` | `GND` | Common ground |
| **HX711** | `DT` (`DOUT`) | `GPIO 17` | Serial Data Output |
| **HX711** | `SCK` (`CLK`) | `GPIO 16` | Serial Clock Input |
| **OLED Display** | `VCC` | `3V3` / `VIN` | Power supply |
| **OLED Display** | `GND` | `GND` | Common ground |
| **OLED Display** | `SDA` | `GPIO 21` | I2C Data Line (Do not connect to `SD0`/`SD3`) |
| **OLED Display** | `SCL` | `GPIO 22` | I2C Clock Line |
| **Piezo Buzzer** | `Positive (+)` | `GPIO 25` | Digital trigger pin |
| **Piezo Buzzer** | `Negative (-)` | `GND` | Common ground |

---

## 💻 Software & Library Setup

### 1. Required Arduino Libraries
Install via **Sketch ➔ Include Library ➔ Manage Libraries**:
1. **HX711 Arduino Library** by *Bogdan Necula*
2. **Adafruit SSD1306** by *Adafruit*
3. **Adafruit GFX Library** by *Adafruit*

### 2. Board Configuration in Arduino IDE
- **Board:** `ESP32 Dev Module`
- **Upload Speed:** `115200`
- **Flash Frequency:** `80MHz`
- **Flash Mode:** `QIO`
- **Partition Scheme:** `Default 4MB with spiffs`

---

## ⚖️ Mechanical Setup & Calibration Guide

### Step 1: Mechanical Mounting
A strain gauge load cell operates as a cantilever beam. For accurate readings:
1. Bolt the **fixed end** of the load cell to a rigid base plate using spacers/nuts so the center beam hangs freely without touching the surface.
2. Bolt the **load end** (where the sticker arrow points) to the weighing platform, also elevated with spacers.
3. Ensure the **arrow on the load cell label points downwards** in the direction of gravity.

### Step 2: Calculating the Calibration Factor
1. Upload `ESP32_Smart_Scale.ino` with the default `float calibration_factor = 420.0;`.
2. Leave the scale empty and press the **RST** button on the ESP32 so it zeroes (`0.0 g`).
3. Place an object of **known weight** on the platform (e.g., `500g`).
4. Calculate your exact calibration factor:

$$\text{New Calibration Factor} = \text{Current Factor} \times \left(\frac{\text{Displayed Weight}}{\text{Actual Known Weight}}\right)$$

5. Update `float calibration_factor` in `ESP32_Smart_Scale.ino` and re-upload.

---

## 🔧 Troubleshooting Guide

| Symptom / Error | Root Cause | Solution |
| :--- | :--- | :--- |
| **No COM Port in Arduino IDE** | Missing UART bridge driver or charge-only USB cable | Install **Silicon Labs CP210x Universal Windows Driver** (`silabser.inf`) and use a data-capable Type-C cable. |
| **`dfu-util: No DFU capable USB device` (Exit Status 74)** | Wrong ESP32 board selected (e.g., Arduino Nano ESP32) | Select **Tools ➔ Board ➔ esp32 ➔ ESP32 Dev Module**. |
| **`Fatal error: Failed to connect to ESP32: No serial data received`** | Board not entering bootloader mode or high baud rate | Lower Upload Speed to `115200`. Hold down the **BOOT** button when `Connecting....` appears until writing starts. |
| **OLED Screen Completely Blank** | Wrong I2C pins or address mismatch | Ensure SDA is on `G21` and SCL is on `G22` (do not use `SD0`/`SD3` flash pins). Verify I2C address is `0x3C`. |
| **`HX711 SENSOR ERROR!` on Display** | Under-voltage on HX711 or swapped DT/SCK lines | Move HX711 `VCC` from `3.3V` to `VIN (5V)` and verify `DT` is on `G17` and `SCK` is on `G16`. |

---

## 📄 License

This project is licensed under the [MIT License](LICENSE).
