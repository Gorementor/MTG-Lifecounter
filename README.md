# M5Core2 Life Counter

A responsive, low-power life counter for tabletop games like *Magic: The Gathering*, built for the M5Stack Core2.

## Features

* Tap top or bottom of the screen to increment or decrement life.
* Long-press to change life by ±5 repeatedly.
* Two-player mode with split-screen layout.
* Switch between single and two-player modes via **Button B**.
* Shows recent life change (`+/-` delta) for each player.
* Dims screen after inactivity to save battery.
* Battery percentage and charging status always visible.
* Reset life to:

  * **20** with Button A
  * **40** with Button C

## Hardware

* [M5Stack Core2](https://docs.m5stack.com/en/core/core2)

## Controls

| Input      | Action                          |
| ---------- | ------------------------------- |
| Tap top    | +1 life                         |
| Tap bottom | -1 life                         |
| Hold tap   | ±5 life (repeats while held)    |
| Button A   | Reset to 20 life                |
| Button B   | Toggle 1-player / 2-player mode |
| Button C   | Reset to 40 life                |

## 2-Player Mode

* Screen is split vertically: left = Player 1, right = Player 2
* Each player can independently tap their half of the screen to change life.
* A large vertical white line visually separates both areas.

## Setup

1. Install the [M5Stack library](https://github.com/m5stack/M5Unified) in Arduino IDE.
2. Upload the code to your M5Core2.
3. Enjoy enhanced life tracking for one or two players!

## TODO

* Long-press reset confirmation
* Persistent settings (EEPROM)

---

Made with ❤️ for tabletop players.
