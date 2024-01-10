#include <Wire.h>
#include <Arduino.h>
#include "RocketDo.h"
#include <SPI.h>            // SPI protocol library required for SD library
#include <SD.h>

enum State {
  PAD_IDLE,
  SET,
  ASCENT,
  COAST,
  APOGEE,
  DESCENT,
  LANDED
};

class StateMachine {
  public:
    StateMachine();
    void dataLog();
    bool liftOffCheck();
    bool coastCheck();
    bool apogeeCheck();
    bool landedCheck();
    void update() {
      switch (current_state) {
        case PAD_IDLE:
          rocket.pad_idle();
          if (liftOffCheck()) { //need to make a lift off check
            rocket.flight_start = millis();
            //Serial.println(rocket.flight_start);
            current_state = SET;
          }
          break;
        case SET:
          rocket.set();
          current_state = ASCENT;
          break;
        case ASCENT:
          rocket.ascent();
          if (coastCheck()) {
            current_state = COAST;
          }
          break;
        case COAST:
          rocket.coast();
          if (apogeeCheck()) {
            //SerialUSB.println("APOGEE!!!");
            current_state = APOGEE;
          }
          break;
        case APOGEE:
          current_state = DESCENT;
          break;
        case DESCENT:
          rocket.descent();
          if (landedCheck()) {
           current_state = LANDED;
          }
          break;
        case LANDED:
          rocket.landed();
          break;
        default:
        // nothing to go here
          break;
      }
    }
  private:
    State current_state;
    Rocket rocket;
};
