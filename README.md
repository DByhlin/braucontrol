# 🍺 BrauControl

**BrauControl** is an open-source, ESPHome-based automated brewing controller designed for Speidel Braumeister systems (BM10, BM20, BM50) and custom RIMS/HERMS single-vessel brewing setups. Built for the **Waveshare ESP32-S3-Touch-LCD-7 (800×480)**, it features a responsive dark-mode LVGL touchscreen interface, an embedded local web dashboard, BeerXML recipe importing, and seamless mDNS handoff integration with **FermControl**.

---

## ✨ Key Features

### 🖥️ Touchscreen & Web Interface
* **7" Capacitive Touch UI (LVGL):** Landscape 800×480 interface with live temperature arc gauges, step timers, manual SSR/pump overrides, and on-screen keyboards.
* **Embedded Web Dashboard (`/brewcontrol`):** Mobile-friendly local web UI for remote monitoring, manual overrides, and live telemetry without requiring external cloud services.
* **Home Assistant Native Integration:** Built on ESPHome (`2026.1.0+`) with full API and OTA support.
* **Offline & Fallback Hotspot Mode:** Automatically spins up a local Wi-Fi Access Point (`BM-BrewController`) if your configured 2.4 GHz network is unreachable.

### 🔥 Brewing & PID Automation
* **Hardware Presets & 230V Fuse Protection:** Built-in profiles for **BM10 (1200W)**, **BM20 (2000W)**, and **BM50 (3300W, 1 or 2 pumps)**. Dynamic power capping and element alternation protect **10A, 13A, or 16A** household breakers.
* **PID Temperature Control & Auto-Tune:** Precision slow-PWM SSR control with on-device PID auto-tuning (`65°C` water calibration) and flash persistence.
* **Dual-Probe Mash Blending:** Supports two DS18B20 temperature probes (Kettle + Aux/Mash) with automatic 70/30 sensor blending during active mash phases and one-tap probe swapping.
* **Multi-Step Mash & Hop Timers:** Configure up to **8 mash steps**, delayed start timers (up to 12 hours), programmable pump rest/run cycles, boil power percentage, and up to **8 hop addition alarms**.
* **Power-Loss Recovery:** Automatically saves active brew state and timers to flash so sessions can resume safely after an unexpected power outage.

### 📂 Recipes & Ecosystem Integration
* **8 Persistent Recipe Slots:** Preloaded with classic styles (German Pilsner, West Coast IPA, Hefeweizen) and customizable on-device.
* **BeerXML Importer:** Upload `.xml` files directly from **Brewfather, BeerSmith, or Grainfather** via browser. Parses mash steps, boil time, hop additions, estimated OG/FG, yeast strain, and fermentation stages.
* **FermControl Handoff:** Discovers **FermControl** units on your LAN via mDNS (`_brauferm._tcp`) and pushes batch metadata (Recipe, Measured OG, Batch Volume, Yeast, and Fermentation Schedule) straight to your fermenter at the end of the boil.

### 🛡️ Built-in Safety Interlocks
* **Dry-Fire Protection:** Monitors heating rate and triggers an immediate safety pause if temperature spikes `> 3.0°C` within 15 seconds.
* **Detached Probe Detection:** Halts heating if temperature fails to rise by at least `0.5°C` over 180–300 seconds under full power.
* **Over-Temp & Pump Lockout:** Automatically cuts heaters above `105°C`, disables pumps at `≥ 88°C` to prevent cavitation/damage, and pauses heating if a temperature probe disconnects.

---

## 🛠️ Hardware & Pinout

Designed specifically for the **Waveshare ESP32-S3-Touch-LCD-7** (8MB Flash, Octal PSRAM) paired with a **PCF8574** I2C I/O expander (`0x20`) for external Solid State Relays (SSRs) and buzzer control.

| Function | Interface / Pin | Notes |
| :--- | :--- | :--- |
| **I2C Bus (Touch & Expanders)** | `SDA: GPIO8`, `SCL: GPIO9` | Connects GT911 Touch, CH422G & PCF8574 |
| **1-Wire Temp Probes** | `GPIO6` | Up to 2× DS18B20 probes (Kettle & Aux) |
| **Touch Interrupt** | `GPIO4` | GT911 Capacitive Touch Controller |
| **Heater 1 SSR** | `PCF8574 P0` | Inverted output, 2s Slow PWM |
| **Heater 2 SSR** | `PCF8574 P1` | Inverted output, 2s Slow PWM (BM50) |
| **Pump 1 Relay/SSR** | `PCF8574 P2` | Inverted output |
| **Pump 2 Relay/SSR** | `PCF8574 P3` | Inverted output (BM50 2-Pump) |
| **Alarm Buzzer** | `PCF8574 P4` | Active-high audible alert for hops/steps/safety |
| **LCD Backlight / Reset** | `CH422G P2 / P1 / P6` | Display enable, touch reset & backlight |

---

## 🚀 Installation & Setup

### Prerequisites
* **ESPHome** version `2026.1.0` or newer.
* Ensure all project files are in your ESPHome configuration directory:
  * `braumeister-controller.yaml` (main config)
  * `bm_scripts.yaml`
  * `bm_ui.yaml`
  * `braumeister_helpers.h`

### Flashing
1. Connect your **ESP32-S3-Touch-LCD-7** via USB-C.
2. Compile and flash using the ESPHome CLI or Home Assistant Dashboard:
   ```bash
   esphome run braumeister-controller.yaml
   ```
3. On first boot, tap **WIFI SETTINGS** on the touchscreen to scan and connect to your 2.4 GHz Wi-Fi network, or connect to the fallback hotspot **`BM-BrewController`**.
4. Access the local web dashboard and BeerXML uploader at:
   ```text
   http://<CONTROLLER-IP>/brewcontrol
   ```

---

## ⚠️ Disclaimer & Limitation of Liability

This software, configuration, and any associated documentation are provided **"AS IS"**, without warranty of any kind, express or implied, including but not limited to the warranties of merchantability, fitness for a particular purpose, and non-infringement.

**High Voltage & Thermal Hazard:** Building and operating an automated brewing controller involves working with **lethal mains voltage (110V/230V AC)**, high-wattage heating elements, boiling liquids, and pressurized/hot vessels. Improper wiring, inadequate earthing/grounding, faulty Solid State Relays (which can fail in a closed/always-on state), or software bugs can result in **electric shock, fire, property damage, severe burns, personal injury, or death**.

* **You assume full and sole responsibility** for all risks associated with assembling, wiring, testing, and operating this project.
* All mains electrical work must comply with your local electrical codes and regulations, and should be performed or inspected by a licensed electrician. Always use appropriate physical safety devices, including Residual Current Devices (RCD/GFCI), circuit breakers, thermal fuses, and proper grounding.
* Software-based safety interlocks (such as dry-fire, over-temperature, and breaker limits) are convenience features only and **must never be relied upon as a primary safety mechanism**. Never leave active heating elements unattended.
* In no event shall the author(s) or contributors be held liable for any direct, indirect, incidental, special, exemplary, or consequential damages, personal injury, or loss arising in any way out of the use or inability to use this project.
