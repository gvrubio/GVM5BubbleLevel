# GVM5BubbleLevel

**GVM5BubbleLevel** is a standalone bubble-level utility for the **M5Cardputer ADV**, created as part of the **GVM5Tools** collection — a suite of simple, focused programs built following the **Unix philosophy**:

> *Do one thing, and do it well.*

This application uses the Cardputer’s onboard IMU to present a smooth, visually intuitive bubble level with angle readouts, degree markers, and battery status.

---

## ✨ Features

* Real-time pitch and roll using the internal IMU
* Smoothly filtered bubble movement
* Scaled circular level indicator with degree ticks
* Crosshair-style bubble marker
* Battery voltage and percentage display
* Double-buffered rendering for flicker-free graphics
* Clean and self-contained code structure

---

## 🧠 Philosophy

GVM5Tools is built around the classic **Unix mindset**:

* **Small** — Each utility is compact and self-contained
* **Single-purpose** — Every program performs exactly one task
* **Clear** — Code is readable and easy to maintain
* **Modular** — Tools can be combined or used independently

GVM5BubbleLevel is one such tool: a dedicated digital level.

---

## 📦 File Overview

| File                    | Description                        |
| ----------------------- | ---------------------------------- |
| **GVM5BubbleLevel.ino** | Main bubble level application      |
| **splash.h**            | Contains my custom splash screen |
| **README.md**           | Project documentation              |

---

## 🔧 Requirements

* **M5Cardputer ADV**
* **M5Unified** and **M5GFX** libraries
* Arduino IDE or PlatformIO toolchain

---

## 🚀 Installation

1. Place `GVM5BubbleLevel.ino` and `splash.h` in a project folder.
2. Open the `.ino` file in Arduino IDE or PlatformIO.
3. Ensure required M5 libraries are installed.
4. Compile and upload to the M5Cardputer ADV.
5. Run it from your preferred launcher or environment.

---

## 📝 License
GNU AFFERO
https://github.com/gvrubio/GVM5BubbleLevel/blob/main/LICENSE
