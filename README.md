# 📡 SenseCore  
### Embedded Intelligent Object Detection & Interaction System

---

## 📌 Overview

SenseCore is a real-time embedded system built on Arduino that integrates multi-sensor input processing, environmental awareness, and user interaction through both physical and remote interfaces.

The system demonstrates core embedded engineering principles including sensor fusion, real-time decision logic, and hardware-software integration for interactive environments.

---

## ⚙️ System Capabilities

### 📏 Real-Time Distance Sensing
- Ultrasonic sensor (HC-SR04) for object proximity detection  
- Continuous distance monitoring with threshold-based alerts  

### 🚨 Adaptive Warning System
- Multi-level LED feedback (normal / warning / critical)  
- Distance-based decision logic for safety signaling  

### 🌗 Ambient Light Adaptation
- Photoresistor (LDR) dynamically adjusts LED brightness  
- Enables energy-efficient visual feedback control  

### 🎮 Multi-Modal User Interaction
- IR remote control input handling  
- Physical push-button input for local control overrides  

### 📟 Embedded Display System
- 16x2 LCD displays real-time system state and sensor readings  

---

## 🧠 Embedded System Design

SenseCore implements a real-time event-driven architecture:

### 📥 Input Layer
- Ultrasonic distance sensor  
- Photoresistor (analog light sensing)  
- IR receiver module  
- Button interrupt input  

### ⚙️ Processing Layer
- Sensor polling loop (real-time sampling)  
- Threshold-based decision logic  
- Conditional state transitions  

### 📤 Output Layer
- LED signaling system (multi-state indicators)  
- LCD display updates  
- Brightness modulation via PWM  

---

## 🔄 System Behavior Flow

- Ultrasonic sensor continuously measures distance  
- System evaluates proximity thresholds:
  - Safe zone → normal LED state  
  - Warning zone → amber LED activation  
  - Critical zone → error LED trigger  
- Photoresistor adjusts LED brightness dynamically  
- IR remote modifies system state or modes  
- Button input overrides or resets system state  
- LCD displays live sensor readings + system status  

---

## 🏗️ Hardware Architecture

**Microcontroller:** Arduino Uno  

### Sensors
- HC-SR04 Ultrasonic Sensor  
- LDR (Photoresistor)  
- IR Receiver Module  

### Actuators
- RGB / discrete LEDs  
- LCD 16x2 display  

### Inputs
- IR Remote  
- Push button  

---

## 🔌 Pin Mapping

- IR Receiver → D5  
- Ultrasonic Echo → D3  
- Ultrasonic Trigger → D4  
- Warning LED → D11  
- Error LED → D12  
- Light LED (PWM) → D10  
- Button → D2  
- Photoresistor → A0  
- LCD → A4 / A5 + D6–D9  

---

## ⚡ Engineering Concepts Demonstrated

- Embedded C++ programming on microcontrollers  
- Real-time sensor data acquisition  
- Analog + digital signal integration  
- PWM-based brightness control  
- State machine-based system design  
- Human-machine interaction systems  
- Hardware debugging and iterative validation  

---

## 🧪 System Challenges & Constraints

- Ultrasonic sensor noise and measurement instability  
- IR signal interference in ambient environments  
- Timing sensitivity in loop-based control systems  
- Limited memory and processing constraints of Arduino Uno  
- LCD refresh rate vs sensor update rate tradeoffs  

---

## 🚀 Potential Improvements

- Replace polling loop with interrupt-driven architecture  
- Add Kalman filtering for ultrasonic noise reduction  
- Upgrade to ESP32 for WiFi-enabled monitoring  
- Add data logging for long-term behavior analysis  
- Implement modular firmware structure (drivers + controllers separation)  

---

## 📈 Why this project matters

SenseCore demonstrates:

- Embedded system design thinking  
- Multi-sensor integration and control logic  
- Real-time hardware-software interaction  
- Practical firmware development skills  
- Foundational robotics / IoT engineering principles  

---

## 🔥 Key Takeaway

This project evolves the perception from:

> “Arduino project with sensors”

to

> “Real-time embedded interactive sensing system with multi-input control and adaptive output behavior”

---
