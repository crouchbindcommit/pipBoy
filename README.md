# PIPBoy MARK I – Wearable Information Hub

### **Overview**
The **PIPBoy MARK I** is a wearable device designed to be the ultimate companion in the post-apocalyptic world, inspired by the iconic **PIPBoy** from the *Fallout* video game series. In a world ravaged by nuclear disaster, this device serves as a **survival companion** by offering essential functions such as real-time weather, location tracking, health monitoring, and more—all in a compact and user-friendly interface.

This project demonstrates the development of an **embedded system** that functions seamlessly without an operating system, offering greater space efficiency and performance. The device uses a **microcontroller-based system** to replicate the key features of the in-game PIPBoy.

---

### **Features**
- **Real-Time Weather Fetching**: The device can fetch real-time weather data to help users plan for outdoor activities in any environment.
- **Health Monitoring**: Provides vital health statistics and real-time health updates.
- **Real-Time Clock (RTC)**: Keeps track of the time, much like the PIPBoy from the *Fallout* universe.
- **Audio Output**: Plays ambient audio, music, and news to keep the wearer informed.
- **Location Tracking**: Integrates GPS data for location tracking and mapping features.

---

### **Technologies Used**
- **Microcontroller-based System**: No operating system is used, ensuring faster performance and higher space efficiency.
- **Finite State Machine**: Used for managing different operational modes (e.g., weather, time, audio).
- **I2C Communication**: To interface with the **LCD display** and **Real-Time Clock (RTC)**.
- **UART Communication**: Used for debugging and WiFi communication.
- **8-bit Binary Weighted DAC**: Implemented for high-quality sound output.
- **API Fetching**: To fetch weather updates and other real-time data.
- **Timer Interrupts**: Ensures responsive and real-time updates during operation.

