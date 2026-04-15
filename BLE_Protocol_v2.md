
1. PUCK.JS → JOYSTICK BLE PROTOCOL
Device Role
Puck.js = BLE Peripheral

Joystick XIAO = BLE Central

1.1 Service Definition
Service Name: ButtonService
UUID: 0xA001
(You may replace with a 128‑bit UUID later if needed.)

1.2 Characteristics
Characteristic: ButtonState
UUID: 0xA002

Format: uint8

Values:

0 = button released

1 = button pressed

Properties:

Notify (Puck.js → Joystick)

Read (optional)

1.3 Update Behavior
Puck.js sends a notification immediately when the button changes state.

No periodic updates needed.

Expected latency: 8–15 ms.

2. JOYSTICK → DONGLE BLE PROTOCOL
Device Roles
Joystick XIAO = BLE Peripheral

Dongle XIAO Sense = BLE Central

2.1 Service Definition
Service Name: GamepadService
UUID: 0xB001
2.2 Characteristics
Characteristic: ControlPacket
UUID: 0xB002

Format: Binary struct (5 bytes total)

Code
struct ControlPacket {
    uint8 mode;     // 0 = movement, 1 = camera
    int16 x;        // joystick X axis (-32768 to 32767)
    int16 y;        // joystick Y axis (-32768 to 32767)
}
Properties
Notify (Joystick → Dongle)

Read (optional)

2.3 Update Rate
Joystick sends packets at 20–50 Hz (recommended: 30 Hz)

Only send when:

joystick moves, or

mode changes

2.4 Expected Latency
BLE transmission: 8–15 ms

USB HID: 1–4 ms

Total joystick → PC: 10–20 ms

3. CONNECTION LOGIC
3.1 Puck.js → Joystick
Joystick scans for ButtonService

Connects automatically

Subscribes to ButtonState

Updates internal mode variable

3.2 Joystick → Dongle
Dongle scans for GamepadService

Connects automatically

Subscribes to ControlPacket

Converts packets to HID events

4. ERROR & RECONNECTION RULES
If Puck.js disconnects
Joystick switches to Movement Mode (0) by default

Continues sending joystick data

Attempts reconnection every 1 second

If Joystick disconnects from Dongle
Dongle sends no HID input

Attempts reconnection every 1 second

5. UUID SUMMARY TABLE
Component	Name	UUID
Service	ButtonService	0xA001
Characteristic	ButtonState	0xA002
Service	GamepadService	0xB001
Characteristic	ControlPacket	0xB002


(You can switch to 128‑bit UUIDs later if needed.)
