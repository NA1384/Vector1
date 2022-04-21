# Vector 1
Vector 1 is a thrust-vectoring model rocket computer with all the hardware required to keep your TVC model rocket upright. 
Full flight software will be released soon for a complete full-stack solution.

If you have any question, comments or advice on how to use the flight computer don't hesitate to open an issue.

<div style="display:flex">
<img src="./Images/Brackets.jpg" height="400" />
<img src="./Images/Front.jpg" height="400" width="300"/>
<img src="./Images/Back.jpg" height="400" width="300"/>
</div>

## Feature Set

- Main microcontroller is an Atmel ATSAMD21G18 running at 48 MHz with 256 KB of flash. The chip run on an 32-Bit ARM® Cortex® M0+ core making it plenty powerful to handle all computations required during flight. 
- High performance sensor suite with the Bosch BMI088 6-axis IMU and the TE MS5611 high accuracy barometer to provide the most precise and accurate data for the flight computer during flight.
- 3 servo outputs, two for thrust-vectoring gimbal and one for a mechanical chute ejection system or any other necessary servo actions.
- 4 pyrotechnic outputs for nichrome wire, black powder parachute ejection charges or igniting a secondary motor. Pyro channels 3 & 4 are PWM capable allowing for variable current outputs through software making them more suitable for nichrome wire outputs.
- TODO

The flight computer is fully compatible with the Arduino IDE, PlatformIO or Atmel Studio. All code is written in C++.
