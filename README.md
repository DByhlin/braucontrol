# braucontrol
Universal ESP32-S3 7 inch touchscreen brewing firmware. Can be used to replace the control unit on BM10, BM20 and BM50.

# For now, it has the following functions:

- Recipe import (BeerXML)
- Local web server with some controls and the BeerXML uploader.
- Delayed start
- Configurable for 1 or 2 heating elements, and 1 or 2 pumps, so should cover all models of BM10/BM20/BM50.
- The Web UI works well on mobile devices, so as long as you are on the same WiFi you could follow along from your phone.
- Confirm before starting the next step in the brew session.
- Presets for different Braumeister models.
- Circuit breaker adaptation; you can set your circuit braker amperage and the software will compensate for this. Mainly used for the BM50.


# Hardware that you need:

- Waveshare ESP32-S3 7" touchscreen with 800x480 resolution (https://docs.waveshare.com/ESP32-S3-Touch-LCD-7)
- 1 or 2 40A SSRs with heatsink (for the heating elements)
- 1 or 2 2A G3MB-202P SSR module (for the pumps)
- 1 PCF8574 IO expander module
- 1 or 2 DS18B20 sensors (recommended to replace the sensor inside the kettle, but also add an external sensor inside the mash)

# Not finished:
- 3D printed controller enclosure
- Cable connectors

# Please note: This requires high voltage wiring as well as modifying your original cable connectors to the heaters and pumps.
# THIS SHOULD ALWAYS BE DONE BY A LICENSED ELECTRICIAN.
However, the new connectors eliminate the issue on the earlier controllers where the socket and connector on the BM50 could melt.

This software has been created in Googles AI-mode straight in my browser, I am not a programmer.
