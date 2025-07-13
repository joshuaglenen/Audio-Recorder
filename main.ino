#include <SD_MMC.h>

#define micPin 4
#define pushButton 47
#define rgb_r 37
#define rgb_g 36
#define rgb_b 35
#define SD_MMC_CMD 38 //Please do not modify it.
#define SD_MMC_CLK 39 //Please do not modify it. 
#define SD_MMC_D0  40 //Please do not modify it.

const uint32_t MAX_FILE_SIZE = 950UL * 1024UL * 1024UL;  // 950 MB limit
File audioFile;
bool recording = false;
bool full = false;
long lastSaveTime = 0;

void setup() {
  Serial.begin(115200);
  analogReadResolution(12);
  analogSetAttenuation(ADC_11db);

  pinMode(rgb_r, OUTPUT);
  pinMode(rgb_g, OUTPUT);
  pinMode(rgb_b, OUTPUT);
  pinMode(pushButton, INPUT_PULLUP);

  //detect sd card and availible space
  SD_MMC.setPins(SD_MMC_CLK, SD_MMC_CMD, SD_MMC_D0);
  if (!SD_MMC.begin("/sdcard", true, true, SDMMC_FREQ_DEFAULT, 5)) {
    Serial.println("SD Card Mount Failed");
    rgb(true, false, false);  // Red
    while (true);
  }
  uint8_t cardType = SD_MMC.cardType();
  if(cardType == CARD_NONE){
      Serial.println("No SD_MMC card attached");
      return;
  }
  Serial.print("SD_MMC Card Type: ");
  if(cardType == CARD_MMC){
      Serial.println("MMC");
  } else if(cardType == CARD_SD){
      Serial.println("SDSC");
  } else if(cardType == CARD_SDHC){
      Serial.println("SDHC");
  } else {
      Serial.println("UNKNOWN");
  }
  uint64_t maxSpace = SD_MMC.totalBytes();
  uint64_t usedSpace = SD_MMC.usedBytes();
  if(0.01>(float)(maxSpace-usedSpace)/(float)maxSpace)
  {
    full = true;
    rgb(true, false, false);
    Serial.println("SD Card Full");
  }
  else
  {
    rgb(false, true, false);  // Green ready
  }
}

void loop() {
  checkPushButton();

  if (recording && !full) {
    static unsigned long lastSampleTime = micros();
    if (micros() - lastSampleTime >= 62) {  // 16kHz
      lastSampleTime += 62;
      int sample = analogRead(micPin);
      sample = constrain(sample, 0, 4095);
      int16_t sample16 = (sample - 2048) * 16;  // Convert to signed 16-bit
      audioFile.write((uint8_t*)&sample16, 2); 

      if(millis()-lastSaveTime>60000)
      {
        lastSaveTime = millis();
        updateWavHeader(audioFile);
      }


      //SD card is full
      if (audioFile.size() >= MAX_FILE_SIZE) {
        recording = false;
        full = true;
        updateWavHeader(audioFile);
        audioFile.close();
        rgb(true, false, false);  // Red LED = SD full
      }

    
    }
  }
}

void checkPushButton() {
  static bool buttonState = HIGH;              
  static bool lastButtonState = HIGH;
  static unsigned long lastDebounceTime = 0;
  static unsigned long pressStartTime = 0;

  bool reading = digitalRead(pushButton);

  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > 200) {
    if (reading != buttonState) {
      buttonState = reading;

      if (buttonState == LOW) {  // Button just pressed
        pressStartTime = millis();
      }

      if (buttonState == HIGH) {  // Button just released
        unsigned long pressDuration = millis() - pressStartTime;

        if (pressDuration >= 5000) erase();
        else if (pressDuration >= 500)  toggleRecording();  
        }
      }
    }
  }

  lastButtonState = reading;
}

//creates a new wav file and enables recording or stops the recording
void toggleRecording() {
  static char filename[20] = "/audio001.wav";
  
  if(!recording)
  {
    rgb(true, true, true);  // White 
    
    int fileNumber = 0;
    do {
      sprintf(filename, "/audio%03d.wav", fileNumber++);
    } while (SD_MMC.exists(filename));  // Find unused filename

    audioFile = SD_MMC.open(filename, FILE_WRITE);
    writeWavHeader(audioFile, 16000, 16);
    Serial.print("Recording started for ");
     Serial.println(filename);
  }
  else
  {
    rgb(false, false, false);
    updateWavHeader(audioFile);
    audioFile.close();
    Serial.println("Recording stopped");
  }
  
  recording = !recording;
}

void erase() {
  rgb(true, false, false);  // Red
  recording = false;
  audioFile.close();
  char filename[20] = "/audio001.wav";
  int fileNumber = 0;
  do {
    sprintf(filename, "/audio%03d.wav", fileNumber++);
  } while (SD_MMC.exists(filename))
    {
       SD_MMC.remove(filename);
    }
      
  Serial.println("All audio erased.");
  full = false;
}

void rgb(bool r, bool g, bool b) {
  digitalWrite(rgb_r, r ? HIGH : LOW);
  digitalWrite(rgb_g, g ? HIGH : LOW);
  digitalWrite(rgb_b, b ? HIGH : LOW);
}

void writeWavHeader(File file, uint32_t sampleRate, uint16_t bitsPerSample) {
  uint32_t byteRate = sampleRate * 2;
  uint16_t blockAlign = 2;

  file.write("RIFF", 4);
  file.write((uint8_t*)"\x00\x00\x00\x00", 4);  // Placeholder for file size
  file.write("WAVE", 4);
  file.write("fmt ", 4);
  file.write((uint8_t*)"\x10\x00\x00\x00", 4);  // Subchunk1Size (16)
  file.write((uint8_t*)"\x01\x00", 2);          // AudioFormat (PCM)
  file.write((uint8_t*)"\x01\x00", 2);          // NumChannels (1)
  file.write((uint8_t*)&sampleRate, 4);
  file.write((uint8_t*)&byteRate, 4);
  file.write((uint8_t*)&blockAlign, 2);
  file.write((uint8_t*)&bitsPerSample, 2);
  file.write("data", 4);
  file.write((uint8_t*)"\x00\x00\x00\x00", 4);  // Placeholder for data size
}

void updateWavHeader(File file) {
  uint32_t fileSize = file.size();
  uint32_t dataChunkSize = fileSize - 44;

  file.seek(4);
  uint32_t chunkSize = fileSize - 8;
  file.write((uint8_t*)&chunkSize, 4);

  file.seek(40);
  file.write((uint8_t*)&dataChunkSize, 4);
}
