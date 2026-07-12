# 🤖 SenseCore
### A little Arduino system that senses, thinks, and reacts — all at once

## 👋 What is this?

Most beginner Arduino projects teach you one thing at a time: blink an LED, read a sensor, done. Useful, but it doesn't really show you what embedded systems are actually like in the real world — where a smart device isn't watching *one* thing, it's watching several things simultaneously and reacting to all of them in real time.

SenseCore is my attempt to build that fuller picture. It's an Arduino Uno-based system that juggles distance sensing, ambient light detection, infrared remote control, a push button, LED indicators, and an LCD display — all running together, all the time, reacting to the world around it almost instantly.

## ❓ Why I built this

I wanted to move past "one sensor, one output" tutorials and actually build something that felt like a real embedded product — the kind where multiple sensors feed into one coordinated system, each one able to change what the others are doing. Getting distance sensing, light detection, IR input, and manual button control all working together (without stepping on each other) taught me way more about real-time systems than any single-sensor project could.

## 🚀 What it actually does

Once it's powered on, SenseCore runs continuously, doing all of this in a tight loop:

1. **Measures distance** using an ultrasonic sensor, and classifies it as safe, warning, or critical.
2. **Updates LED indicators** instantly to reflect that proximity level.
3. **Reads ambient light** with a photoresistor, and automatically adjusts LED brightness (via PWM) to match the room.
4. **Displays live readings** — distance, light level, system status — on a 16×2 LCD.
5. **Listens for an IR remote**, letting you switch the system's operating mode from across the room.
6. **Watches a push button** for manual overrides.

All of this happens in the same loop, continuously, so the system feels responsive rather than laggy or one-track-minded.

## 📈 A quick example

Picture this mounted near a doorway or workspace, quietly keeping an eye on things.

As someone walks closer, the ultrasonic sensor picks up the shrinking distance, and the firmware immediately reclassifies it — safe, warning, or critical — updating the LEDs and LCD in real time. If the room happens to get darker at the same moment, the light sensor notices too, and PWM smoothly ramps up LED brightness for better visibility. Meanwhile, someone could grab the IR remote or hit the push button to switch modes entirely — and the system adapts instantly, without missing a beat on anything else it's tracking.

That's the whole point: not one sensor working alone, but several working together.

## 👥 Who this is for

- Embedded systems or ECE students wanting to see multi-sensor integration in action
- Arduino hobbyists ready to go beyond single-component tutorials
- Robotics enthusiasts curious about real-time sensor fusion
- Anyone learning state machines and event-driven embedded design

## 🛠️ What you'll need to build it

**Hardware:** Arduino Uno, HC-SR04 ultrasonic sensor, LDR (photoresistor), IR receiver module + remote, 16×2 LCD display, LEDs, push button, breadboard, jumper wires, USB cable

**Software:** Arduino IDE, Embedded C++, and Arduino libraries for the LCD, IR receiver, and ultrasonic sensor

**Helpful background:** basic Arduino programming, digital/analog I/O, PWM, and a bit of state machine logic — though the code is written to be approachable even if some of this is new to you.

## 💡 How it flows, visually

```
Power On
     │
     ▼
Initialize Sensors & LCD
     │
     ▼
Read Distance Sensor
     │
     ▼
Read Ambient Light Sensor
     │
     ▼
Check IR Remote Commands
     │
     ▼
Check Push Button State
     │
     ▼
Evaluate System State
     │
     ▼
Update LEDs
     │
     ▼
Adjust PWM Brightness
     │
     ▼
Refresh LCD Display
     │
     ▼
Repeat Continuously
```

## 🎓 What I took away from this

Beyond just wiring up sensors, this project was really about sensor fusion, state-based programming, and hardware interfacing — the same underlying ideas that show up in smart home devices, industrial automation, and IoT products. It's a small system, but the concepts scale up.

## 🧰 Built with

Arduino (C++)
