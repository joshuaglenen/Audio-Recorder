# Audio-Recorder
Records audio using an electret mic and preamp into a 12 bit adc and stores the data on an SD card with an esp32-s3.

I designed a lightweight audio recorder that samples audio when a button is pushed. The audio is stored in a new file on the SD card which can be erased with a long press of the button. The porgram monitors SD card storage and is meant for 16bit audio sampling using the ADC onboard the ESP32-S3-WROOM2. The PCB contains it's own Li Ion battery, battery managment system, buck converter, LED state indicater, LED charging indicator, USB C serial port for charging and programming, a micro SD card reader, a mic with MAX9814 preamplifier and RC filter.


<img width="3507" height="2480" alt="Untitled" src="https://github.com/user-attachments/assets/8034e723-c1d5-44f0-b7af-ac122d7a806e" />
Diagram 1: Completed circuit

<img width="863" height="591" alt="Capture" src="https://github.com/user-attachments/assets/72116734-36f5-41b2-b972-c85a7a47114a" />
Figure 2: Completed PCB
