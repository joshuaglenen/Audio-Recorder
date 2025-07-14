# Audio-Recorder
Records audio using a mic and preamp using a 12 bit adc and stores the data on an SD card.

### Methods
I designed a lightweight audio recorder that samples audio when a button is pushed. The audio is stored in a new file on the SD card which can be erased with a long press of the button. 
The program monitors SD card storage and is meant for 16bit audio sampling using the default ADC onboard the ESP32-S3-WROOM2. 

The PCB contains: 
<li>ESP32-S3-WROOM2</li>
<li>It's own Li Ion battery</li>
<li>Battery managment system</li>
<li>Buck converter</li>
<li>Power switch</li>
<li>Push button</li>
<li>RGB state indicater</li>
<li>RGB charging indicator</li> 
<li>USB C serial port for charging and programming</li> 
<li>Micro SD card reader</li> 
<li>Mic with MAX9814 preamplifier and RC filter</li>


<img width="3507" height="2480" alt="Untitled" src="https://github.com/user-attachments/assets/8034e723-c1d5-44f0-b7af-ac122d7a806e" />

Diagram 1: Completed circuit

<img width="932" height="545" alt="Capture" src="https://github.com/user-attachments/assets/6eb4369d-f50a-4e9e-bbb9-da3e2df4f0dc" />

Figure 2: Completed PCB
