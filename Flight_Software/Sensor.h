#include <Adafruit_BNO055.h>
#include <Adafruit_Sensor.h>
#include "BMI088.h"
#include <MS5611.h>
#include <Wire.h>
#include <Arduino.h>
#include "Vector.h"
#include <Orientation.h>    // Orientation library for calculating attitude

class Sensors{
  public:
    Sensors();
    Vector3 getGyros();
    Vector3 getAccel();
    Vector3 getAttitude();
    Vector4 getAltitude();
  private:
    double temp;
    float pressure = 0.0;
    float relativeAltitude;
    float altitude = 0.0;
    double referencePressure;
    long realPressure;
    float yaw;
    float pitch;
    float roll;
};
