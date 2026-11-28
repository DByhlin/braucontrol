# BrauControl — ESPHome Universal Braumeister Controller

**BrauControl** is an open-source, standalone automated brewing controller built on ESPHome and LVGL for **ESP32-S3 7-inch (800×480) RGB touchscreens** (such as the Waveshare ESP32-S3-Touch-LCD-7). Designed for Speidel Braumeister systems and custom RIMS/HERMS kettles, it operates offline with an integrated Wi-Fi web dashboard, BeerXML recipe importer, and local mDNS handoff for compatible fermentation controllers.

---

## Core Features & Updates

* **Enhanced 7" Touchscreen UI (LVGL):** Features a live radial kettle temperature gauge with dual-probe blending indicators `(BLEND)`, up to 8 mash steps and hop addition timers, and 8 persistent NVS flash recipe slots. Setup menus are organized into tabbed interfaces (`KETTLE & FUSE`, `PID TUNER`, `FERMENTATION`, `2.4 GHZ WIFI`, and `WEB & ACCESS POINT`).
* **Fermentation Controller Integration:** Utilizes automatic LAN discovery via mDNS (`_brauferm._tcp`) to pair up to 6 local fermenters. Includes a post-boil handoff modal to transmit recipes, measured original gravity, batch volume, and yeast data directly to `http://<FERMENTER-IP>/brew_handoff`.
* **Dual-Probe Mash Blending & Safety:** Automatically weights control between 70% mash probe and 30% kettle probe when a secondary DS18B20 is detected, falling back to 100% kettle probe during boils or manual overrides. Includes a 1-second hardware watchdog loop, Safe-Pause recovery, isomerization-aware boil timers, and strict thermal lockouts.
* **Embedded Web Dashboard (`/brewcontrol`):** Mobile-friendly interface supporting live telemetry, remote brew state controls, and a client-side BeerXML parser for Brewfather, BeerSmith, and Grainfather files. Features power-loss recovery and dynamic 230V breaker/fuse protection (10A, 13A, 16A, or Unlimited).

---

## Hardware & Pinout

* **Controller:** ESP32-S3 (8MB Flash, Octal PSRAM @ 120MHz) with an 800×480 MIPI RGB LCD and GT911 Capacitive Touch Controller.
* **Key Connections:** Shared I2C bus (`GPIO 8` / `GPIO 9`), OneWire DS18B20 probes (`GPIO 6`), CH422G I/O expander for display/touch resets, and PCF8574 (`0x20`) for SSR heating elements, circulation pumps, and a piezo alarm buzzer.

---

## Repository Structure & Installation

* **Source Files:** `bm_braucontrol.yaml` (main config and PID/blending), `bm_ui.yaml` (touchscreen layouts), `bm_scripts.yaml` (state machine, safety interlocks, and mDNS/HTTP handoffs), `braumeister_helpers.h` (web dashboard and BeerXML/C++ helpers), and `secrets.yaml`.
* **Flashing Steps:** Install ESPHome `2026.1.0` or newer, place the source files in your configuration directory, and run `esphome run bm_braucontrol.yaml` via USB-C. Post-boot, configure Wi-Fi via the touchscreen and access `http://<CONTROLLER-IP>/brewcontrol` or the **HARDWARE -> FERMENTATION** menu to pair external fermenters.

---

## Disclaimer & AI Development Notice

Built using AI-assisted coding tools. **USE AT YOUR OWN RISK.** Brewing automation involves high voltage, high-wattage elements, and boiling liquids. Provided "AS IS" under the GNU General Public License v3.0 (GPL-3.0) with no warranty or liability; users are responsible for proper electrical isolation, GFCI/RCD breakers, mechanical high-limit cutoffs, and attending active equipment.
