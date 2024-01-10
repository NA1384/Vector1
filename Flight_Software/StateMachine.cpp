#include "StateMachine.h"
#define LIFT_THRESH 11.27
#define RED_LED              PIN_USB_HOST_ENABLE
#define GREEN_LED            PIN_LED_RXL
#define BLUE_LED             PIN_LED_TXL

int launchCheck;
int coast;
int decentCheck;
int LandedCheck;
const int chipSelect = A5; // CS pin on sd card
int prev_file_indx = 0; // used for file naming
String fileName = "000";
long id = 1;              //Store the id# of our reading
String sd_saver(File dir){
  while(true){
    // iterate all files to ensure no overwrites
    File entry = dir.openNextFile();
    if (!entry){
      break;
    }
    // naming routine
    String entry_name = entry.name();
    if ((entry_name.substring(4,7)).toInt()>=prev_file_indx){
      prev_file_indx = (entry_name.substring(4,7)).toInt()+1;
      if (prev_file_indx>=100){
        fileName = String(prev_file_indx);
      } else if (prev_file_indx>=10){
        fileName = "0"+String(prev_file_indx);
      } else{
        fileName = "00"+String(prev_file_indx);
      }
    }
    entry.close();
  }
  return fileName;
}

// Data Logging
File main_folder; // initialize folder for saving
File dataFile; // initialize sd file

StateMachine::StateMachine() {
  Serial.println("SM init");
  Rocket();
  current_state = PAD_IDLE; // default state
  // verify SD card is working
  if (!SD.begin(chipSelect)) {
    digitalWrite(RED_LED,HIGH);
    digitalWrite(GREEN_LED,LOW);
    digitalWrite(BLUE_LED,LOW); 
    while (1) {}
  }
  main_folder = SD.open("/");
  fileName = sd_saver(main_folder);

  // SD Card writing and saving  
  dataFile = SD.open("DATA"+fileName+".csv",FILE_WRITE);
  // if the file is valid, write to it:
  if(dataFile){
    dataFile.println("Log Number, Time (ms), Flight Time (ms), Rocket State, Lift Off, Coast Check, Decent Check, Landed Check, Temperature (C), Raw Pressure (Pa), Abs Altitude (m), Rel Altitude (m)," 
    "Parachute, AccelX, AccelY, AccelZ, GyroX, GyroY, GyroZ, Yaw, Pitch, Roll, SetpointX, SetpointY, InputX, InputY, OutputX, OutputY, S_Pitch, S_Yaw, Voltage");
    dataFile.close();
  }
}

void StateMachine::dataLog() {
  // SD save section
  String data_array = "";
  data_array += String(id); // ID number of recording
  data_array += ",";
  data_array += String(millis()); // save milliseconds since start of program
  data_array += ",";
  data_array += String(rocket.flight_time); // save milliseconds since start of program
  data_array += ",";
  data_array += String(current_state); // state machine
  data_array += ",";
  data_array += String(launchCheck); // state machine
  data_array += ",";
  data_array += String(coast); // coast check integer 
  data_array += ",";
  data_array += String(decentCheck); // Decent check integer 
  data_array += ",";
  data_array += String(LandedCheck); // Landed check integer 
  data_array += ",";
  data_array += String(rocket.baro.temp); // save temp
  data_array += ",";
  data_array += String(rocket.baro.pressure); // save pressure in Pa
  data_array += ",";
  data_array += String(rocket.baro.altitude); // save absolute altitude
  data_array += ",";
  data_array += String(rocket.baro.relativeAltitude); // save relative altitude 
  data_array += ",";
  data_array += String(rocket.parachute); // Parachute check integer 
  data_array += ",";
  data_array += String(rocket.accels.x); // save AccelX
  data_array += ",";
  data_array += String(rocket.accels.y); // save AccelY 
  data_array += ",";
  data_array += String(rocket.accels.z); // save AccelZ 
  data_array += ",";
  data_array += String(rocket.gyros.x); // save GyroX
  data_array += ",";
  data_array += String(rocket.gyros.y); // save GyroY
  data_array += ",";
  data_array += String(rocket.gyros.z); // save GyroZ
  data_array += ",";
  data_array += String(rocket.ypr.x); // save Yaw 
  data_array += ",";
  data_array += String(rocket.ypr.y); // save Pitch 
  data_array += ",";
  data_array += String(rocket.ypr.z); // save Roll 
  data_array += ",";
  data_array += String(rocket.Setpointx); // save SetpointX 
  data_array += ",";
  data_array += String(rocket.Setpointy); // save SetpointY 
  data_array += ",";
  data_array += String(rocket.Inputx); // save InputX 
  data_array += ",";
  data_array += String(rocket.Inputy); // save InputY 
  data_array += ",";
  data_array += String(rocket.Outputx); // save OutputX
  data_array += ",";
  data_array += String(rocket.Outputy); // save OutputY
  data_array += ",";
  data_array += String(rocket.servo_z); // save OutputY
  data_array += ",";
  data_array += String(rocket.servo_y); // save OutputY
  data_array += ",";
  data_array += String(rocket.voltage); // save OutputY

  // Order of the inputs
  //dataFile.println("ID, Time (ms), Pad Idle, Lift Off, Apogee, Temperature (C), Raw Pressure (Pa), Abs Altitude (m), Rel Altitude (m)," 
  // "Decent Check, AccelX, AccelY, AccelZ, GyroX, GyroY, GyroZ, Yaw, Pitch, Roll, SetpointX, SetpointY, InputX, InputY, OutputX, OutputY");
  // SD Card writing and saving  
  dataFile = SD.open("DATA"+fileName+".csv",FILE_WRITE);
  // if the file is valid, write to it:
  if(dataFile){
    dataFile.println(data_array);
    dataFile.close();
  }
  //increment ID #
  id++;
  rocket.updateState();
}

bool StateMachine::liftOffCheck() {
 if (launchCheck > 0) {
  return true;
  }
 if(rocket.accels.x > LIFT_THRESH){ //need to check acceleration values on vertical axis  
    launchCheck++;
    return true; 
  }
  return false;
}

bool StateMachine::coastCheck() {
  if (coast > 1) {
    return true;
  }
  if (rocket.accels.x < 2) {
    coast++;
  }
  return false;
}

bool StateMachine::apogeeCheck() {
  float altDiff;
  float lastAlt;
  altDiff = lastAlt - rocket.baro.relativeAltitude;
  lastAlt = rocket.baro.relativeAltitude;
  if (decentCheck > 10) {
    return true;
  }
  if (altDiff > 0) {
    decentCheck++;
  }
  return false;
}

bool StateMachine::landedCheck() {
  if (LandedCheck > 10) {
    return true;
  }
  if (rocket.baro.relativeAltitude < 2) {
    LandedCheck++;
  }
  return false;
}
