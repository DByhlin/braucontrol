# BrauControl — ESPHome Universal Braumeister Controller

**BrauControl** is an open-source, standalone automated brewing controller built on ESPHome and LVGL for **ESP32-S3 7-inch (800×480) RGB touchscreens** (such as the Waveshare ESP32-S3-Touch-LCD-7). Designed for Speidel Braumeister systems (BM10, BM20, BM50) and custom RIMS/HERMS single-vessel kettles, it runs completely offline with an integrated Wi-Fi web dashboard and BeerXML recipe importer.

---

## Features

* **7" Touchscreen UI (LVGL):**
  * **Brew Session Dashboard:** Live radial kettle temperature gauge (with automatic `(BLEND)` indicator during dual-probe mashing), target temperature, countdown/delay timer, auxiliary probe readout, manual element/pump overrides, tap-to-silence hop banner, and step confirmation modals.
  * **Recipe Builder:** Up to **8 mash steps** (`30–85°C`), configurable boil duration and boil power (`30–100%`), up to **8 hop addition timers**, and **8 persistent NVS flash recipe slots**.
  * **Hardware & PID Setup:** One-tap presets for **BM10 (1.2kW)**, **BM20 (2.0kW)**, and **BM50 (3.3kW)**, custom pump run/rest cycling, software probe swapping, and built-in **PID auto-tuning**.
  * **On-Screen Wi-Fi Manager:** Scan 2.4 GHz networks and enter credentials directly on the touchscreen, with automatic fallback AP (`BM-BrewController`).
* **Automatic Dual-Probe Mash Blending:**
  * Works seamlessly with **1 or 2 DS18B20 temperature probes** across all hardware configurations.
  * When a second probe is placed in a mash thermowell during an active mash, BrauControl automatically weights control to **70% Mash Probe + 30% Kettle Probe** and displays **`(BLEND)`** on the temperature gauge.
  * If only 1 probe is connected—or during Boil, Auto-Tune, and Manual Mode—the controller automatically uses **100% of the main kettle probe**.
* **Embedded Web Dashboard (`/brewcontrol`):**
  * Mobile-friendly web UI for live telemetry, remote brew progression (`Start`, `Confirm Step`, `Start Boil`, `Stop`, `Delay Start`), manual I/O overrides, and hop alarm silencing.
  * Client-side **BeerXML (`.xml`)** parser supporting recipes exported from Brewfather, BeerSmith, and Grainfather.
* **230V Breaker / Fuse Protection:**
  * Dynamically caps heater duty cycles and alternates dual elements on **10A (2.3kW)**, **13A (3.0kW)**, **16A (3.6kW)**, or **Unlimited** circuits while reserving headroom for circulation pumps.
* **Power-Loss Recovery:**
  * Periodically checkpoints active brew state, mash step, and remaining time to NVS flash, automatically resuming an interrupted mash, boil, or delayed start upon reboot.

---

## Hardware & Pinout

### Supported Controller Board
* **Microcontroller:** ESP32-S3 (8MB Flash, Octal PSRAM @ 120MHz)
* **Display & Touch:** 800×480 MIPI RGB LCD + GT911 Capacitive Touch Controller

### GPIO & I2C Expander Mapping

| Interface / Expander | Pin / Address | Function | Notes |
| :--- | :--- | :--- | :--- |
| **I2C Bus (`bus_a`)** | `GPIO 8` (SDA) / `GPIO 9` (SCL) | Shared I2C bus | Connects GT911, CH422G, and PCF8574 |
| **GT911 Touch** | `GPIO 4` | Touch Interrupt (`INT`) | Reset handled via CH422G `EXIO1` |
| **OneWire Bus** | `GPIO 6` | DS18B20 Temp Probes | Index `0`: Kettle Probe, Index `1`: Aux/Mash Probe |
| **CH422G Expander** | `EXIO 1` | Touch Reset | Active-low touch reset |
| **CH422G Expander** | `EXIO 2` | LCD Backlight | Binary backlight control |
| **CH422G Expander** | `EXIO 6` | LCD Enable | Panel power/enable pin |
| **PCF8574 (`0x20`)** | `P0` | Heater Element 1 (SSR) | 2s Slow PWM, active-low (`inverted: true`) |
| **PCF8574 (`0x20`)** | `P1` | Heater Element 2 (SSR) | 2s Slow PWM, active-low (`inverted: true`) |
| **PCF8574 (`0x20`)** | `P2` | Circulation Pump 1 | Active-low relay control (`inverted: true`) |
| **PCF8574 (`0x20`)** | `P3` | Circulation Pump 2 | Active-low relay control (`inverted: true`) |
| **PCF8574 (`0x20`)** | `P4` | Alarm Buzzer | Active-high piezo buzzer (`inverted: false`) |

---

## Built-In Safety Interlocks & Safe-Pause Recovery

BrauControl runs a 1-second hardware watchdog loop across both **automated brewing** and **manual heater overrides**. When any fault condition triggers, BrauControl executes a **Safe-Pause**: it immediately cuts power to all heaters and pumps, sounds the alarm buzzer, and **holds your active mash step or boil timer** so you can resolve the physical issue and tap **START** (or **BOIL**) to resume right where you left off—or tap **STOP** to fully abort.

* **Isomerization-Aware Boil Timer:** If a Safe-Pause occurs during the boil after hops have already been added and the wort remains **`>= 80.0°C`**, the boil countdown (and any upcoming hop alerts) continues automatically so hop alpha-acid isomerization and IBU balance stay on schedule; if wort temperature drops below `80.0°C`, the timer pauses until heating resumes.

1. **Over-Temperature Safe-Pause:** Cuts power and pauses the session if `kettle_temp > 105.0°C`.
2. **Lost Sensor Safe-Pause:** Cuts power and pauses the session if the primary kettle probe disconnects or returns `NaN` during active heating (automated or manual).
3. **Dry-Fire / Sliding-Window Spike Safe-Pause:** Cuts power and pauses the session if kettle temperature jumps by **more than `3.0°C` within any 15-second sliding window** while heaters are active.
4. **Detached Probe / No-Rise Safe-Pause:** Cuts power and pauses the session if heaters run at high power (`>= 80%`) below `96°C` with less than `0.5°C` temperature rise over **3 minutes (`180s`)** in standard mode, or **5 minutes (`300s`)** during dual-probe blended mashing.
5. **Pump Thermal Lockout:** Automatically turns off and locks out circulation pumps whenever wort temperature reaches **`>= 88.0°C`** to prevent pump cavitation and impeller damage.
6. **Smart Mash Hold:** On dual-element systems (BM50), mash temperature maintenance uses only the smaller 1300W element to prevent scorching and overshoot.

---

## Repository Structure

* `braumeister-googleai.yaml` — Main ESPHome configuration, hardware pins, dual-probe temperature blending, PID climate controller, and persistent globals.
* `bm_ui.yaml` — LVGL 800×480 touchscreen interface (Home, Brew Session, Recipe Builder, Hardware Setup, Wi-Fi Settings).
* `bm_scripts.yaml` — Brew state machine, sliding-window safety interlocks, Safe-Pause recovery, PID auto-tune handler, power-loss recovery, and `/brewcontrol` web API.
* `braumeister_helpers.h` — Embedded HTML/JS web dashboard, BeerXML parser, and C++ recipe/list helper functions.
* `secrets.yaml` — Wi-Fi credentials template (Wi-Fi can also be configured directly via the touchscreen).

---

## Installation & Flashing

1. Install **ESPHome `2026.1.0`** or newer.
2. Place all five source files in your ESPHome configuration directory.
3. Compile and flash via USB-C for the initial install:
   ```bash
   esphome run braumeister-googleai.yaml
   ```
4. Once booted, tap **WIFI SETTINGS** on the touchscreen to connect to your 2.4 GHz network, then open `http://<CONTROLLER-IP>/brewcontrol` in any browser to upload BeerXML recipes or monitor your brew remotely.

---

## Disclaimer & AI Development Notice

### AI-Assisted Development
Large portions of the firmware, web dashboard, and user interface code in this repository were generated and refined using Artificial Intelligence (AI) coding tools. While the software includes built-in safety interlocks and has been structured for brewing automation, AI-generated code may contain unexpected edge cases, logic flaws, or hardware-specific bugs.

### Limitation of Liability & Hardware Safety
**USE AT YOUR OWN RISK.** Brewing automation involves high-voltage electricity, high-wattage heating elements, and boiling liquids, all of which carry inherent risks of fire, electrical shock, scalding, and property damage. 

* **No Warranty:** In accordance with the GNU General Public License v3.0 (GPL-3.0), this software is provided *"AS IS"*, without warranty of any kind, express or implied. 
* **No Liability:** Under no circumstances shall the project author, contributors, or copyright holders be held liable for any direct, indirect, incidental, or consequential damages, personal injury, or equipment failure resulting from the use, misuse, or inability to use this software or its associated hardware schematics.
* **User Responsibility:** By flashing or running this firmware, you assume full responsibility for verifying the code, ensuring proper electrical isolation and grounding, installing independent physical safety devices (such as thermal fuses, GFCI/RCD breakers, and mechanical high-limit cutoffs), and never leaving active heating elements unattended.
