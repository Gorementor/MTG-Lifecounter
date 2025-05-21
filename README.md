# M5Core2 Life Counter

A responsive, low-power life counter for tabletop games like Magic: The Gathering, built for the M5Stack Core2.

## Features

- Tap **top** or **bottom** of the screen to adjust life:
  - **Short tap (<1s)**: +/-1 life
  - **Long press (≥1s)**: Repeats +/-5 life while held
- Displays life total in a large, clear font.
- Shows recent life change (`+/-` delta) for 500ms after each update.
- Automatically dims screen after 5 seconds of inactivity to save power.
- Displays battery percentage and charging status.
- Quick reset:
  - **Button A** resets life to **20**
  - **Button C** resets life to **40**

## Hardware

- [M5Stack Core2](https://docs.m5stack.com/en/core/core2)

## Controls

| Input              | Action                              |
|-------------------|-------------------------------------|
| Short tap (top)    | +1 life                             |
| Short tap (bottom) | -1 life                             |
| Long press (top)   | Repeats +5 life every 400ms         |
| Long press (bottom)| Repeats -5 life every 400ms         |
| Button A           | Reset to 20 life                    |
| Button C           | Reset to 40 life                    |

## Setup

1. Install the [M5Unified library](https://github.com/m5stack/M5Unified) in the Arduino IDE.
2. Upload the code to your M5Core2 using a compatible USB cable.
3. Start playing — no more pen and paper needed!

## TODO

- 2-player split screen mode (toggle with Button B)

---

Made with ❤️ for tabletop players.
