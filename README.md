# Macbook-Lid-Angle-Sensor-Calibration-Tool
This tool can be used to calibrate the new lid angle sensor on the macbook A2141 A2442 A2485 A2779 A2780 A2681.
# Operating modes
The programmer has 3 operating modes, the modes are switched by a short push of a button and switched in a circle. You can tell which mode is currently selected by the blinking of the LED (1, 2 and 3, respectively). Each of the modes is activated by pressing the button for a long time (1 second).

In the mode 1, calibration is performed for the PRO series macbooks (A2141, A2442, A2485, A2779, A2780), if the calibration was successfully recorded, the LED will light up for 3 seconds.

In the mode 2, calibration is performed for the AIR series macbooks (A2681), if the calibration record was completed successfully, the LED will light up for 3 seconds.

In mode 3, the function of recognizing the current angle is activated (if the lid is open, the LED lights up, closed - goes out).

Before recording in modes 1 and 2, the tool makes checks for the possibility of flashing, if the sensor is not connected, the LED will blink 3 times, if a sensor removed from another device (calibrated by Apple) is connected, it will blink 6 times.

Demonstration of the device operation:
https://www.youtube.com/watch?v=4y4Nq-6QuaA
# Required for assembly

I used a controller NodeMCU v3 (ESP8266) use any controller with an spi interface that you have. 


Connectors:

503548-1220 molex - for Pro models

BM28P0.6-10DS-0.35V Hirose - For air models, also installed on iphone 11 (J_SIM_K)

As a connector board, I used a piece of the motherboard from the macbook A2141

![NodeMCU pinout](https://github.com/Vladislav98759/Macbook-Lid-Angle-Sensor-Calibration-Tool/assets/33593193/6550a118-871b-4d62-8de6-cfd9e4de9bab)

![Connectors pinout](https://github.com/Vladislav98759/Macbook-Lid-Angle-Sensor-Calibration-Tool/assets/33593193/dc2e7380-8eec-4fec-9e8c-6d418c56af62)

![IMG_0431](https://github.com/Vladislav98759/Macbook-Lid-Angle-Sensor-Calibration-Tool/assets/33593193/05c358fa-7ced-46c6-9795-95fb02d9ed8f)
