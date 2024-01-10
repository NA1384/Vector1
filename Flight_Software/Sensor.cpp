#include "Sensor.h"

#define RED_LED              PIN_USB_HOST_ENABLE
#define GREEN_LED            PIN_LED_RXL
#define BLUE_LED             PIN_LED_TXL
#define BUZZER               3

uint64_t thisLoopMicros = 0; // Stores microsecond timestamp for current loop
uint64_t lastOriUpdate = 0; // Stores mircosecond timestamp for last time orientation updated

// Orientation library
Orientation ori; // Main orientation measurement
EulerAngles oriMeasure; // Quaternion converted to Euler Angles for maths etc.

/* accel object */
Bmi088Accel accel(Wire, 0x18);
/* gyro object */
Bmi088Gyro gyro(Wire, 0x68);
// Barometer object
MS5611 ms5611;

Sensors::Sensors() {
  int status;
  //wrapper for BNO class probably
  SerialUSB.println("Initialising Sensors...");
  Wire.begin();
  Wire.setClock(400000);
  /* start the sensors */
  status = accel.begin();
  if (status < 0) {
    SerialUSB.println("Accel Initialization Error");
    SerialUSB.println(status);
    digitalWrite(RED_LED,HIGH);
    digitalWrite(GREEN_LED,LOW);
    digitalWrite(BLUE_LED,LOW);
    tone(BUZZER, 2000);
    delay(1000);
    noTone(BUZZER);
    while (1) {
      delay(100);
      status = accel.begin();
      if (status > 0) break;
    }
  }
  status = gyro.begin();
  if (status < 0) {
    SerialUSB.println("Gyro Initialization Error");
    SerialUSB.println(status);
    digitalWrite(RED_LED,HIGH);
    digitalWrite(GREEN_LED,LOW);
    digitalWrite(BLUE_LED,LOW);
    tone(BUZZER, 1000);
    delay(1000);
    noTone(BUZZER);
    while (1) {
      delay(100);
      status = gyro.begin();
      if (status > 0) break;
    }
  }
  while(!ms5611.begin(MS5611_ULTRA_HIGH_RES))
  {
    SerialUSB.println("Could not find a valid MS5611, check wiring!");
    tone(BUZZER, 1000);
    delay(1000);
    noTone(BUZZER);
    digitalWrite(RED_LED,HIGH);
    digitalWrite(GREEN_LED,LOW);
    digitalWrite(BLUE_LED,LOW); 
    while (1) {}
  }
  referencePressure = ms5611.readPressure();
  SerialUSB.print("Oversampling: ");
  SerialUSB.println(ms5611.getOversampling());
  SerialUSB.println("MS5611 has been initialized...");
  thisLoopMicros = lastOriUpdate = micros(); // Set starting time after init/calibration
}

Vector3 Sensors::getGyros() {
  gyro.readSensor();
  Vector3 V(gyro.getGyroX_rads(), gyro.getGyroY_rads(), gyro.getGyroZ_rads());
  return V;
}

Vector3 Sensors::getAccel() {
  accel.readSensor();
  Vector3 V(accel.getAccelX_mss(), accel.getAccelY_mss(), accel.getAccelZ_mss());
  return V;
}

Vector3 Sensors::getAttitude() {
  thisLoopMicros = micros(); // Get new microsecond timestamp for this loop
  /* read the gyro */
  gyro.readSensor();  
  float dtOri = (float)(thisLoopMicros - lastOriUpdate) / 1000000.; // Finds elapsed microseconds since last update, converts to float, and converts to seconds
  lastOriUpdate = thisLoopMicros; // We have updated, set the new timestamp
  /*
    This is where the magic actually happens

    The order of your axis measurements (x, y, z) will depend on your sensor, your reference frame, and your IMU library of choice
    Swap & invert your gyro measurements so that .update() is called with (yaw, pitch, roll, dt) in that order

    All gyro measurements must be measured right-handed (positive = yaw left, pitch down, roll right) and coverted to radians/sec
  */
  ori.update(gyro.getGyroY_rads(), gyro.getGyroZ_rads(), gyro.getGyroX_rads(), dtOri); // '* DEG_TO_RAD' after all gyro functions if they return degrees/sec
  oriMeasure = ori.toEuler();
  yaw = oriMeasure.yaw * RAD_TO_DEG;
  pitch = oriMeasure.pitch * RAD_TO_DEG;
  roll = oriMeasure.roll * RAD_TO_DEG;
  Vector3 V(yaw, pitch, roll);
  return V;
}

Vector4 Sensors::getAltitude() {  
  pressure = ms5611.readPressure();
  altitude = ms5611.getAltitude(pressure);
  relativeAltitude = ms5611.getAltitude(pressure, referencePressure);
  temp = ms5611.readTemperature();
  Vector4 V(pressure, altitude, relativeAltitude, temp);
  return V;
}
