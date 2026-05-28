# E-Paper Display Dashboard

A personal dashboard built with an ESP32-S3 and a Waveshare 6" HD e-Paper display. Navigate between pages using an IR remote, displaying the date, a random facts page, and a todo checklist.

---

## Hardware

| Component | Details |
|---|---|
| Microcontroller | ESP32-S3 (FREENOVE WROOM) |
| Display | Waveshare 6inch HD e-Paper HAT (IT8951, 1448×1072) |
| IR Receiver | TSOP1738 |
| Remote | Generic IR remote |

### Wiring

#### E-Paper Display (SPI)
| HAT Pin | ESP32-S3 GPIO |
|---|---|
| 5V | 5V |
| GND | GND |
| MISO | GPIO 13 |
| MOSI | GPIO 11 |
| SCK | GPIO 12 |
| CS | GPIO 10 |
| RST | GPIO 9 |
| HRDY/BUSY | GPIO 8 |

> **Note:** MISO and MOSI are swapped from defaults in `SPI.begin()`. No DC pin — the IT8951 handles command/data distinction internally.

#### IR Receiver (TSOP1738)
| Sensor Pin | ESP32-S3 |
|---|---|
| OUT | GPIO 15 |
| GND | GND |
| VCC | 3.3V |

> **Note:** Use 3.3V not 5V for the TSOP1738 on ESP32-S3.

---

## Features

- **Home Page** — displays current date fetched via NTP
- **Random Page** — fetches a random useless fact from the web, refresh with the pause button
- **Todo Page** — checklist with selectable and toggleable items
- **IR Remote Navigation** — navigate between pages with forward/back buttons
- **WiFi + NTP** — automatic time sync on boot

---


## Setup & Installation

> **Note:** use the epaper-tests file and folder, ignore the other test files :)

### 1. Prerequisites

Install the following libraries via Arduino IDE Library Manager:
- `GxEPD2`
- `IRremote`
- `ArduinoJson`

Board: **ESP32-S3 Dev Module**

Partition Scheme: **Huge APP (3MB No OTA/1MB SPIFFS)**
> This is required — set it under `Tools → Partition Scheme` in Arduino IDE.

### 2. WiFi Credentials

Create a `secrets.h` file in the sketch folder (see `secrets.h.example`):

```cpp
#define WIFI_SSID "your_network_name"
#define WIFI_PASSWORD "your_password"
```

> `secrets.h` is listed in `.gitignore` and will never be committed.

### 3. VCOM Value

The VCOM value is printed on the sticker on the display's FPC ribbon cable (e.g. `-2.99`). This is set during display initialization.

### 4. Upload

Select your ESP32-S3 board and port in Arduino IDE, then upload.

---

## IR Remote Button Mapping

| Button | Action |
|---|---|
| Forward | Next page |
| Back | Previous page |
| Pause | Refresh fact (on Random page) |

---

## Notes

- E-ink displays retain their image with no power — ghosting may occur with long-term static content
- The display uses a paging system (`HEIGHT / 8`) due to RAM constraints
- `configTime()` is called once on boot for NTP sync — time is read each loop via `getLocalTime()`
