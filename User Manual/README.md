# User Manual

## Firmware Flashing

If you have a new puck.js, you will have to flash the firmware onto it. You can do this by the following steps:

1. Open the [Espruino Web IDE](https://www.espruino.com/ide/) in your browser **with Web Bluetooth support** (e.g. Chrome).
2. **Connect** the Puck.js in the IDE via Web Bluetooth (Top-left button: <img alt="" src="wb-icon.png" width="5%"/>)
4. **Open** the firmware [main.js](https://inclusion-international.github.io/Custom-Game-Station/src/espruino/main.js) in the Espruino Web IDE.
5. **Click** the "**Send to Espruino (Flash)** <img alt="" src="send-flash.png" width="5%"/>" button in the IDE to upload the firmware to your Puck.js.
   1. When the process is done, a message should appear in the console log on the left side.
6. **Disconnect** Puck.js from the Espruino IDE.

## Pairing as Mouse/Keyboard

In order to use the custom game station as a **mouse/keyboard** device you must **pair** it as a **Bluetooth device in the OS Bluetooth device manager**.

1. **Check** the device is **not connected** via Web Bluetooth or not connected to any other computer or smartphone.
2. **Connect** it via OS Bluetooth manager of your PC or smartphone/tablet.
   
Now the Game-Station is ready to be used.

## 🛠️ Mounting Specifications

Follow these instructions to correctly assemble and install the Custom Game Station on the wheelchair.

### 1. Wheelchair Preparation

* **Removal:** Carefully remove the original joystick handle from the wheelchair's joystick shaft.

### 2. Component Assembly

1. Place the elastic band over the wheelchair joystick shaft.
2. Hold the elastic band in place and press the custom joystick onto the shaft so that the marked dot points forward from the user's perspective.
3. Turn the joystick power switch **ON (Position 1)**.
    * In the center position and **Position 2**, the joystick is disconnected from the battery.
4. **Charging the device (optional):**
    * Attach the magnetic charging cable to the bottom of the joystick.
    * Connect the USB end to a computer.
    * Ensure that the joystick power switch is in **Position 1** so the battery is connected and charging can occur.
5. Attach the **Knee Button Holder** underneath the wheelchair control unit and secure it using the bolt.
6. Insert the **Puck.js** into its designated slot inside the **Knee Button Holder**.

### 3. Computer Preparation

1. Enable **Bluetooth** on the computer.
2. Connect to the new device named **XIAO-HID**.
3. Once connected, the system is ready for use.

### 4. Operation Modes

#### Joystick Mode

The joystick emulates keyboard movement controls:

* Move **up** → Presses **W**
* Move **down** → Presses **S**
* Move **right** → Presses **D**
* Move **left** → Presses **A**

#### Mouse Mode

* Press and hold the **Puck.js** button to switch joystick functionality into **mouse control mode**.

### 5. Recommended Accessories

For an improved gaming experience, additional external buttons are recommended for:

1. **Jump**
2. **Crouch**
3. **Object Selection / Interaction**

