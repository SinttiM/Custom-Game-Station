BLE PROTOCOL SPECIFICATION — CURRENT SYSTEM (Puck.js → Joystick → PC)
Version: 2.0
Date: 2026‑06‑27
Author: Microsoft Copilot (based on provided source code)
Devices:

Puck.js (Mode Controller)

XIAO nRF52840 Sense (Joystick + HID Peripheral)

PC / Host (BLE HID Central)


1. System Overview
The system consists of two BLE links:

Puck.js → Joystick (XIAO)

Puck.js acts as a BLE Peripheral

Joystick acts as a BLE Central

Purpose: Send mode changes (Mouse / Keyboard)

Joystick (XIAO) → PC

Joystick acts as a BLE HID Peripheral

PC acts as a BLE Central (HID Host)

Purpose: Send mouse and keyboard HID reports

There is no custom BLE service between Joystick and PC — only HID.


2. Puck.js → Joystick BLE Protocol

2.1 Roles
Device	Role
Puck.js	BLE Peripheral
Joystick (XIAO)	BLE Central


2.2 Service Definition
Service: ModeService
UUID: df71f80f-0000-0b00-0a4f-a4908e7d86ee

128‑bit, little‑endian in XIAO code

Characteristic: ModeState
UUID: df71f80f-0001-0b00-0a4f-a4908e7d86ee

Format: uint8 (1 byte)

Properties:

Notify

Read

Characteristic Values
Value	Meaning
0	    Mouse Mode
1	    Keyboard/Gamepad Mode


2.3 Advertising
Puck.js advertises:

Name: "Pck"

Interval: 500 ms

Connectable: Yes

Discoverable: Yes

Joystick scans for "Pck" by name.


2.4 Connection Behavior
On Connect (Puck.js)
LED2 pulse

Sets connection interval to 100–200 ms

Sends current mode immediately via notification

On Disconnect (Puck.js)
LED1 pulse

No further action

2.5 Joystick Behavior
Scan Logic
Joystick scans for "Pck":

cpp
if (nameLen == 3 && memcmp(nameBuf, "Pck", 3) == 0)
    Bluefruit.Central.connect(report);
GATT Discovery
After connection:

Discover service UUID

Discover characteristic UUID

Enable notifications

Register callback notifyCb

Notification Callback
cpp
mode = data[0];  // 0 = Mouse, 1 = Keyboard
If switching to Mouse mode, joystick clears all key states.

2.6 Reconnection Logic
If Puck disconnects:

Joystick clears puckHandle

Restarts scanning immediately

No delay or backoff


3. Joystick → PC BLE Protocol (HID)

3.1 Roles
Device	    Role
Joystick    (XIAO)	BLE HID Peripheral
PC	        BLE Central (HID Host)


3.2 HID Services
Joystick exposes a combined HID Mouse + Keyboard service using Adafruit Bluefruit HID.

Report Descriptor
Contains:

Mouse Report (Report ID = 1)

Keyboard Report (Report ID = 2)

Mouse Report Format
Field	    Type	Description
Buttons	    uint8	Always 0
X	        int8	-127 to +127
Y	        int8	-127 to +127
Wheel	    int8	Always 0


Keyboard Report Format
Field	        Type	    Description
Modifiers	    uint8	    Always 0
Reserved	    uint8	    Always 0
Keycodes[6]	    uint8[6]	W, A, S, D mapping


3.3 Advertising
Joystick advertises:
HID Mouse appearance
HID service
Device name "XIAO-HID"
Advertising restarts automatically on PC disconnect.

3.4 Connection Behavior
On Connect (PC)
cpp
pcConnHandle = handle;
conn->requestConnectionParameter(6, 12);
On Disconnect (PC)
Clear key states
Restart advertising
No backoff or delay

4. Joystick Input Processing

4.1 IMU Sampling
IMU read interval: 15 ms (≈66 Hz)
Reads:
ax = accelX
ay = accelY

4.2 Mode 0 — Mouse Mode
Deadzone Logic
Up deadzone: 0.10f
Down deadzone: 0.45f
X deadzone: 0.33f
Scaling
Code
moveX = ax * 30
moveY = ay * 30
Sending
Only sends HID report if movement ≠ 0.

4.3 Mode 1 — Keyboard Mode
Mapping
valX = ax * 127
valY = ay * 127

Press Thresholds
Direction	Press	Release
Up (W)	    20	    10
Down (S)	35	    20
Left (A)	30	    15
Right (D)	30	    15

Hysteresis
Release thresholds are always lower than press thresholds.

Sending
Keyboard report is sent only when state changes.


5. Error Handling & Recovery

5.1 Puck Disconnect
Joystick resumes scanning
Mode remains unchanged
No fallback mode forced

5.2 PC Disconnect
HID reports stop
Advertising restarts
Key states reset

5.3 IMU Failure
System halts (while(1);)
No fallback behavior

6. UUID Summary
Component	UUID
Puck Mode Service	df71f80f-0000-0b00-0a4f-a4908e7d86ee
Puck Mode Characteristic	df71f80f-0001-0b00-0a4f-a4908e7d86ee
HID Service (Joystick → PC)	Standard HID UUIDs (Adafruit Bluefruit)


7. Protocol Summary Diagram
Code
Puck.js (Peripheral)
    |
    |  Notify: mode (0/1)
    v
Joystick (Central + HID Peripheral)
    |
    |  HID Mouse + Keyboard Reports
    v
PC (HID Host)
