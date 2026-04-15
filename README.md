# Custom-Game-Station

Project results of "ASSIST HEIDI - Designing and implementing Assistive Tools for people with disabilities" course.

A Custom Game Station designed for specifical needs of our co-designer. It is a multifunctional adaptive system projected to replace standard joystick and triggers. Specifically, the user requested a customized system to play "Blue Prince", a game requiring strategic navigation and interaction, but can be adapted to easily play multiple games.

## Product overview

This solution is composed of 1 Puck.js microcontroller (in Joystick), which could be easily connected wirelessly to a PC through Web Bluetooth. Connected to this, 2 switch buttons (T: Top Button, S: Side Button) and a custom 3D printed support. The solution is meant to be applied on a wheelchair.

 <img src="Photos/IMG_3863.png" width="34%">  
 <img src="Photos/IMG_3861.png" width="60%"> 

## Key Features

### Wheelchair Joystick

* Could work both as **PC mouse** or **WASD keys** on keyboard, it depends on the selected modality
* Could be recalibrated every time needed
* Adjustable Sensitivity: Settings to adjust joystick sensitivity to individual needs.

### Top Cover

* Holds the batteries Battery Pack (B) for the Puck.js
* Hosts the Top Button (T)
* Hosts a plug-in connector to an additional button, the Side-Knee-Button

### Side-Knee-Button (S)

* Is mounted on a Side-Knee-Holder, meant to be attached on the inner side of the wheelchair
* Easy to use, convenient positioning and easy operation with minimal effort


## Technical specifications

* Microcontroller: [Puck.js](https://www.puck-js.com/) with Bluetooth Low Energy (BLE) support and very low power consumption.
* Power Supply: Battery-powered with **two AAA Alkaline (1.5 V)** batteries: **3V needed**
  * two 1,2V rechargable NiMh batteries don't work!
  * two [1,5V rechargeable Li Ion AAA batteries](https://www.amazon.de/wiederaufladbare-Lithium-1300mWh-langlebige-Batterien/dp/B0FR4LBB2Z/ref=asc_df_B0FR4LBB2Z?mcid=a224817bdf743904875bd3077be37424&language=de_DE&tag=detxtgostdde-21&linkCode=df0&hvadid=718119058994&hvpos=&hvnetw=g&hvrand=7233695344780195382&hvpone=&hvptwo=&hvqmt=&hvdev=c&hvdvcmdl=&hvlocint=&hvlocphy=20044&hvtargid=pla-2459684705217&psc=1&language=de_DE&gad_source=1) can be used.
* Compatibility: Compatible with Windows, macOS, and Linux operating systems
* Range: Up to 10 meters wireless range.

## Material

* 1x Puck.js microcontrollers
* Glue
* 5x Pair of small connectors
* 2x Switch buttons
* 1x Jack Connector
* 3D printed devices (Joystick Top, Joystick cap, Top Cover, Side Knee holder, Side Knee Button holder)
* Elastic band 10 mm thick

## User Documentation

* [Additional information - About our Project](Additional_Information.pdf)
* [User Manual](User%20Manual/)
* [CAD files](CAD%20Files/)


**VERSION 2.0**

Architecture diagram
```mermaid
architecture-beta
    group api(cloud)[API]

    service db(database)[Database] in api
    service disk1(disk)[Storage] in api
    service disk2(disk)[Storage] in api
    service server(server)[Server] in api

    db:L -- R:server
    disk1:T -- B:server
    disk2:T -- B:db
```
High-level

 ┌──────────────────┐        BLE         ┌────────────────────┐        BLE         ┌────────────────────────┐
 │   Puck.js Button  │ ─────────────────▶ │  Joystick Unit      │ ─────────────────▶ │   XIAO nRF52840 Sense   │
 │ (BLE Peripheral)  │                    │ (BLE Central +      │                    │     (Dongle, BLE        │
 │ Sends: 0/1 state  │                    │  BLE Peripheral)    │                    │      Central + USB HID) │
 └──────────────────┘                    │ Reads joystick X/Y  │                    │ Sends HID to PC         │
                                          │ Switches mode       │                    └─────────────┬──────────┘
                                          └────────────────────┘                                  USB
                                                                                                    │
                                                                                                    ▼
                                                                                           ┌─────────────────┐
                                                                                           │      PC          │
                                                                                           │ (Game receives   │
                                                                                           │  HID input)      │
                                                                                           └─────────────────┘


Detailed Architecture Diagram (with roles & data flow)

──────────────────────────────────────────────────────────────────────────────────────────────
   DEVICE 1: PUCK.JS BUTTON
──────────────────────────────────────────────────────────────────────────────────────────────
   • BLE Peripheral
   • Exposes GATT service: ButtonService
   • Characteristic: ButtonState (0 = released, 1 = pressed)
   • No joystick logic — only a wireless button

   Output:
       ButtonState → sent via BLE to Joystick Unit
──────────────────────────────────────────────────────────────────────────────────────────────

                                      BLE LINK #1
                    (ButtonState notifications: pressed / released)
──────────────────────────────────────────────────────────────────────────────────────────────

──────────────────────────────────────────────────────────────────────────────────────────────
   DEVICE 2: JOYSTICK UNIT — XIAO nRF52840
──────────────────────────────────────────────────────────────────────────────────────────────
   Roles:
   • BLE Central (connects to Puck.js)
   • BLE Peripheral (advertises to Dongle)
   • Reads analog joystick X/Y
   • Switches between:
         MODE 0 = Movement (WASD or left stick)
         MODE 1 = Camera (mouse or right stick)

   Internal Logic:
       - Read Puck.js ButtonState
       - If pressed → switch to Camera Mode
       - If released → switch to Movement Mode
       - Read joystick X/Y
       - Build ControlPacket:
             mode (0/1)
             x (int16)
             y (int16)
       - Send ControlPacket to Dongle via BLE

   Output:
       ControlPacket → sent via BLE to Dongle
──────────────────────────────────────────────────────────────────────────────────────────────

                                      BLE LINK #2
                     (ControlPacket: mode + joystick X/Y values)
──────────────────────────────────────────────────────────────────────────────────────────────

──────────────────────────────────────────────────────────────────────────────────────────────
   DEVICE 3: DONGLE — XIAO nRF52840 SENSE
──────────────────────────────────────────────────────────────────────────────────────────────
   Roles:
   • BLE Central (connects to Joystick Unit)
   • USB HID Device (connected to PC)

   Responsibilities:
       - Receive ControlPacket from Joystick
       - Convert to HID events:
             If mode = Movement → WASD or Gamepad Left Stick
             If mode = Camera   → Mouse movement or Gamepad Right Stick
       - Send HID reports to PC via USB

   Output:
       HID Input → PC (mouse, keyboard, or gamepad)
──────────────────────────────────────────────────────────────────────────────────────────────

──────────────────────────────────────────────────────────────────────────────────────────────
   DEVICE 4: PC (Game)
──────────────────────────────────────────────────────────────────────────────────────────────
   • Receives HID input from Dongle
   • Moves character or camera based on mode
──────────────────────────────────────────────────────────────────────────────────────────────

