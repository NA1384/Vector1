#include "StateMachine.h"

StateMachine* sm;
void setup() {
  sm = new StateMachine();
}

void loop() {
  sm->update();
  sm->dataLog();
}
