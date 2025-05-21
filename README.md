# M5Core2 Life Counter

A responsive, low-power life counter for tabletop games like Magic the Gathering, built for the M5Stack Core2.

## Features

- Tap top or bottom of the screen to increment or decrement life.
- Displays life total in large font.
- Shows recent change (`+/-` delta) for 500ms after life change.
- Dim screen to 30% brightness after 50 seconds of inactivity (for energy saving).
- Battery status display with charging indicator.
- Reset life to:
  - **20** with Button A
  - **40** with Button C

## Hardware

- [M5Stack Core2](https://docs.m5stack.com/en/core/core2)

## Controls

| Input        | Action                         |
|--------------|--------------------------------|
| Tap top half | +1 life                        |
| Tap bottom   | -1 life                        |
| Hold tap     | Repeats increment/decrement    |
| Button A     | Reset to 20 life               |
| Button C     | Reset to 40 life               |

## Setup

1. Install the [M5Stack library](https://github.com/m5stack/M5Core2) in Arduino IDE.
2. Upload the code to your M5Core2.
3. Start tracking your life total with style.

## TODO

- 2-player split screen mode
- Long-press reset confirmation
- Persistent settings (EEPROM)

---

Made with ❤️ for tabletop players.

