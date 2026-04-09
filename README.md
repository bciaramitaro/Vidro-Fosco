# Vidro-Fosco

An interactive Arduino and TouchDesigner installation prototype for animator Matilde Senos, created in collaboration with Yidi Chen. Combining animation, projection, and Pepper's Ghost technique to bring a fairy to life, the piece was designed as a proof of concept for audience testing during an interactive feedback session. Audience members interact with conductive props from the fairy's world (rocks and trees), which trigger new animations and move the story forward.

**Project documentation:** [bellaciaramitaro.com/vidro-fosco](https://www.bellaciaramitaro.com/vidro-fosco)

---

## Overview

The system works in two parts:

**Arduino (`Arduino_Interactive_Animation.ino`)** reads three software-based capacitive touch sensors on analog pins A0–A4 using the ADCTouch library, which simulates capacitive sensing without dedicated hardware. Each pin is assigned to a physical prop:

| Pin | Prop | Action |
|-----|------|--------|
| A0  | Rocks | State streamed to TouchDesigner |
| A1  | Tree A | State streamed to TouchDesigner |
| A2  | Tree B | State streamed to TouchDesigner |

Each sensor uses a rolling average of 100 readings for noise smoothing, and a touch is registered when the reading exceeds the average by a multiplier of 1.2. The three touch states (0 or 1) are streamed over serial at 9600 baud as a space-separated line — values must be separated by spaces, not commas or periods, to parse correctly in TouchDesigner.

A continuous rotation servo on pin 10 starts automatically on setup (`servo.write(110)`) and runs independently of touch input. The LED strip on pin 6 is triggered by touch on Tree B and latches on — it will not turn off again until the Arduino is reset.

**TouchDesigner (`projection.toe`)** receives the serial data from Arduino via a `serial` DAT, which is parsed and converted through `select`, `convert`, and `math` operators into three individual touch channels (`touch_data1`, `touch_data2`, `touch_data3`), each fed through a `logic` operator to detect active touch states.

The patch is structured around five sequential stages, each contained in its own subnetwork and triggered by the touch inputs:

| Stage | Trigger | Video | Audio |
|-------|---------|-------|-------|
| 1 | Background loop | `01_background01eloop` | — |
| 2 | Background + fairy | `02_fairy10movib2bgm` | — |
| 3 | Stone touch (A0 — Rocks) | `touch_stone_video` | `touch_stone_audio` |
| 4 | Tree A touch (A1) | `touch_treeA_video` | `touch_treeA_audio` |
| 5 | Tree B touch (A2) | `touch_treeB_video` | `touch_treeB_audio` |

Each stage also triggers corresponding background and audio layers that play in parallel. Video and audio are routed through separate switch networks and a reset button allows the sequence to restart from the beginning. The final output is corner-pinned for projection alignment and routed to two windows for multi-display projection.

---

## Setup

### Hardware Requirements

- Arduino (Uno or compatible)
- 3 conductive touch surfaces (e.g. conductive fabric, foil, or painted props)
- Continuous rotation servo motor
- Jumper wires
- USB cable for Arduino-to-computer serial connection
- Computer running TouchDesigner

### Wiring

| Component | Arduino Pin |
|-----------|-------------|
| Touch — Rocks | A0 |
| Touch — Tree A | A1 |
| Touch — Tree B | A2 |
| Servo motor | Digital 10 |

### Arduino Setup

1. Install the [ADCTouch library](https://github.com/martin2250/ADCTouch) and the built-in **Servo** library via the Arduino Library Manager.
2. Open `Arduino_Interactive_Animation.ino` in the Arduino IDE.
3. If touch sensitivity needs adjustment, modify the `multiplier` value (default `1.2`). Increase for less sensitivity, decrease for more.
4. Upload the sketch to your Arduino.
5. Open the Serial Monitor (set to **9600 baud**) to confirm three space-separated values are streaming correctly when the props are touched.

### TouchDesigner Setup

1. Open `projection.toe` in TouchDesigner.
2. Navigate to the `serial` DAT in the bottom-left subnetwork and update the COM port to match the port your Arduino is connected to.
3. Ensure the baud rate is set to **9600** to match the Arduino.
4. Use the **reset** button (visible in the top-left of the network) to restart the stage sequence at any time.
5. The output renders to two windows (`window1` and `window2`) — align these to your projection surfaces using the `corner1` corner-pin operator in the output network.
6. Run the patch and verify that touching the props advances the animation stages in real time.

### Collaborators

- **Bella Ciaramitaro** 
- **Yidi Chen** 
- **Matilde Senos** 
