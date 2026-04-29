#include <LSM6DS3.h>
#include <Wire.h>
#include <MadgwickAHRS.h>

// =====================
// HARDWARE OBJECTS
// =====================

// IMU sensor on I2C (address 0x6A)
LSM6DS3 imu(I2C_MODE, 0x6A);

// Sensor fusion filter (combines gyro + accel → orientation)
Madgwick filter;


// =====================
// USER TUNING (IMPORTANT FOR FEEL)
// =====================

// Dead zone: ignore very small tilt (removes noise)
// Too large → feels unresponsive / laggy
const float DEAD_ZONE = 5.0;

// Sensitivity: how quickly movement ramps up after dead zone
// Lower value = more sensitive
const float sensitivity = 2.0;

// Maximum output speed (limits extreme motion)
const float max_speed = 25.0;


// =====================
// TIMING (CRITICAL FOR LATENCY)
// =====================

// How often the Madgwick filter is updated
// This directly affects responsiveness and smoothness
const float FILTER_RATE = 100.0;   // Hz

// How often we output movement (like Puck.js setInterval)
const int TILT_INTERVAL = 25;      // ms (~40 Hz)


// =====================
// CALIBRATION STORAGE
// =====================

// Stores the "neutral" orientation
float x_calib = 0;
float y_calib = 0;


// =====================
// TIMERS
// =====================

// Last time filter was updated (microseconds)
unsigned long lastFilter = 0;

// Last time movement was processed (milliseconds)
unsigned long lastTilt = 0;


// =====================
// READ IMU DATA
// =====================

inline void readIMU(float &ax, float &ay, float &az,
                    float &gx, float &gy, float &gz) {

  // Convert raw accelerometer data → g units
  ax = imu.readRawAccelX() * 0.061 / 1000.0;
  ay = imu.readRawAccelY() * 0.061 / 1000.0;
  az = imu.readRawAccelZ() * 0.061 / 1000.0;

  // Convert raw gyroscope data → degrees/sec
  gx = imu.readRawGyroX() * 8.75 / 1000.0;
  gy = imu.readRawGyroY() * 8.75 / 1000.0;
  gz = imu.readRawGyroZ() * 8.75 / 1000.0;

  // ⚠️ Note:
  // This performs multiple I2C reads → relatively slow
  // Can be optimized later with burst reads or FIFO
}


// =====================
// CALIBRATION
// =====================

void calibrate() {
  float ax, ay, az, gx, gy, gz;

  // Read sensor multiple times to stabilize readings
  // (startup data is often noisy)
  for (int i = 0; i < 50; i++) {
    readIMU(ax, ay, az, gx, gy, gz);
    delay(2);
  }

  // Compute orientation using accelerometer only
  // Good enough for defining "zero position"
  float pitch = atan2(ax, sqrt(ay * ay + az * az)) * 180.0 / PI;
  float roll  = atan2(ay, sqrt(ax * ax + az * az)) * 180.0 / PI;

  // Store calibration offsets
  x_calib = pitch;
  y_calib = roll;
}


// =====================
// MOVEMENT LOGIC (Puck.js style)
// =====================

void updateMovement(float pitch, float roll) {

  // Difference from calibrated neutral position
  float dx = pitch - x_calib;
  float dy = roll  - y_calib;

  // Dead zone removes small jitter/noise
  if (abs(dx) < DEAD_ZONE) dx = 0;
  if (abs(dy) < DEAD_ZONE) dy = 0;

  // Convert tilt amount → speed
  float speed_roll  = abs(dx) - sensitivity;
  float speed_pitch = abs(dy) - sensitivity;

  // Clamp negative speeds to zero
  if (speed_roll < 0) speed_roll = 0;
  if (speed_pitch < 0) speed_pitch = 0;

  // Limit maximum speed
  if (speed_roll > max_speed) speed_roll = max_speed;
  if (speed_pitch > max_speed) speed_pitch = max_speed;

  // Apply direction (positive / negative)
  int x = dx > 0 ? speed_roll : (dx < 0 ? -speed_roll : 0);
  int y = dy > 0 ? speed_pitch : (dy < 0 ? -speed_pitch : 0);

  // Only output when movement actually exists
  // → reduces CPU load and serial bottleneck
  if (x || y) {
    Serial.print("X:");
    Serial.print(x);
    Serial.print(" Y:");
    Serial.println(y);
  }
}


// =====================
// SETUP
// =====================

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Initialize IMU
  if (imu.begin() != 0) {
    while (1); // Stop if IMU fails
  }

  // 🔥 CRITICAL:
  // Set correct filter update rate
  // Without this → unstable and laggy orientation
  filter.begin(FILTER_RATE);

  // Capture neutral orientation
  calibrate();
}


// =====================
// MAIN LOOP (NON-BLOCKING)
// =====================

void loop() {

  float ax, ay, az, gx, gy, gz;

  // =========================================
  // 1. FAST LOOP → SENSOR + FILTER (~100 Hz)
  // =========================================
  // Keeps orientation data fresh and stable

  if (micros() - lastFilter >= (1000000.0 / FILTER_RATE)) {

    lastFilter = micros();

    // Read IMU data
    readIMU(ax, ay, az, gx, gy, gz);

    // Update orientation filter
    // Gyro must be in radians/sec
    filter.updateIMU(
      gx * DEG_TO_RAD,
      gy * DEG_TO_RAD,
      gz * DEG_TO_RAD,
      ax, ay, az
    );
  }


  // =========================================
  // 2. SLOW LOOP → OUTPUT (~40 Hz)
  // =========================================
  // Equivalent to Puck.js setInterval()

  if (millis() - lastTilt >= TILT_INTERVAL) {

    lastTilt = millis();

    // Get computed orientation
    float pitch = filter.getPitch();
    float roll  = filter.getRoll();

    // Convert orientation → movement
    updateMovement(pitch, roll);
  }
}
