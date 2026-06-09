# 📡 SenseCore

<p align="left">
  <img src="https://img.shields.io/badge/stack-Arduino%20Uno%20%7C%20Sensors%20%7C%20Embedded%20C++-blue?style=for-the-badge" />
  <img src="https://img.shields.io/badge/system-Real%20Time%20Embedded%20Interaction-green?style=for-the-badge" />
  <img src="https://img.shields.io/badge/interface-IR%20%7C%20LCD%20%7C%20LED%20Control-orange?style=for-the-badge" />
</p>

## 🧠 Embedded Intelligent Object Detection & Interaction System

SenseCore is a real-time embedded Arduino system that integrates **multi-sensor input processing, environmental awareness, and user interaction** into a single interactive system.

It demonstrates core embedded engineering principles including **sensor fusion, state-based logic, and real-time hardware-software integration**.

---

## ⚙️ System Capabilities

### 📏 Real-Time Distance Sensing
- HC-SR04 ultrasonic sensor
- Continuous object proximity detection
- Threshold-based safety evaluation

### 🚨 Adaptive Warning System
- Multi-level LED indicators (safe / warning / critical)
- Distance-based alert logic
- Dynamic blinking behavior

### 🌗 Ambient Light Adaptation
- LDR (photoresistor) sensing
- Automatic LED brightness control via PWM

### 🎮 Multi-Modal User Interaction
- IR remote control input
- Push-button override system
- Mode switching functionality

### 📟 Embedded Display System
- 16x2 LCD live output
- Real-time sensor visualization
- System state feedback

---

## 🧠 System Architecture

**Input Layer**
- Ultrasonic sensor (HC-SR04)
- LDR light sensor
- IR receiver module
- Push button input

**Processing Layer**
- Real-time loop execution
- Threshold-based decision logic
- State machine transitions

**Output Layer**
- LED warning system
- LCD display output
- PWM brightness control

---

## 🔄 System Behavior Flow

1. Ultrasonic sensor continuously measures distance  
2. System evaluates safety thresholds  
3. LED state updates dynamically  
4. LCD displays live system status  
5. IR remote modifies system behavior  
6. Button triggers manual override/unlock  

---

## 🏗️ Hardware Architecture

- Arduino Uno microcontroller  
- HC-SR04 ultrasonic sensor  
- LDR (photoresistor)  
- IR receiver module  
- 16x2 LCD display  
- LEDs (warning + error + light control)  
- Push button input  

---

## 🔌 Pin Mapping

- Echo → D3  
- Trigger → D4  
- IR Receiver → D5  
- Warning LED → D11  
- Error LED → D12  
- Light LED (PWM) → D10  
- Button → D2  
- LDR → A0  
- LCD → A4 / A5 + D6–D9  

---

## ⚡ Key Engineering Concepts

✔ Real-time embedded C++ programming  
✔ Sensor fusion (analog + digital inputs)  
✔ State machine-based system design  
✔ PWM-based brightness control  
✔ Interrupt-driven sensing (echo timing)  
✔ Human-machine interaction systems  

---

## 🧪 System Constraints

- Ultrasonic sensor noise and fluctuation  
- IR signal interference in ambient environments  
- Arduino Uno memory limitations  
- Timing-sensitive loop execution  
- LCD refresh rate constraints  

---

## 🚀 Future Improvements

- Replace polling with interrupt-driven architecture  
- Add Kalman filter for distance smoothing  
- Upgrade to ESP32 for WiFi connectivity  
- Add SD card / EEPROM logging system  
- Modular firmware refactoring (drivers + controllers separation)  

---

## 📈 Why SenseCore Matters

SenseCore demonstrates how embedded systems evolve into **intelligent interactive sensing platforms** by combining:

- Real-time signal processing  
- Hardware-software integration  
- Multi-input decision making  
- Reactive control systems  

---

## 📜 License

MIT License
