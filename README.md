# 🤖 SenseCore — Automated Access Control & Environmental Monitor

**An Arduino Embedded System that watches its surroundings, makes decisions, and takes physical action — safely, and without ever locking itself out of its senses.**

---

## 👋 What is this, really?

Picture a smart doorway. As someone walks up, a sensor clocks the distance and decides: *plenty of room, getting close, too close.* Based on that decision, a little motorized gate opens, half-closes, or locks shut. Lights and a buzzer escalate to match the urgency. Meanwhile, the system is also watching the room's light level and adjusting its own display brightness, listening for a remote control, and keeping an eye on its own health — if a sensor ever goes quiet, it notices, flags a fault, and recovers on its own.

That's SenseCore Advanced: one small board doing the job of several coordinated systems at once, the way a real embedded product would.

**Where you'd actually use something like this:**
- An automated gate or barrier for a lab, warehouse, or restricted work cell
- A safety curtain for a robotics/CNC station (stop something dangerous before a person gets too close)
- A smart parking or loading-dock stopper
- An accessible entryway that opens/closes based on proximity instead of a badge swipe
- A teaching rig for anyone learning sensor fusion, state machines, or real-time embedded design

---

## 🧰 Hardware — the parts list and exactly how they're wired

Everything runs off one **Arduino Uno**. Here's every component, the pin it lands on, and why it's wired that way.

| # | Component | Arduino Pin | Wiring Notes |
|---|-----------|:-----------:|---------------|
| 1 | **HC-SR04 Ultrasonic Distance Sensor** | `TRIG` → D4, `ECHO` → D3 | `ECHO` is on D3 specifically because it's one of the Uno's external-interrupt-capable pins — we trigger a timer on both the rising and falling edge of the echo pulse instead of polling, which is far more precise. VCC → 5V, GND → GND. |
| 2 | **IR Receiver Module (e.g. TSOP38238) + remote** | `OUT` → D5 | VCC → 5V, GND → GND. Lets you send commands (unlock, switch units, cycle display) from across the room, no wires needed. |
| 3 | **Push Button** | D2 | Wired between D2 and GND. We use the Uno's **internal pull-up** (`INPUT_PULLUP`), so no external resistor is needed — the pin reads HIGH normally and LOW when pressed. Configured as a hardware interrupt on the falling edge, debounced in software. |
| 4 | **Photoresistor (LDR)** | A0 | Wired as a voltage divider: 5V → LDR → A0 → 10kΩ resistor → GND. As light increases, resistance drops, and the voltage at A0 rises — that's what we read on the analog pin. |
| 5 | **16×2 LCD Display** | `RS`→A5, `EN`→A4, `D4`→D6, `D5`→D7, `D6`→D8, `D7`→D9 | Running in 4-bit mode (only 4 data lines instead of 8) to save pins. `R/W` is tied to GND (write-only, which is all we need). A 10kΩ potentiometer on `V0` controls contrast — start it around the midpoint and adjust to taste. |
| 6 | **Warning LED** | D11 (PWM-capable) | Through a ~220–330Ω resistor to GND. Blinks at different rates depending on how close/urgent things are. |
| 7 | **Error LED** | D12 | Same resistor setup. Comes on solid/fast-blinking when the system is locked or faulted. |
| 8 | **Backlight/Ambient LED** | D10 (PWM-capable) | Brightness is driven by `analogWrite()`, scaled automatically against the LDR reading — brighter room, dimmer LED, and vice versa. |
| 9 | **Micro Servo (gate actuator)** | D13 | Signal wire → D13, power (V+) and ground run **straight to the 5V supply, not through the Arduino's 5V pin**, since servos can spike in current draw and you don't want that browning out your board. Rotates 0°–180° to represent gate closed → half → open. |
| 10 | **Passive Piezo Buzzer** | A1 | One leg → A1, the other → GND. Driven with `tone()`, so pitch (not just on/off) can escalate with urgency. |
| 11 | *(Optional)* **TMP36 Temperature Sensor** | A2 | Only needed if you enable temperature-compensated distance readings — the speed of sound actually changes with air temperature, so on a serious build you'd correct for it. Feature-flagged off by default so you don't need the part to run the project. |

**Total parts:** 1 Arduino Uno, 1 HC-SR04, 1 IR receiver + remote, 1 push button, 1 photoresistor + 10kΩ resistor, 1 16×2 LCD + 10kΩ potentiometer, 2 standard LEDs + resistors, 1 PWM-driven LED, 1 micro servo, 1 passive piezo buzzer, breadboard, jumper wires, external 5V supply for the servo.

---

## 🧠 What's actually going on under the hood

This is the part that makes it more than a wiring exercise — it's designed the way a real embedded system is designed:

- **A real state machine.** The system is always in exactly one of: *Monitoring → Warning → Locked → Settings → Fault.* Every behavior — LEDs, buzzer, servo position, display — falls out of "what state am I in," not a pile of scattered `if` checks.
- **Sensor fusion, not just sensor reading.** The ultrasonic distance goes through a Kalman filter — the same category of math used in GPS and motion tracking — to smooth out noise without lagging behind real changes.
- **A cooperative task scheduler.** Instead of one long `loop()` with timers sprinkled everywhere, every job (read distance, read light, check IR, update display, publish telemetry) is a task with its own interval. It's a lightweight version of what a real RTOS does, and it means nothing blocks anything else.
- **A hardware watchdog.** If the code ever hangs, the watchdog timer resets the board automatically — no one has to walk over and power-cycle it.
- **Fault detection.** If the distance sensor stops reporting valid readings, the system doesn't just sit there with stale data — it notices, tells you, and if it doesn't recover, resets itself cleanly.
- **Power-loss-safe settings.** The unit preference (cm/inches) is saved to EEPROM with a checksum, so a corrupted write on power loss falls back to a safe default instead of booting into garbage.
- **Interrupts done safely.** Both the button and the ultrasonic echo are handled via hardware interrupts that do the absolute minimum inside the interrupt itself (just flag-setting), with all the real logic handled safely outside — the way you're taught to do it, and the way it actually needs to be done.

---

## 🚀 Getting it running

1. Wire everything up per the table above.
2. Install the Arduino libraries: `LiquidCrystal`, `IRremote`, `Servo` (all available via Library Manager).
3. Open `SenseCore_Advanced.ino`, hit upload.
4. Watch the LCD say hello, then walk your hand toward the sensor and watch the gate, lights, and buzzer respond in real time.
5. Open the Serial Monitor at 115200 baud to see live JSON telemetry streaming out — perfect if you ever want to hook this up to a laptop dashboard.

---

## 🎓 Why this project matters

This started as a single-sensor Arduino tutorial and grew into something that actually mirrors how embedded systems get built in industry: multiple sensors feeding a coordinated decision engine, real-time responsiveness without blocking calls, graceful handling of sensor failure, and safe persistent storage. Small board, real ideas — the same ones behind smart entry systems, industrial safety curtains, and IoT devices everywhere.
