#include "RocketDo.h"

#define RED_LED              PIN_USB_HOST_ENABLE
#define GREEN_LED            PIN_LED_RXL
#define BLUE_LED             PIN_LED_TXL
#define PYRO_1               0
#define PYRO_2               1        
#define PYRO_3               6
#define PYRO_4               12
#define BUZZER               3
#define Y_SERVO_PIN          9
#define Z_SERVO_PIN          8
#define CHUTE_PIN            4
#define CHUTE_CLOSE          50
#define CHUTE_OPEN           110
#define SERVO_RATIO          6.0f

Rocket::Rocket() {
  //can initialize states and gyros/accel
  pinMode(RED_LED,OUTPUT);
  pinMode(GREEN_LED,OUTPUT);
  pinMode(BLUE_LED,OUTPUT);
  pinMode(PYRO_1,OUTPUT);
  pinMode(PYRO_2,OUTPUT);
  pinMode(PYRO_3,OUTPUT);
  pinMode(PYRO_4,OUTPUT);
  pinMode(BUZZER, OUTPUT);
  Sensors();
  tone(BUZZER, 500);
  delay(500);
  noTone(BUZZER);
  y_axis.attach(Y_SERVO_PIN);
  z_axis.attach(Z_SERVO_PIN);
  chute.attach(CHUTE_PIN);
  y_axis.write(90);
  z_axis.write(90);
  chute.write(CHUTE_CLOSE);
  PIDx.SetSampleTime(10);
  PIDy.SetSampleTime(10);
  PIDx.SetOutputLimits(-PID_LIM, PID_LIM);
  PIDy.SetOutputLimits(-PID_LIM, PID_LIM);
  PIDx.SetMode(AUTOMATIC);
  PIDy.SetMode(AUTOMATIC);
}

void Rocket::pad_idle() {
  digitalWrite(RED_LED,LOW);
  digitalWrite(GREEN_LED,HIGH);
  digitalWrite(BLUE_LED,LOW); 
  //make buzzer beep and update sensors waiting for liftoff
  gyros = sensors.getGyros();
  accels = sensors.getAccel();
  ypr = sensors.getAttitude();
  baro = sensors.getAltitude();
  flight_start = millis();
}

void Rocket::set() {
  gyros = sensors.getGyros();
  accels = sensors.getAccel();
  ypr = sensors.getAttitude();
  baro = sensors.getAltitude();
  flight_time = millis() - flight_start;
  // Makes sure the sides are correct in integration.
  Setpointx = ypr.y;
  Setpointy = ypr.x;
  digitalWrite(RED_LED,HIGH);
  digitalWrite(GREEN_LED,HIGH);
  digitalWrite(BLUE_LED,HIGH);
}
  

void Rocket::ascent() {
  digitalWrite(RED_LED,HIGH);
  digitalWrite(GREEN_LED,LOW);
  digitalWrite(BLUE_LED,LOW);
  gyros = sensors.getGyros();
  accels = sensors.getAccel();
  ypr = sensors.getAttitude();
  baro = sensors.getAltitude();
  flight_time = millis() - flight_start;
  Inputx = ypr.y; //again be ready to change these based on the orienttion of your IMU
  Inputy = ypr.x; //again be ready to change these based on the orienttion of your IMU
  PIDx.Compute();
  PIDy.Compute();
  servo_y = Outputy * SERVO_RATIO;
  servo_z = Outputx * SERVO_RATIO;
  y_axis.write(servo_y + 90);
  z_axis.write(servo_z + 90);
}

void Rocket::coast() {
  digitalWrite(RED_LED,HIGH);
  digitalWrite(GREEN_LED,HIGH);
  digitalWrite(BLUE_LED,HIGH);
  gyros = sensors.getGyros();
  accels = sensors.getAccel();
  ypr = sensors.getAttitude();
  baro = sensors.getAltitude();
  flight_time = millis() - flight_start;
}

void Rocket::apogee() {
  digitalWrite(RED_LED,HIGH);
  digitalWrite(GREEN_LED,HIGH);
  digitalWrite(BLUE_LED,LOW);
  gyros = sensors.getGyros();
  accels = sensors.getAccel();
  ypr = sensors.getAttitude();
  baro = sensors.getAltitude();
  flight_time = millis() - flight_start;
}

void Rocket::descent() {
  digitalWrite(RED_LED,LOW);
  digitalWrite(GREEN_LED,LOW);
  digitalWrite(BLUE_LED,HIGH);
  gyros = sensors.getGyros();
  accels = sensors.getAccel();
  ypr = sensors.getAttitude();
  baro = sensors.getAltitude();
  flight_time = millis() - flight_start;
  chute.write(CHUTE_OPEN);
  parachute = 1;
}

void Rocket::landed() {
  digitalWrite(RED_LED,HIGH);
  digitalWrite(GREEN_LED,LOW);
  digitalWrite(BLUE_LED,HIGH);
  gyros = sensors.getGyros();
  accels = sensors.getAccel();
  ypr = sensors.getAttitude();
  baro = sensors.getAltitude();
  flight_time = millis() - flight_start;
}

void Rocket::updateState(){
  float input;
  input = (analogRead(A4) * 3.3) / 1024.0;
  voltage = input * 11;
  flight_time = millis() - flight_start;
  if (flight_time > 20000) {
    chute.write(CHUTE_OPEN);
    parachute = 1;
  }
}
