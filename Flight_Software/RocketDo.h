#include <Wire.h>
#include <Arduino.h>
#include "Sensor.h"
#include <Servo.h>
#include <PID_v1.h>         // PID control library for control system calculations

class Rocket{
  public:
    Rocket();
    Vector3 ypr;
    Vector3 gyros;
    Vector3 accels; // x accel is vertical
    Vector4 baro;
    void updateState();
    void pad_idle();
    void ascent();
    void apogee();
    void coast();
    void descent();
    void landed();
    void set();
    unsigned long flight_time = 0.0;
    unsigned long flight_start = 0.0;
    float servo_z = 0.0;
    float servo_y = 0.0;
    float voltage = 0.0;
    double Outputx = 0.0;
    double Inputx = 0.0; 
    double Outputy = 0.0; 
    double Inputy = 0.0;
    double Setpointx = 0;
    double Setpointy = 0;
    int parachute = 0;
  private:
    Sensors sensors;
    unsigned long current_t = 0.0;
    unsigned long previous_t = 0.0;
    float del_t = 0.0f;
    Servo y_axis;
    Servo z_axis;
    Servo chute; 
    float PID_LIM = 10; // this is the output limit of our system
    PID PIDx = PID(&Inputx, &Outputx, &Setpointx, 1, 0.1, 0.1, DIRECT);
    PID PIDy = PID(&Inputy, &Outputy, &Setpointy, 1, 0.1, 0.1, DIRECT);
  };
