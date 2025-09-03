# 🛠️ Firmware Notes

This document explains the firmware modifications and configurations made for the Drone 3D Printing Project.  

---

## 🔹 Marlin Firmware (Extruder Control)
- Based on **Marlin 2.x** release.  
- Target board: **Arduino Mega 2560 + RAMPS 1.4**.  
- Changes made:
  - Enabled **Extruder-only mode** (no X/Y/Z axes).  
  - Customized **stepper motor configuration** for lightweight extruder.  
  - Adjusted **temperature sensor** and **heater pin mapping**.  
  - Disabled LCD/SD support to reduce firmware size.  
  - Modified **movement sync** to allow triggering from Raspberry Pi (via serial).  

---

## 🔹 Raspberry Pi Integration
- **Language:** Python (with `pyserial`).  
- Sends G-code commands to Arduino Mega for extrusion control.  
- Synchronizes extrusion with drone movement commands (sent to CUAV X7+ via MAVLink).  
- Added safety check: extrusion disabled if drone altitude < threshold.  

---

## 🔹 CUAV X7+ / Flight Controller
- Stock **PX4 firmware** used (no direct modification).  
- Extended **MAVLink messages** for communication with Raspberry Pi.  
- Raspberry Pi bridges between **PX4** and **Arduino/Marlin**.  

---

## 🔹 Future Improvements
- Direct integration of extruder control into PX4 firmware (remove Arduino).  
- Implement closed-loop control for extrusion based on drone velocity.  
- Optimize G-code parsing for lightweight onboard computation.  
