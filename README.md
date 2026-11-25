# Digital Ruler: IoT Workshop (Applied Technology Training)

Workshop material for an applied technology training at FTEK UKSW: an ESP32 ultrasonic "digital ruler" that publishes measurements over MQTT to a live web dashboard.

<!-- TODO: add demo GIF/photo -->

## Overview

Participants build a distance meter with an HC-SR04-style ultrasonic sensor and an OLED display. Pressing the button publishes the current reading to an MQTT topic. A static web page subscribes to the same topic and shows the value on an animated ruler bar.

```
HC-SR04 ──> ESP32 ──(MQTT, TCP 1883)──> broker.hivemq.com ──(MQTT over WSS 8884)──> Web dashboard
               │
               └──> SSD1306 OLED
```

## Features

- Continuous distance measurement shown on a 128x64 OLED
- Button-triggered MQTT publish with on-screen "SENT!" feedback
- Browser dashboard with connection status, last-update time and a 0 to 30 cm ruler bar (turns red below 5 cm)
- No backend: uses the public HiveMQ broker

## Hardware

| Part | ESP32 pin |
| --- | --- |
| Ultrasonic sensor TRIG | GPIO 5 |
| Ultrasonic sensor ECHO | GPIO 18 |
| Push button (to GND) | GPIO 17 |
| SSD1306 OLED (I2C, 0x3C) | SDA 21, SCL 22 (default) |

## Repository layout

```
digital_ruler_mcu/digital_ruler_mcu.ino   ESP32 firmware
website/index.html                        MQTT dashboard (Paho MQTT JS)
```

## Getting started

### Firmware

1. Install the ESP32 board package and these libraries in the Arduino IDE: `PubSubClient`, `Adafruit GFX`, `Adafruit SSD1306`.
2. In `digital_ruler_mcu.ino`, set `ssid`, `password` and a unique `mqtt_topic`.
3. Upload and open the serial monitor at 115200 baud.

### Dashboard

1. In `website/index.html`, set `topic_target` to exactly the same topic used in the firmware.
2. Open the file in a browser (or host it on GitHub Pages) and press the button on the device.

Note: the two files ship with different placeholder topics (`workshop/ftek/namatopic` and `esp32/workshop/ftek/namatopic`). They must match for data to appear.
