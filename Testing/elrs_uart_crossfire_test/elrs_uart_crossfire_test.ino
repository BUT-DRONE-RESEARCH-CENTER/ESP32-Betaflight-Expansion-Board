#include <AlfredoCRSF.h>
#include <HardwareSerial.h>

#define PIN_RX 16
#define PIN_TX 17 

HardwareSerial crsfSerial(2); 
AlfredoCRSF crsf;

void setup() {
  Serial.begin(115200);
  crsfSerial.begin(CRSF_BAUDRATE, SERIAL_8N1, PIN_RX, PIN_TX);
  crsf.begin(crsfSerial);
}

void loop() {
  crsf.update();

  if (crsf.isLinkUp()) {
    // UNCOMMENT the one you want to use:
    
    // outputSerialMonitor(); 
    outputSerialPlotter(); 
  }
  
  delay(20); // Faster refresh for smoother plotting
}

// FORMAT: CH1: 1500 | CH2: 1500 ...
void outputSerialMonitor() {
  for (int i = 1; i <= 16; i++) {
    Serial.print("CH");
    Serial.print(i);
    Serial.print(": ");
    Serial.print(crsf.getChannel(i));
    if (i < 16) Serial.print(" | ");
  }
  Serial.println();
}

// FORMAT: 1500 1500 1500 ... (Standard for Arduino Plotter)
void outputSerialPlotter() {
  for (int i = 1; i <= 16; i++) {
    Serial.print(crsf.getChannel(i));
    if (i < 16) {
      Serial.print(" "); // Plotter uses spaces or tabs as delimiters
    }
  }
  Serial.println(); // Newline signals the end of the data point
}